#include "QVTFPlugin.h"

#include <cstring>

#include <QImage>
#include <QVariant>

QImageIOPlugin::Capabilities QVTFPlugin::capabilities(QIODevice* device, const QByteArray& format) const {
	if (format.isNull() && !device) {
		return {};
	}
	if (!format.isNull() && format.toLower() != "vtf") {
		return {};
	}
	if (device && !QVTFHandler::canRead(device)) {
		return {};
	}
	return CanRead;
}

QImageIOHandler* QVTFPlugin::create(QIODevice* device, const QByteArray& format) const {
	if (format.isNull() || format.toLower() == "vtf") {
		auto* handler = new QVTFHandler;
		handler->setDevice(device);
		handler->setFormat("vtf");
		return handler;
	}
	return nullptr;
}

bool QVTFHandler::canRead(QIODevice* device) {
	if (!device) {
		return false;
	}
	const QByteArray fourCC = device->peek(sizeof(uint32_t));
	if (fourCC.size() != sizeof(uint32_t)) {
		return false;
	}
	const auto signature = *reinterpret_cast<const uint32_t*>(fourCC.data());
	return signature == vtfpp::VTF_SIGNATURE || signature == vtfpp::VTFX_SIGNATURE || signature == vtfpp::VTF3_SIGNATURE;
}

bool QVTFHandler::canRead() const {
	switch (this->state) {
		using enum State;
		case Read:
			return this->currentFrame < this->vtf.getFrameCount();
		case Error:
			return false;
		default:
		case Ready:
			break;
	}
	return canRead(this->device());
}

int QVTFHandler::currentImageNumber() const {
	if (this->state != State::Read) {
		return -1;
	}
	return this->currentFrame;
}

QRect QVTFHandler::currentImageRect() const {
	if (this->state != State::Read) {
		return {};
	}
	return {0, 0, this->vtf.getWidthWithoutPadding(), this->vtf.getHeightWithoutPadding()};
}

int QVTFHandler::imageCount() const {
	if (this->state != State::Read) {
		return -1;
	}
	return this->vtf.getFrameCount();
}

bool QVTFHandler::jumpToImage(int imageNumber) {
	if (!this->read() || imageNumber < 0 || imageNumber >= this->vtf.getFrameCount()) {
		return false;
	}
	this->currentFrame = imageNumber;
	return true;
}

bool QVTFHandler::jumpToNextImage() {
	if (!this->read() || this->currentFrame >= this->vtf.getFrameCount()) {
		return false;
	}
	this->currentFrame++;
	return true;
}

int QVTFHandler::loopCount() const {
	if (this->state != State::Read) {
		return 0;
	}
	if (this->vtf.getFrameCount() > 1) {
		return -1;
	}
	return 0;
}

int QVTFHandler::nextImageDelay() const {
	return 250; // ms
}

QVariant QVTFHandler::option(ImageOption option) const {
	switch (option) {
		case Animation:
			return true;
		case Size:
			if (this->state == State::Read) {
				return QSize{this->vtf.getWidthWithoutPadding(), this->vtf.getHeightWithoutPadding()};
			}
			break;
		case ImageFormat:
			if (this->state == State::Read) {
				if (vtfpp::ImageFormatDetails::decompressedAlpha(this->vtf.getFormat())) {
					return QImage::Format_ARGB32;
				}
				return QImage::Format_RGB888;
			}
			break;
		//case Description:
		//	if (this->state == State::Read) {
		//		return QString{};
		//   }
		//	break;
		default:
			break;
	}
	return {};
}

bool QVTFHandler::supportsOption(ImageOption option) const {
	switch (option) {
		case Animation:
		case Size:
		case ImageFormat:
		// todo
		//case Description:
			return true;
		default:
			return false;
	}
}

bool QVTFHandler::read(QImage* image) {
	if (!image) {
		return false;
	}
	if (!this->read() || this->currentFrame >= this->vtf.getFrameCount()) {
		return false;
	}

	const auto width  = this->vtf.getWidthWithoutPadding();
	const auto height = this->vtf.getHeightWithoutPadding();
	QImage::Format format;
	std::vector<std::byte> imageData;
	if (vtfpp::ImageFormatDetails::decompressedAlpha(this->vtf.getFormat())) {
		format = QImage::Format_RGBA8888;
		imageData = this->vtf.getImageDataAsRGBA8888(0, this->currentFrame);
	} else {
		format = QImage::Format_RGB888;
		imageData = this->vtf.getImageDataAs(vtfpp::ImageFormat::RGB888, 0, this->currentFrame);
	}
	if (imageData.empty()) {
		return false;
	}

	if (image->width() != width || image->height() != height || image->format() != format) {
		*image = {width, height, format};
		if (image->isNull()) {
			return false;
		}
	}
	memcpy(image->bits(), imageData.data(), imageData.size());

	// todo
	/*
	if (this->currentFrame == 0 && !image->textKeys().contains("Version")) {
		const auto reflectivity = this->vtf.getReflectivity();

		SVTFImageFormatInfo formatInfo = VTFLib::CVTFFile::GetImageFormatInfo(vtf.GetFormat());

		image->setText("Version",QString::fromUtf8("%1").arg(vtf.GetMajorVersion()).arg(vtf.GetMinorVersion()));
		image->setText("Format",QLatin1String(formatInfo.lpName));
		image->setText("Depth",QString::number(vtf.GetDepth()));
		image->setText("Bumpmap Scale",QString::number(vtf.GetBumpmapScale()));
		image->setText("Reflectivity",QString{"rgb(%1%% %2%% %3%%)"}.arg(reflectivity[0] * 100).arg(reflectivity[1] * 100).arg(reflectivity[2] * 100));
		image->setText("Faces",QString::number(vtf.GetFaceCount()));
		image->setText("Mipmaps",QString::number(vtf.GetMipmapCount()));
		image->setText("Frames",QString::number(vtf.GetFrameCount()));
		image->setText("Start Frame",QString::number(vtf.GetStartFrame()));
		image->setText("Flags",vtfFlagNames(vtf.GetFlags()).join(QLatin1String(", ")));
		image->setText("Bits Per Pixel",QString::number(formatInfo.uiBitsPerPixel));
		image->setText("Alpha Channel",QLatin1String(formatInfo.uiAlphaBitsPerPixel > 0 ? "True" : "False"));
		image->setText("Compressed",QLatin1String(formatInfo.bIsCompressed ? "True" : "False"));
	}
	*/

	this->currentFrame++;
	return true;
}

bool QVTFHandler::read()  {
	switch (this->state) {
		using enum State;
		case Read:
			return true;
		case Error:
			return false;
		case Ready:
			break;
	}
	const QByteArray data = device()->readAll();
	this->vtf = vtfpp::VTF{{reinterpret_cast<const std::byte*>(data.data()), static_cast<std::span<const std::byte>::size_type>(data.size())}};
	if (!this->vtf) {
		this->state = State::Error;
		return false;
	}
	this->currentFrame = 0;
	this->state = State::Read;
	return true;
}

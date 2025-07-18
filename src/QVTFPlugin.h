#pragma once

#include <QImageIOPlugin>
#include <vtfpp/VTF.h>

class QVTFPlugin : public QImageIOPlugin {
	Q_OBJECT;
	Q_CLASSINFO("author", "Laura Lewis");
	Q_CLASSINFO("url", "https://github.com/craftablescience/qvtfpp");
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "QVTFPlugin.json");

public:
	Capabilities capabilities(QIODevice* device, const QByteArray& format) const override;

	QImageIOHandler* create(QIODevice* device, const QByteArray& format) const override;
};

class QVTFHandler : public QImageIOHandler {
public:
	QVTFHandler() = default;

	[[nodiscard]] bool canRead() const override;

	[[nodiscard]] int currentImageNumber() const override;

	[[nodiscard]] QRect currentImageRect() const override;

	[[nodiscard]] int imageCount() const override;

	[[nodiscard]] bool jumpToImage(int imageNumber) override;

	[[nodiscard]] bool jumpToNextImage() override;

	[[nodiscard]] int loopCount() const override;

	[[nodiscard]] int nextImageDelay() const override;

	[[nodiscard]] QVariant option(ImageOption option) const override;

	[[nodiscard]] bool read(QImage *image) override;

	[[nodiscard]] bool supportsOption(ImageOption option) const override;

	[[nodiscard]] static bool canRead(QIODevice *device);

	[[nodiscard]] bool read();

protected:
	enum class State {
		Ready,
		Read,
		Error
	};

	State state = State::Ready;
	int currentFrame = 0;
	vtfpp::VTF vtf;
};

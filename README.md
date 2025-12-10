# QVTF++

A QImageIO plugin to load VTF textures, based on panzi's QVTF plugin.
Using this you will be able to view VTFs in Qt programs like Gwenview.

Unlike the original it supports VTF v7.6, as well as console VTF variants (X360/PS3).
It will also use the proper names for flags from different VTF versions.

## Build & Install

- Substitute `<GENERATOR>` with either `DEB` (Debian-based systems) or
`RPM` (Fedora-based systems).
- Substitute `<VERSION>` with the version of the plugin.

```bash
git clone https://github.com/craftablescience/qvtfpp.git
mkdir qvtfpp/build
cd qvtfpp/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR="<GENERATOR>" -DQVTFPP_USE_LTO=ON
cmake --build .
cpack

# Debian-based
sudo apt install ./qvtf++-<VERSION>-Linux.deb
# Fedora-based
sudo dnf install ./qvtf++-<VERSION>-Linux.rpm
```

## Backend

This plugin is powered by a collection of open-source C++20 Source engine parsers called
[sourcepp](https://sourcepp.org).

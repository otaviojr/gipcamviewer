# GIpCamViewer
A simple Gnome Gtk3 IP camera viewer for linux.

![screenshot](./screenshot.png?raw=true "Screenshot")
![screenshot](./screenshot1.png?raw=true "Screenshot")

# Cameras Support
Camera support is done through lua drivers. At the present moment only foscam cameras are supported.

# Drivers
- Foscam FI8918W (Works with most foscam SD cameras)
- Foscam FI9821P (Works with most foscam HD cameras)

# Compiling
Meson/ninja are needed in order to compile GIpCamViewer.

```bash
apt-get update
apt-get install git meson build-essential autotools-dev autoconf pkg-config libgtk-3-dev libgstreamer1.0-dev libjson-glib-dev libvlc-dev liblua5.3-dev
mkdir gipcamviewer
cd gipcamviewer
git clone https://github.com/otaviojr/gipcamviewer.git .
mkdir build
cd build
meson --prefix=<your prefix>
ninja build install
```

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
git clone https://github.com/otaviojr/gipcamviewer.git .
cd gipcamviewer/build
meson --prefix=<your prefix>
ninja build install
```

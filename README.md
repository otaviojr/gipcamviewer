# GIpCamViewer
A simple GTK3 IP camera viewer for Gnome

# Cameras Support
Camera support is done through lua drivers. At the present moment only foscam cameras are supported.

# Drivers
- Foscam FI8918W (Will support most SD cameras)
- Foscam FI9821P (Will support most HD cameras)

# Compiling
Meson/ninja are needed in order to compile GIpCamViewer.

```bash
git clone https://github.com/otaviojr/gipcamviewer.git .
cd gipcamviewer/build
meson
ninja build
```

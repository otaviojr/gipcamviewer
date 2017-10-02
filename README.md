# GIpCamViewer
A simple GTK3 IP camera viewer for Gnome

# Cameras Support
Camera support is done trough lua drivers. At the present moment only foscam cameras are supported.  

###drivers
- Foscam FI8918W (Will support most SD cameras)
- Foscam FI9821P (Will support most HD cameras)

###compiling
Meson/ninja is nedded in order to compile GIpCamViewer

```
git clone https://github.com/otaviojr/gipcamviewer.git .
cd build
meson
ninja build
```

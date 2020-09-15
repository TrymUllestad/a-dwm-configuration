My configuration of dwm
=======================
dwm, dynamic window manager, is a tiling window manager created by the team over at [suckless.org](https://suckless.org/) for X.
My configuration changes the default key bindings and add functionality through patches gotten from the [patches section](https://dwm.suckless.org/patches/) over at their website.

Requirements
------------

dwm requires the Xlib header package and make to compile, the laptop xf86 keys use pamixer and light to change audio and screen brightness.

Building from source
--------------------

To build from source, open the folder in your terminal and type:
```
    sudo make clean install
```
This will build dwm and place the binary in /usr/local/bin/.

Launching dwm
-------------

To launch dwm using xinit, replace or comment out any `exec something` with:

```
    exec dwm
```

Configuration
-------------

The configuration of dwm is done by creating a custom `config.h` and recompiling the source code.

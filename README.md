# My configuration of dwm
dwm, dynamic window manager, is a tiling window manager created by the team over at [suckless.org](https://suckless.org/) for X.
My configuration changes the default key bindings and add functionality through patches gotten from the [patches section](https://dwm.suckless.org/patches/) over at their website.

You might find the key bindings to be weird and seemingly randomly placed; this is because my keyboard uses a norwegian-dvorak layout. 
I hope to add comments to some of the bindings to make them more qwerty-friendly; other bindings are set based on which command they execute (mod+t for Tiling mode).

dwm requires the Xlib header package and make to compile, and some keybindings use pamixer and light to change audio and screen brightness.

Underneath is the rest of the original README from the vanilla dwm.

dwm - dynamic window manager
============================
dwm is an extremely fast, small, and dynamic window manager for X.


Requirements
------------
In order to build dwm you need the Xlib header files.


Installation
------------
Edit config.mk to match your local setup (dwm is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install dwm (if
necessary as root):

    make clean install


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

In order to display status info in the bar, you can do something
like this in your .xinitrc:

    while xsetroot -name "`date` `uptime | sed 's/.*,//'`"
    do
    	sleep 1
    done &
    exec dwm


Configuration
-------------
The configuration of dwm is done by creating a custom config.h
and (re)compiling the source code.

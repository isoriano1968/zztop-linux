# zztop-linux
Gtk/Linux  based version of the MNT ZZ Top application

Howto:

make        - will create ZZTop
make clean  - clean up folder

As we are accessing a device you have to sudo ./ZZTop

Currently reads from /dev/i2c-0 address 0x3b and register 0xe0 .. but no output

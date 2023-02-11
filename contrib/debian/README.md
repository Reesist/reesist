
Debian
====================
This directory contains files used to package reesistd/reesist-qt
for Debian-based Linux systems. If you compile reesistd/reesist-qt yourself, there are some useful files here.

## reesist: URI support ##


reesist-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install reesist-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your reesist-qt binary to `/usr/bin`
and the `../../share/pixmaps/reesist128.png` to `/usr/share/pixmaps`

reesist-qt.protocol (KDE)


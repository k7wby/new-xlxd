﻿# XLXD (or XRFD)

The XLX Multiprotocol Gateway Reflector Server is part of the software system for the D-Star Network. The sources are published under GPL Licenses.

## Introduction

This will build **either** a new kind of XLX reflector **or** a tri-mode XRF reflector. This build support *dual-stack* operation, so the server on which it's running, must have both an IPv4 and IPv6 routable address if you are going to configure a dual-stack reflector. This XLX is different from the original because it can support out-going DExtra links, by adding a new DExtra Peer type *and* it has many changes designed to increase reliability and stability. The XRF reflector built from this source supports inbound DExtra, DPlus **and DCS connections**. Like XLX, XRF also supports out-going DExtra linking. Please note that for now, only one DExtra peer link per node is supported.

This is an improved version of the multi-protocol Reflector. Nearly all std::vector containers have been replaced with std::list containers. This is a far better choice for any collection where it is common to delete elements that are not at the end of the collection. Also in this package, no classes are derived from any standard containers. Because standard containers don't have a virtual destructor, derriving from them is highly ill-advised and while the origin XLX server worked using such derivations, it represents a possible serious problem when considering future development. Also, the clean-up routines designed to be executed when shutting down were unreachable as designed and this has been fixed. In addtion, long sleep times in certain threads were preventing a polite systemd shutdown and this has been fixed. Please note that it can still take several seconds to complete the shutdown while some execution threads complete their i/o cycle. The C++ warning flag, -W has been turned on and a significant number of warning have been fixed.

Only systemd-based operating systems are supported. Debian or Ubuntu is recommended. If you want to install this on a non-systemd based OS, you are on your own. Also, by default, ambed and xlxd or xrfd are built without gdb support. If you want to add it, modify the `Makefile` in each build directory. Finally, this repository is designed so that you don't have to modify any file in the repository when you build your system. Any file you need to modify to properly configure your reflector will be a file you copy from you locally cloned repo. This makes it easier to update the source code when this repository is updated. Follow the instructions below to build your transcoding XLX reflector or tri-mode XRF reflector.

- 73 de n7tae

## Usage

The packages which are described in this document are designed to install server software which is used for the D-Star network infrastructure. It requires a 24/7 internet connection which can support 20 voice streams or more to connect repeaters and hot-spot dongles!

- The server can build a reflector that support IPv4, IPv6 or both (dual stack).
- The public IP addresses should have a DNS record which must be published in the common host files.

If you want to run this software please make sure that you can provide this service free of charge, like the developer team provides the software and they network infrastructure free of charge! If you don't need transcoding support, you don't need to build or install `ambed`.

## Installation

Below are instructions for building either an XLX or XRF reflector. It is assumed that the XLX system will support transcoding. If this is not the case, you can safely ignore commands associated with building and installing `ambed`.

### After a clean installation of Debian make sure to run update and upgrade

```bash
sudo apt update
sudo apt upgrade
```

### Required packages (some of these will probably already be installed)

```bash
sudo apt install git git-core
sudo apt install apache2 php5
sudo apt install build-essential
sudo apt install g++
```

### Download the repository and enter the directory

```bash
git clone https://github.com/n7tae/new-xlxd.git
cd new-xlxd
```

### Create and edit your blacklist, whitelist and linking files and systemd file

If you are building an XLX reflector:

```bash
cp ../config/xlxd.blacklist .
cp ../config/xlxd.whitelist .
cp ../config/xlxd.interlink .
cp ../config/xlxd.terminal .
```

If you are building an XRF reflector (please note the name changes, especially for the interlink file):

```bash
cp ../config/xlxd.blacklist xrfd.blacklist
cp ../config/xlxd.whitelist xrfd.whitelist
cp ../config/xlxd.interlink xrfd.interlink
cp ../config/xlxd.terminal xrfd.terminal
```

If you are not going to support G3 links, you don't need to copy the .terminal file. Use your favorite editor to modify each of these files. If you want a totally open network, the blacklist and whitelist files are ready to go. The blacklist determine which callsigns can't use the reflector. The whitelist determines which callsigns can use the reflector. The interlink file sets up the XLX<--->XLX inter-linking and/or out-going XRF linking.

### Configuring your reflector

Start the configuration script:

```bash
./rconfig
```

There are only a few things that need to be specified, most importantly, the reflector callsign and the IP addresses for the IPv4 and IPv6 listen ports and a transcoder port, if there is a transcoder. Obviously the transcoder is only specified for an XLX reflector. If you are building an XLX system with a transcoder, you can specify which channels get transcoder support. There are also true/false flags to prevent G3 support and so that you can build executables that will support gdb debugging.

Be sure to write out the configuration files and look over the up to 5 different configration files that are created. The first file, reflector.cfg is the memory file for rconfig so that if you start that script again, it will remember how you left things. There are one or two `.h` files for the reflector and ambed and there are one or two `.mk` files for the reflector and ambed makefiles.

### Compling and installing your system

After you have written your configutation files, you can build and install your system:

```bash
./radmin
```

Use this command to compile and install your system. It can also be used to uninstall your system. It will use the information in reflector.cfg to perform each task. This radmin menu can also perform other tasks like restarting the reflector or transcoder process. It can also be used to update the software, if the system is uninstalled.

### Stoping and starting the services

```bash
sudo systemctl stop xlxd # (or xrfd)
sudo systemctl stop ambed
```

You can start each component by replacing `stop` with `start`, or you can restart each by using `restart`.

### Copy dashboard to /var/www

There are two supplied, one for XRF systems and one for XLX systems.

```bash
sudo cp -r ~/xlxd/dashboard.xlx /var/www/db     # or dashboard.xrf
```

Please note that your www root directory might be some place else. There is one file that needs configuration. Edit the copied files, not the ones from the repository:

- **pgs/config.inc.php** - At a minimum set your email address, country and comment. **Do not** enable the calling home feature if you built an XRF reflector. This feature is for **XLX systems only**.

## Updating xlxd and ambed

Go to the build directory and do a `git pull`. If you notice there is new version of the `Makefile` after you do the `git pull`, you will want to reconcile those new files with your copies **before** you make and install the executables.

## Firewall settings

XLX Server requires the following ports to be open and forwarded properly for in- and outgoing network traffic:

- TCP port 80            (http) optional TCP port 443 (https)
- TCP port 8080          (RepNet) optional
- UDP port 10001         (json interface XLX Core)
- UDP port 10002         (XLX interlink)
- TCP port 22            (ssh) optional  TCP port 10022
- UDP port 42000         (YSF protocol)
- UDP port 30001         (DExtra protocol)
- UPD port 20001         (DPlus protocol)
- UDP port 30051         (DCS protocol)
- UDP port 8880          (DMR+ DMO mode)
- UDP port 62030         (MMDVM protocol)
- UDP port 10100         (AMBE controller port)
- UDP port 10101 - 10199 (AMBE transcoding port)
- UDP port 12345 - 12346 (Icom Terminal presence and request port)
- UDP port 40000         (Icom Terminal dv port)

## YSF Master Server

Pay attention, the XLX Server acts as an YSF Master, which provides 26 wires-x rooms.
It has nothing to do with the regular YSFReflector network, hence you don’t need to register your XLX at ysfreflector.de !

## Copyright

- Copyright © 2016 Jean-Luc Deltombe LX3JL and Luc Engelmann LX1IQ
- Copyright © 2020 Thomas A. Early N7TAE

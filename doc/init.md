Sample init scripts and service configuration for reesistd
==========================================================

Sample scripts and configuration files for systemd, Upstart and OpenRC
can be found in the contrib/init folder.

    contrib/init/reesistd.service:    systemd service unit configuration
    contrib/init/reesistd.openrc:     OpenRC compatible SysV style init script
    contrib/init/reesistd.openrcconf: OpenRC conf.d file
    contrib/init/reesistd.conf:       Upstart service configuration file
    contrib/init/reesistd.init:       CentOS compatible SysV style init script

Service User
---------------------------------

All three Linux startup configurations assume the existence of a "reesistcore" user
and group.  They must be created before attempting to use these scripts.
The OS X configuration assumes reesistd will be set up for the current user.

Configuration
---------------------------------

At a bare minimum, reesistd requires that the rpcpassword setting be set
when running as a daemon.  If the configuration file does not exist or this
setting is not set, reesistd will shutdown promptly after startup.

This password does not have to be remembered or typed as it is mostly used
as a fixed token that reesistd and client programs read from the configuration
file, however it is recommended that a strong and secure password be used
as this password is security critical to securing the wallet should the
wallet be enabled.

If reesistd is run with the "-server" flag (set by default), and no rpcpassword is set,
it will use a special cookie file for authentication. The cookie is generated with random
content when the daemon starts, and deleted when it exits. Read access to this file
controls who can access it through RPC.

By default the cookie is stored in the data directory, but it's location can be overridden
with the option '-rpccookiefile'.

This allows for running reesistd without having to do any manual configuration.

`conf`, `pid`, and `wallet` accept relative paths which are interpreted as
relative to the data directory. `wallet` *only* supports relative paths.

For an example configuration file that describes the configuration settings,
see `contrib/debian/examples/reesist.conf`.

Paths
---------------------------------

### Linux

All three configurations assume several paths that might need to be adjusted.

Binary:              `/usr/bin/reesistd`  
Configuration file:  `/etc/reesistcore/reesist.conf`  
Data directory:      `/var/lib/reesistd`  
PID file:            `/var/run/reesistd/reesistd.pid` (OpenRC and Upstart) or `/var/lib/reesistd/reesistd.pid` (systemd)  
Lock file:           `/var/lock/subsys/reesistd` (CentOS)  

The configuration file, PID directory (if applicable) and data directory
should all be owned by the reesistcore user and group.  It is advised for security
reasons to make the configuration file and data directory only readable by the
reesistcore user and group.  Access to reesist-cli and other reesistd rpc clients
can then be controlled by group membership.

### Mac OS X

Binary:              `/usr/local/bin/reesistd`  
Configuration file:  `~/Library/Application Support/ReesistCore/reesist.conf`  
Data directory:      `~/Library/Application Support/ReesistCore`
Lock file:           `~/Library/Application Support/ReesistCore/.lock`

Installing Service Configuration
-----------------------------------

### systemd

Installing this .service file consists of just copying it to
/usr/lib/systemd/system directory, followed by the command
`systemctl daemon-reload` in order to update running systemd configuration.

To test, run `systemctl start reesistd` and to enable for system startup run
`systemctl enable reesistd`

### OpenRC

Rename reesistd.openrc to reesistd and drop it in /etc/init.d.  Double
check ownership and permissions and make it executable.  Test it with
`/etc/init.d/reesistd start` and configure it to run on startup with
`rc-update add reesistd`

### Upstart (for Debian/Ubuntu based distributions)

Drop reesistd.conf in /etc/init.  Test by running `service reesistd start`
it will automatically start on reboot.

NOTE: This script is incompatible with CentOS 5 and Amazon Linux 2014 as they
use old versions of Upstart and do not supply the start-stop-daemon utility.

### CentOS

Copy reesistd.init to /etc/init.d/reesistd. Test by running `service reesistd start`.

Using this script, you can adjust the path and flags to the reesistd program by
setting the RAPTOREUMD and FLAGS environment variables in the file
/etc/sysconfig/reesistd. You can also use the DAEMONOPTS environment variable here.

### Mac OS X

Copy org.reesist.reesistd.plist into ~/Library/LaunchAgents. Load the launch agent by
running `launchctl load ~/Library/LaunchAgents/org.reesist.reesistd.plist`.

This Launch Agent will cause reesistd to start whenever the user logs in.

NOTE: This approach is intended for those wanting to run reesistd as the current user.
You will need to modify org.reesist.reesistd.plist if you intend to use it as a
Launch Daemon with a dedicated reesistcore user.

Auto-respawn
-----------------------------------

Auto respawning is currently only configured for Upstart and systemd.
Reasonable defaults have been chosen but YMMV.

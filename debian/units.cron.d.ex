#
# Regular cron jobs for the units package.
#
0 4	* * *	root	[ -x /usr/bin/units_maintenance ] && /usr/bin/units_maintenance

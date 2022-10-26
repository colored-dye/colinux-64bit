coLinux.exe: colinux_def.inc
	@wine "C:\\Program Files\\NSIS\\makensis" /V3 colinux.nsi

colinux_def.inc:
	@sh -c colinux_def.sh ../../../../VERSION $@

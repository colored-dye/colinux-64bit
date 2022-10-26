CURRENT_DIR = $(shell pwd)

all: daemon.res \
	colinux-debug.res \
	colinux-net.res \
	colinux-bridged-net.res \
	colinux-ndis-net.res \
	colinux-slirp-net.res \
	colinux-serial.res \
	colinux-fltk.res \
	colinux-wx.res

daemon.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) daemon.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux program\\\"'

colinux-debug.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-debug-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux Debug daemon program\\\"'

colinux-net.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-net-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux TAP Network program\\\"'

colinux-bridged-net.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-bridged-net-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux PCAP Bridged Network program\\\"'

colinux-ndis-net.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-ndis-net-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux Kernel Bridged Network program\\\"'

colinux-slirp-net.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-slirp-net-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux SLiRP Network program\\\"'

colinux-serial.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-serial-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux Serial daemon program\\\"'

colinux-fltk.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-console-fltk-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux FLTK console program\\\"'

colinux-nt.res: resources.def.inc
	$(CROSS)windres -O coff -I $(CURRENT_DIR) colinux.rc -o $@ '-DCO_FILENAME_STRING=\\\"colinux-console-nt-daemon.exe\\\"' '-DCO_TEXTNAME_STRING=\\\"coLinux NT console program\\\"'

resources.def.inc:
	resources_def.sh ../../../../../VERSION resources_def.inc

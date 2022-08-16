colinux-daemon.exe: ./user/daemon/daemon.o ./user/daemon/res/daemon.res
	$(CC) -o $@ $^

colinux-debug-daemon.exe: ./user/debug/build.o ../user/debug/build.o ./user/daemon/res/colinux-debug.res
	$(CC) -o $@ $^

#include <stdio.h>

#include <colinux/user/daemon.h>
#include <colinux/user/debug/main.h>

int main(int argc, char *argv[]) {
    co_rc_t rc;
    rc = co_debug_main(argc, argv);
    if (!CO_OK(rc)) {
        return -1;
    }
    return 0;
}

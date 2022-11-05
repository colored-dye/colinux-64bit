#include <stdio.h>
#include <windows.h>
#include <winternl.h>

int main() {
    static SYSTEM_BASIC_INFORMATION system_basic_info;
    static long long mem_size = 0;
    static SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    NtQuerySystemInformation(
        SystemBasicInformation,
        &system_basic_info,
        sizeof(SYSTEM_BASIC_INFORMATION),
        NULL
    );
    mem_size = system_basic_info.NumberOfPhysicalPages;
    mem_size *= sys_info.dwPageSize;
    float size = (float)mem_size;
    size /= (1024 * 1024);
    printf("%.3f\n", size);

    return 0;
}

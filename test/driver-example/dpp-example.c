#include <ntddk.h>

#define KDPRINT(...) \
	DbgPrint("coLinux %s: ", __FILE__);\
	DbgPrint(__VA_ARGS__);

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

NTSTATUS NTAPI
DriverEntry(
		IN PDRIVER_OBJECT DriverObject,
		IN PUNICODE_STRING RegistryPath
	)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	KDPRINT("%s\n", "Hello ring0!");

//	asm ("int3");

	DriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}

VOID NTAPI
DriverUnload(
		IN PDRIVER_OBJECT DriverObject
        )
{
    (void)DriverObject;

    KDPRINT("%s\n", "Bye ring0!");
}


#include <ntddk.h>

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

NTAPI VOID
DriverUnload(
		IN PDRIVER_OBJECT DriverObject
        )
{
    (void)DriverObject;

    DbgPrint("%s\n", "Bye ring0!");
}

NTAPI NTSTATUS
DriverEntry(
		IN PDRIVER_OBJECT DriverObject,
		IN PUNICODE_STRING RegistryPath
	)
{
	(void)RegistryPath;
	DriverObject->DriverUnload = DriverUnload;

	DbgPrint("%s\n", "Hello ring0!");
	return STATUS_SUCCESS;
}


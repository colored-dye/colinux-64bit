#include <ntddk.h>

#define NT_DEVICE_NAME	L"\\\\.\\Device\\coLinux"

#define KDPRINT(...) \
	DbgPrint("coLinux %s: ", __FILE__);\
	DbgPrint(__VA_ARGS__);

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;


NTAPI NTSTATUS
DriverEntry(
		IN PDRIVER_OBJECT DriverObject,
		IN PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS ntStatus;
	UNICODE_STRING ntUnicodeString;
	PDEVICE_OBJECT deviceObject = NULL;

	UNREFERENCED_PARAMETER(RegistryPath);

	KDPRINT("%s\n", "Hello ring0!");

	DriverObject->DriverUnload = DriverUnload;

	RtlInitUnicodeString(&ntUnicodeString, NT_DEVICE_NAME);

	ntStatus = IoCreateDevice(
			DriverObject,
			0,
			&ntUnicodeString,
			FILE_DEVICE_UNKNOWN,
			FILE_DEVICE_SECURE_OPEN,
			FALSE,
			&deviceObject
			);

	if ( !NT_SUCCESS(ntStatus) ) {
		KDPRINT("Couldn't create device object\n");
		return ntStatus;
	}

	KDPRINT("Created device object\n");

	return STATUS_SUCCESS;
}

NTAPI VOID
DriverUnload(
		IN PDRIVER_OBJECT DriverObject
        )
{
    (void)DriverObject;

    KDPRINT("%s\n", "Bye ring0!");
}


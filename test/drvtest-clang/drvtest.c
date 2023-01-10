/*
  by Frank Rysanek <rysanek@fccps.cz>

  Based on the source code of ioperm.c by Marcel Telka <marcel@telka.sk> 
    - thanks a million :-)

  Further knowledge taken from a series of 6 articles called "Driver Development"
  published by Toby Opferman at CodeProject.com
*/

//#define WINVER 0x0601
//#define _WIN32_WINNT 0x0601

#include <ntddk.h>
#include "my_ioctl.h"
#include "my_names.h"

#define IOCTL_KDPRINT(...) \
	DbgPrint("drvtest [%s:%04d]: ", __FILE__, __LINE__);\
	DbgPrint(__VA_ARGS__);

/* undocumented functions */
/* Used by Telka's ioperm - may come in handy [*** grin ***]
void STDCALL Ke386IoSetAccessProcess( PEPROCESS, int );
void STDCALL Ke386QueryIoAccessMap( int, unsigned char * );
void STDCALL Ke386SetIoAccessMap( int, unsigned char * );
*/

/* workaround for bug 666316 and bug 671964   (by Marcel Telka) */
/* Huh? How relevant is this? Commenting it out... (by Frank Rysanek) */
/*
#undef IoGetCurrentIrpStackLocation
#define	IoGetCurrentIrpStackLocation(irp) (*(struct _IO_STACK_LOCATION**)((char*)(irp) + 96))
*/


NTAPI VOID
my_unload( IN PDRIVER_OBJECT DriverObject )
{
	ANSI_STRING SymbolicLinkNameA;	
	UNICODE_STRING SymbolicLinkNameW;	

   IOCTL_KDPRINT("DriverUnload called\r\n");  // catch this using DBGVIEW from www.sysinternals.com

	/*RtlInitUnicodeString( &SymbolicLinkName, MY_DOSDEVICE_NAME );*/
	/* If I want to use normal string combining logic in my_names.h, I need to mess with ANSI vs. Unicode */
	RtlInitString( &SymbolicLinkNameA, MY_DOSDEVICE_NAME );
	RtlAnsiStringToUnicodeString( &SymbolicLinkNameW, &SymbolicLinkNameA, TRUE );  /* Init from an ANSI string, and do allocate target buffer */

	IoDeleteSymbolicLink( &SymbolicLinkNameW );
	IoDeleteDevice( DriverObject->DeviceObject );
}


NTAPI
NTSTATUS 
my_dispatch_create( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return STATUS_SUCCESS;
}


/*
 Promising functions:
 PVOID MmMapIoSpace(PHYSICAL_ADDRESS phy, SIZE_T num_bytes, MmNonCached);
 VOID MmUnmapIoSpace(PVOID virt, SIZE_T num_bytes);

 Reserved for system use:
 MmGetPhysicalMemoryRanges()
 MmGetVirtualForPhysical()
 MmIsIoSpaceActive()
);
*/
NTAPI
NTSTATUS
my_dispatch_device_control( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	PIO_STACK_LOCATION io_stack;

	/* default return values */
	Irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
	Irp->IoStatus.Information = 0; /* !!! override as needed !!! */
   /* "Information" = dwBytesReturned = override this in switch cases 
                            returning some data to user space */

	/* get current I/O stack location */
	io_stack = IoGetCurrentIrpStackLocation( Irp );
	if (! (io_stack))  /* do we have a valid io_stack? */
   {
      Irp->IoStatus.Status = STATUS_INTERNAL_ERROR;
      goto err_out;
   }

   /* if this fires, we've hooked up the ioctl() handler to the wrong callback slot */
	if (io_stack->MajorFunction != IRP_MJ_DEVICE_CONTROL)  
   {
      Irp->IoStatus.Status = STATUS_INTERNAL_ERROR;
      goto err_out;
   }


   switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
   {
      case IOCTL_EXAMPLE :
      {
         /* get input buffer */
         struct example_ioctl_data* my_data = (struct example_ioctl_data *) Irp->AssociatedIrp.SystemBuffer;

         /* test input buffer size and existence */
         if (
               (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof (struct example_ioctl_data))
               || (!my_data) 
            )
         {
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            goto err_out;
         } 

         /* else okay, ready to go */
         IOCTL_KDPRINT("ioctl(IOCTL_EXAMPLE) called, arg = %d\r\n", my_data->code );  // catch this using DBGVIEW from www.sysinternals.com
         Irp->IoStatus.Status = STATUS_SUCCESS;
         Irp->IoStatus.Information = 0; /* !!! No data to copy_to_user()... */
      }
      break;

      default: /* STATUS_NOT_IMPLEMENTED takes action */
      break;
	}

err_out:

	/* complete IRP request */
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return Irp->IoStatus.Status;
}



NTAPI
NTSTATUS
DriverEntry( IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath )
{
	NTSTATUS status;
   /* a particular Device Instance, theoretically one of many,
      managed by this driver : */
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceNameW;
	UNICODE_STRING SymbolicLinkNameW;
	ANSI_STRING DeviceNameA;
	ANSI_STRING SymbolicLinkNameA;

   IOCTL_KDPRINT("DriverEntry called\r\n");  // catch this using DBGVIEW from www.sysinternals.com

	/* support for service stopping */
	DriverObject->DriverUnload = my_unload;
	/* create support */
	DriverObject->MajorFunction[IRP_MJ_CREATE] = my_dispatch_create;
	/* IOCTL support */
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = my_dispatch_device_control;

	/* initialize counted unicode strings */
	/* If I want to use normal string combining logic in my_names.h, I need to mess with ANSI vs. Unicode */
	/* RtlInitUnicodeString( &DeviceName, MY_DEVICE_NAME ); */
	RtlInitString( &DeviceNameA, MY_DEVICE_NAME );
	RtlAnsiStringToUnicodeString( &DeviceNameW, &DeviceNameA, TRUE );              /* Init from an ANSI string, and do allocate target buffer */
	/* RtlInitUnicodeString( &SymbolicLinkName, MY_DOSDEVICE_NAME ); */
	RtlInitString( &SymbolicLinkNameA, MY_DOSDEVICE_NAME );
	RtlAnsiStringToUnicodeString( &SymbolicLinkNameW, &SymbolicLinkNameA, TRUE );  /* Init from an ANSI string, and do allocate target buffer */

	/* create device object */
	status = IoCreateDevice( DriverObject, 
                            0,
                            &DeviceNameW,
                            FILE_DEVICE_UNKNOWN,
                            0,
                            FALSE,
                            &DeviceObject 
                           );
	if (!NT_SUCCESS( status ))
		return status;

   /* Should you want to use the "write()" handler,
    * you'll want to select one of the possible write styles 
    * (direct/buffered).
    
#define IO_TYPE DO_DIRECT_IO
#define IO_TYPE DO_BUFFERED_IO
#define IO_TYPE 0
   
   DeviceObject->Flags |= IO_TYPE;
   */

   /* Clear a flag, set by IoCreateDevice.
    * This is not mandatory within DriverEntry, 
    * but it *is* mandatory if used anywhere else. */
   DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	/* create user-visible name for the device */
	status = IoCreateSymbolicLink( &SymbolicLinkNameW, &DeviceNameW );
	if (!NT_SUCCESS( status ))
		return status;

	return STATUS_SUCCESS;
}

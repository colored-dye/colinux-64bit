#ifndef _MY_IOCTL_H_
#define _MY_IOCTL_H_ 1

/* 
   METHOD_BUFFERED handles the copy_to_user/copy_from_user automagically for us, at the cost of some overhead
   METHOD_OUT_DIRECT  = no automagical copy, MDL set up for  "Kernel_driver -> User_mode_app"  direction
   METHOD_IN_DIRECT   = no automagical copy, MDL set up for  "User_mode_app -> Kernel_driver"  direction
   METHOD_NEITHER     = no automagical copy, no automagical MDL setup. Driver (ioctl code) must be guaranteed 
                        to run in the context of the calling user-space thread and ask for the user->system 
                        mapping itself, and its access to the buffer should be wrapped in a try/catch to handle
                        potential exceptions...

   That's right, DIRECT IN/OUT are from the driver's point of view, relative to the user-space app!!!
   Both the MSDN and the Coreproject.com article say so, the CodeProject article with added emphasis.
*/

/* CTL Codes above 0x800 are free for your custom use 
   Note: using METHOD_BUFFERED = we're lazy and we don't care about a bit of overhead */

#define IOCTL_EXAMPLE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct example_ioctl_data {
	int code;
};


#endif /* _MY_IOCTL_H_ */

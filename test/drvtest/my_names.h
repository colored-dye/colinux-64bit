#ifndef _MY_NAMES_H_
#define _MY_NAMES_H_ 1

/* Remember: two "string literals" "will get concatenated into one" 
   if there's no punctuation between them:
   "hello" "world" => "helloworld" 
*/
#ifndef MY_DRIVER_NAME
#define MY_DRIVER_NAME "drvtest"                     /* not used itself outside of this header file */
#endif
/* for the driver */
#define MY_DEVICE_NAME     "\\Device\\" MY_DRIVER_NAME
#define MY_DOSDEVICE_NAME  "\\DosDevices\\"  MY_DRIVER_NAME  /* AKA symlink name */
/* for the loader and app */
#define MY_SERVICE_NAME_LONG  "Driver Test"
#define MY_SERVICE_NAME_SHORT MY_DRIVER_NAME
#define MY_DRIVER_FILENAME    MY_DRIVER_NAME ".sys"

#endif /* _MY_NAMES_H_ */

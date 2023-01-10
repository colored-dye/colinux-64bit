/* 
   File created by Frank Rysanek <rysanek@fccps.cz>
   Source code taken almost verbatim from "Driver Development, Part 1&2"
   published by Toby Opferman at CodeProject.com 
*/

#include <stdio.h>
#include <windows.h>
/*#include <string.h>*/
/* #include <unistd.h> */     /* getcwd() */
#include <winioctl.h>        /* so that my_ioctl.h works */
#include "my_ioctl.h"
#include "my_names.h"
#include "err_helpers.h"

int main(void)
{
 HANDLE hFile;
 DWORD bytes_returned;
 int retval = 0;
 struct example_ioctl_data my_data;

   hFile = CreateFile("\\\\.\\" MY_SERVICE_NAME_SHORT, 
                      GENERIC_READ | GENERIC_WRITE, 
                      0, 
                      NULL, 
                      OPEN_EXISTING,
                      0, 
                      NULL);

   if (! hFile)
   {
      printf("Uh oh:\n");
      process_error();
      retval = -1;
      goto err_out1;
   }

   my_data.code = 42;
   /* We'll just use the same buffer for input and output. 
      Our CTL code specifies buffered operation (IN+OUT). */
   if (
         DeviceIoControl(hFile,
                   IOCTL_EXAMPLE, 
                   &my_data, 
                   sizeof(struct example_ioctl_data), 
                   &my_data, 
                   sizeof(struct example_ioctl_data), 
                   &bytes_returned, 
                   NULL)
         == 0)
   {
      printf("Uh oh:\n");
      process_error();
      retval = -2;
   }
   /* else okay, but nothing more to do anyway */
    
   CloseHandle(hFile);

err_out1:

   return retval;
}


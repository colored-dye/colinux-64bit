/* 
   File created by Frank Rysanek <rysanek@fccps.cz>
   Source code taken almost verbatim from "Driver Development, Part 1"
   published by Toby Opferman at CodeProject.com 
*/

#include <stdio.h>
#include <windows.h>
/*#include <string.h>*/
#include <unistd.h>     /* getcwd() */
#include "my_names.h"
#include "err_helpers.h"

#define MAX_CWD_LEN 1024
static char cwd[MAX_CWD_LEN+3];  /* the XXXXX.sys filename will get appended to this as well */

int main(void)
{
 HANDLE hSCManager;
 HANDLE hService;
 SERVICE_STATUS ss;
 int retval = 0;

   /* First of all, maybe concatenate the current working directory
      with the desired driver file name - before we start messing with
      the service manager etc. */
   if (getcwd(cwd, MAX_CWD_LEN) == NULL)  /* error */
   {
      printf("Failed to learn the current working directory!\n");
      retval = -8;
      goto err_out1;
   } /* else got CWD just fine */

   if (strlen(cwd) + strlen(MY_DRIVER_FILENAME) + 1 > MAX_CWD_LEN)
   {
      printf("Current working dir + driver filename > longer than %d ?!?\n", MAX_CWD_LEN);
      retval = -9;
      goto err_out1;
   } /* else our buffer is long enough :-) */

   strcat(cwd, "\\");
   strcat(cwd, MY_DRIVER_FILENAME); 
   printf("Driver path+name: %s\n", cwd);


   printf("Going to open the service manager... ");

   hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
   if (! hSCManager)
   {
      printf("Uh oh:\n");
      process_error();
      retval = -1;
      goto err_out1;
   }

   printf("okay.\n");
   printf("Going to create the service... ");

   hService = CreateService(hSCManager, MY_SERVICE_NAME_SHORT, 
                            MY_SERVICE_NAME_LONG, 
                            SERVICE_START | DELETE | SERVICE_STOP, 
                            SERVICE_KERNEL_DRIVER,
                            SERVICE_DEMAND_START, 
                            SERVICE_ERROR_IGNORE, 
                            cwd, 
                            NULL, NULL, NULL, NULL, NULL);

   if(!hService)
   {
      process_error();
      printf("\n already exists? Trying to open it... ");
      hService = OpenService(hSCManager, MY_SERVICE_NAME_SHORT, 
                             SERVICE_START | DELETE | SERVICE_STOP);
   }

   if(!hService)
   {
      printf("FAILED!\n");
      process_error();
      retval = -2;
      goto err_out2;
   }

   printf("okay.\n");
   printf("Going to start the service... ");

   if (StartService(hService, 0, NULL) == 0)  /* error */
   {
      printf("Uh oh:\n");
      process_error();
      retval = -3;
      goto err_out3;
   }

   printf("okay.\n");
   printf("\n >>> Press Enter to unload the driver! <<<\n");
   getchar();

   printf("Going to stop the service... ");
   if (ControlService(hService, SERVICE_CONTROL_STOP, &ss) == 0) /* error */
   {
      printf("Uh oh:\n");
      process_error();
      retval = -4;
   }
   else printf("okay.\n");

err_out3:

   printf("Going to delete the service... ");
   if (DeleteService(hService) == 0) /* error */
   {
      printf("Uh oh:\n");
      process_error();
      retval = -5;
   }
   else printf("okay.\n");

   printf("Going to close the service handle... ");
   if (CloseServiceHandle(hService) == 0) /* error */
   {
      printf("Uh oh:\n");
      process_error();
      retval = -6;
   }
   else printf("okay.\n");

err_out2:

   printf("Going to close the service manager... ");
   if (CloseServiceHandle(hSCManager) == 0) /* error */
   {
      printf("Uh oh:\n");
      process_error();
      retval = -7;
   }
   else printf("okay.\n");

err_out1:

   printf("Finished! :-b\n");

   return(retval);
}



#include <stdio.h>
#include <windows.h>

/* geterrstr() taken verbatim from some code snippet at www.mingw.org by Dan Osborne. */
/* Apparently, it's a way to get a classic null-terminated string containing "last error". */
static char errbuffer[256];

static const char *geterrstr(DWORD errcode)
{
 size_t skip = 0;
 DWORD chars;

   chars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                         NULL, errcode, 0, errbuffer, sizeof(errbuffer)-1, 0);
   errbuffer[sizeof(errbuffer)-1] = 0;

   if (chars) 
   {
      while (errbuffer[chars-1] == '\r' || errbuffer[chars-1] == '\n') 
      {
         errbuffer[--chars] = 0;
      }
   }

   if (chars && errbuffer[chars-1] == '.') errbuffer[--chars] = 0;

   if (chars >= 2 && errbuffer[0] == '%' 
       && errbuffer[1] >= '0' && errbuffer[1] <= '9')
   {
      skip = 2;

      while (chars > skip && errbuffer[skip] == ' ') ++skip;

      if (chars >= skip+2 && errbuffer[skip] == 'i' && errbuffer[skip+1] == 's')
      {
         skip += 2;
         while (chars > skip && errbuffer[skip] == ' ') ++skip;
      }
   }

   if (chars > skip && errbuffer[skip] >= 'A' && errbuffer[skip] <= 'Z') 
   {
      errbuffer[skip] += 'a' - 'A';
   }

   return errbuffer+skip;
}



void process_error(void)
{
   DWORD err = GetLastError();
   printf("Error: %lu = \"%s\"\n", (unsigned long)err, geterrstr(err));
}

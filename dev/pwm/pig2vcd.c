/*
Disclaimer of Warranty.

  THERE IS NO WARRANTY FOR THE SOFTWARE, TO THE EXTENT PERMITTED BY
APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS" WITHOUT WARRANTY
OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE
IS WITH YOU.  SHOULD THE SOFTWARE PROVE DEFECTIVE, YOU ASSUME THE COST OF
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

Limitation of Liability.

  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS
THE SOFTWARE AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
USE OR INABILITY TO USE THE SOFTWARE (INCLUDING BUT NOT LIMITED TO LOSS OF
DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD
PARTIES OR A FAILURE OF THE SOFTWARE TO OPERATE WITH ANY OTHER SOFTWARES),
EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.
*/

/*
This version is for pigpio version 3-4
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include "pigpio.h"

/*
This software converts pigpio notification reports
into a VCD format understood by GTKWave.
*/

#define RS (sizeof(gpioReport_t))

static char * timeStamp()
{
   static char buf[32];

   struct timeval now;
   struct tm tmp;

   gettimeofday(&now, NULL);

   localtime_r(&now.tv_sec, &tmp);
   strftime(buf, sizeof(buf), "%F %T", &tmp);

   return buf;
}

int symbol(int bit)
{
   if (bit < 26) return ('A' + bit);
   else          return ('a' + bit - 26);
}

int main(int argc, char * argv[])
{
   int b, r, v;
   uint32_t t0;
   uint32_t lastLevel, changed;

   gpioReport_t report;

   r=read(STDIN_FILENO, &report, RS);

   if (r != RS) exit(-1);

   printf("$date %s $end\n", timeStamp());
   printf("$version pig2vcd V1 $end\n");
   printf("$timescale 1 us $end\n");
   printf("$scope module top $end\n");

   for (b=0; b<32; b++)
      printf("$var wire 1 %c %d $end\n", symbol(b), b);
        
   printf("$upscope $end\n");
   printf("$enddefinitions $end\n");
         
   t0 = report.tick;
   lastLevel =0;

   while ((r=read(STDIN_FILENO, &report, RS)) == RS)
   {
      if (report.level != lastLevel)
      {
         printf("#%u\n", report.tick - t0);

         changed = report.level ^ lastLevel;

         lastLevel = report.level;

         for (b=0; b<32; b++)
         {
            if (changed & (1<<b))
            {
               if (report.level & (1<<b)) v='1'; else v='0';

               printf("%c%c\n", v, symbol(b));
            }
         }
      }
   }
   return 0;
}


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
This version is for pigpio version 4
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "pigpio.h"
#include "command.h"

cmdInfo_t cmdInfo[]=
{
   {PI_CMD_BR1,   "BR1",   1, 3},
   {PI_CMD_BR2,   "BR2",   1, 3},
   {PI_CMD_BC1,   "BC1",   7, 1},
   {PI_CMD_BC2,   "BC2",   7, 1},
   {PI_CMD_BS1,   "BS1",   7, 1},
   {PI_CMD_BS2,   "BS2",   7, 1},
   {PI_CMD_HWVER, "HWVER", 1, 4},
   {PI_CMD_MODES, "MODES", 8, 0},
   {PI_CMD_MODES, "M",     8, 0},
   {PI_CMD_MODEG, "MODEG", 2, 2},
   {PI_CMD_MODEG, "MG"   , 2, 2},
   {PI_CMD_NO,    "NO",    1, 2},
   {PI_CMD_NB,    "NB",    4, 0},
   {PI_CMD_NP,    "NP",    2, 0},
   {PI_CMD_NC,    "NC",    2, 0},
   {PI_CMD_PWM,   "PWM",   3, 0},
   {PI_CMD_PWM,   "P",     3, 0},
   {PI_CMD_PFS,   "PFS",   3, 2},
   {PI_CMD_PFG,   "PFG",   2, 2},
   {PI_CMD_PRS,   "PRS",   3, 2},
   {PI_CMD_PRG,   "PRG",   2, 2},
   {PI_CMD_PRRG,  "PRRG",  2, 2},
   {PI_CMD_PUD,   "PUD",   9, 0},
   {PI_CMD_READ,  "READ",  2, 2},
   {PI_CMD_READ,  "R",     2, 2},
   {PI_CMD_SERVO, "SERVO", 3, 0},
   {PI_CMD_SERVO, "S",     3, 0},
   {PI_CMD_WRITE, "WRITE", 3, 0},
   {PI_CMD_WRITE, "W",     3, 0},
   {PI_CMD_WDOG,  "WDOG",  3, 0},
   {PI_CMD_TICK,  "TICK",  1, 4},
   {PI_CMD_TICK,  "T",     1, 4},
   {PI_CMD_HELP,  "HELP",  6, 5},
   {PI_CMD_HELP,  "H",     6, 5},
};

char * cmdUsage = "\
BR1          read gpios bank 1\n\
BR2          read gpios bank 2\n\
BC1 x        clear gpios in bank 1\n\
BC2 x        clear gpios in bank 2\n\
BS1 x        set gpios in bank 1\n\
BS2 x        set gpios in bank 2\n\
HWVER        return hardware version\n\
MODES/M g m  set gpio mode\n\
MODEG/MG g   get gpio mode\n\
NO           request notification handle\n\
NB h x       start notification\n\
NP h         pause notification\n\
NC h         close notification\n\
PWM/P u d    set PWM value for gpio\n\
PFS u d      set PWM frequency for gpio\n\
PFG u        get PWM frequency for gpio\n\
PRS u d      set PWM range for gpio\n\
PRG u        get PWM range for gpio\n\
PRRG u       get PWM real range for gpio\n\
PUD g p      set gpio pull up/down\n\
READ/R g     read gpio\n\
SERVO/S u d  set servo value for gpio\n\
WRITE/W g d  write value to gpio\n\
WDOG u d     set watchdog on gpio\n\
TICK/T       return current tick\n\
HELP/H       displays command help\n\
\n\
d = decimal value\n\
g = gpio (0-53)\n\
h = handle (0-31)\n\
m = mode (RW540123)\n\
p = pud (ODU)\n\
u = user gpio (0-31)\n\
x = hex value\n\
";

typedef struct
{
   int error;
   char * str;
} errInfo_t;

static errInfo_t errInfo[]=
{
   {PI_INIT_FAILED      , "pigpio initialisation failed"},
   {PI_BAD_USER_GPIO    , "gpio not 0-31"},
   {PI_BAD_GPIO         , "gpio not 0-53"},
   {PI_BAD_MODE         , "mode not 0-7"},
   {PI_BAD_LEVEL        , "level not 0-1"},
   {PI_BAD_PUD          , "pud not 0-2"},
   {PI_BAD_PULSEWIDTH   , "pulsewidth not 0 or 500-2500"},
   {PI_BAD_DUTYCYCLE    , "dutycycle not 0-255"},
   {PI_BAD_TIMER        , "timer not 0-9"},
   {PI_BAD_MS           , "ms not 10-60000"},
   {PI_BAD_TIMETYPE     , "timetype not 0-1"},
   {PI_BAD_SECONDS      , "seconds < 0"},
   {PI_BAD_MICROS       , "micros not 0-999999"},
   {PI_TIMER_FAILED     , "gpioSetTimerFunc failed"},
   {PI_BAD_WDOG_TIMEOUT , "timeout not 0-60000"},
   {PI_NO_ALERT_FUNC    , "DEPRECATED"},
   {PI_BAD_CLK_PERIPH   , "clock peripheral not 0-1"},
   {PI_BAD_CLK_SOURCE   , "clock source not 0-1"},
   {PI_BAD_CLK_MICROS   , "clock micros not 1, 2, 4, 5, 8, or 10"},
   {PI_BAD_BUF_MILLIS   , "buf millis not 100-10000"},
   {PI_BAD_DUTY_RANGE   , "dutycycle range not 25-40000"},
   {PI_BAD_SIGNUM       , "signum not 0-63"},
   {PI_BAD_PATHNAME     , "can't open pathname"},
   {PI_NO_HANDLE        , "no handle available"},
   {PI_BAD_HANDLE       , "unknown notify handle"},
   {PI_BAD_IF_FLAGS     , "ifFlags > 3"},
   {PI_BAD_CHANNEL      , "DMA channel not 0-14"},
   {PI_BAD_SOCKET_PORT  , "socket port not 1024-30000"},
   {PI_BAD_FIFO_COMMAND , "unknown fifo command"},
   {PI_BAD_SECO_CHANNEL , "DMA secondary channel not 0-6"},
   {PI_NOT_INITIALISED  , "function called before gpioInitialise"},
   {PI_INITIALISED      , "function called after gpioInitialise"},
   {PI_BAD_WAVE_MODE    , "waveform mode not 0-1"},
   {PI_BAD_CFG_INTERNAL , "bad parameter in gpioCfgInternals call"},
   {PI_BAD_WAVE_BAUD    , "baud rate not 100-250000"},
   {PI_TOO_MANY_PULSES  , "waveform has too many pulses"},
   {PI_TOO_MANY_CHARS   , "waveform has too many chars"},
   {PI_NOT_SERIAL_GPIO  , "no serial read in progress on gpio"},

};

static char * fmtMdeStr="RW540123";
static char * fmtPudStr="ODU";

static int cmdMatch(char * str)
{
   int i;

   for (i=0; i<(sizeof(cmdInfo)/sizeof(cmdInfo_t)); i++)
   {
      if (strcasecmp(str, cmdInfo[i].name) == 0) return i;
   }
   return -1;
}

int cmdParse(char * buf, cmdCmd_t * cmd)
{
   char str[8];
   int f, valid, idx, val;
   char * ptr;
   char c, t;

   sscanf(buf, " %7s", str);

   cmd->cmd = -1;

   idx = cmdMatch(str);

   if (idx < 0) return idx;

   valid = 0;

   cmd->cmd = cmdInfo[idx].cmd;
   cmd->p1  = 0;
   cmd->p2  = 0;

   switch (cmdInfo[idx].vt)
   {
      case 1: /* BR1 BR2 HWVER NO TICK */
         f = sscanf(buf, " %7s %c", str, &t);
         if (f == 1) valid = 1;
         break;

      case 2: /* MODEG READ NC NP PFG PRG PRRG */
         f = sscanf(buf, " %7s %d %c", str, &cmd->p1, &t);
         if (f == 2) valid = 1;
         break;
   
      case 3: /* WRITE PWM PRS PFS SERVO WDOG */
         f = sscanf(buf, " %7s %d %d %c", str, &cmd->p1, &cmd->p2, &t);
         if (f == 3) valid = 1;
         break;
   
      case 4: /* NB */
         f = sscanf(buf, " %7s %d %x %c", str, &cmd->p1, &cmd->p2, &t);
         if (f == 3) valid = 1;
         break;
   
      case 6: /* HELP */
         valid = 1;
         break;
   
      case 7: /* BC1 BC2 BS1 BS2 */
         f = sscanf(buf, " %7s %x %c", str, &cmd->p1, &t);
         if (f == 2) valid = 1;
         break;
   
      case 8: /* MODES */
         f = sscanf(buf, " %7s %d %c %c", str, &cmd->p1, &c, &t);
         if (f == 3)
         {
            val = toupper(c);
            ptr = strchr(fmtMdeStr, val);
            if (ptr != NULL)
            {
               val = ptr - fmtMdeStr;
               cmd->p2 = val;
               valid = 1;
            }
         }
         break;

      case 9: /* PUD */
         f = sscanf(buf, " %7s %d %c %c", str, &cmd->p1, &c, &t);
         if (f == 3)
         {
            val = toupper(c);
            ptr = strchr(fmtPudStr, val);
            if (ptr != NULL)
            {
               val = ptr - fmtPudStr;
               cmd->p2 = val;
               valid = 1;
            }
         }
         break;
   }

   if (valid) return idx;
   else       return -1;
}

void cmdFatal(char *fmt, ...)
{
   char buf[128];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   fprintf(stderr, "%s\n", buf);

   fflush(stderr);

   exit(EXIT_FAILURE);
}

char * cmdErrStr(int error)
{
   int i;

   for (i=0; i<(sizeof(errInfo)/sizeof(errInfo_t)); i++)
   {
      if (errInfo[i].error == error) return errInfo[i].str;
   }
   return "unknown error";
}


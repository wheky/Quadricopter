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
PARTIES OR A FAILURE OF THE SOFTWARE TO OPERATE WITH ANY OTHER SOFTWARE),
EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.
*/

/*
This version is for pigpio version 4
*/

#ifndef PIGPIO_H
#define PIGPIO_H

/************************************************************************** /
/                                                                           /
/ pigpio is a C library for the Raspberry Pi which allows                   /
/ control of the gpios.                                                     /
/                                                                           /
/ Its main features are:                                                    /
/                                                                           /
/ 1) provision of PWM on any number of gpios 0-31 simultaneously.           /
/ 2) provision of servo pulses on any number of gpios 0-31 simultaneously.  /
/ 3) callbacks when any of gpios 0-31 change state.                         /
/ 4) callbacks at timed intervals.                                          /
/ 5) reading/writing all of the gpios in a bank (0-31, 32-53) as a          /
/    single operation.                                                      /
/ 6) individually setting gpio modes, reading and writing.                  /
/                                                                           /
/ NOTE:                                                                     /
/                                                                           /
/ ALL gpios are identified by their Broadcom number.                        /
/                                                                           /
*************************************************************************** /
/                                                                           /
/ The PWM and servo pulses are timed using the DMA and PWM peripherals.     /
/                                                                           /
/ This use was inspired by Richard Hirst's servoblaster kernel module.      /
/ See https://github.com/richardghirst/PiBits                               /
/ Tag rgh on the Raspberry Pi forums http://www.raspberrypi.org/phpBB3/     /
/                                                                           /
*************************************************************************** /
/                                                                           /
/ Usage:                                                                    /
/                                                                           /
/ copy libpigpio.a-hard or libpigpio.a-soft to libpigpio.a                  /
/ copy libpigpio.a to /usr/local/lib                                        /
/ copy pigpio.h    to /usr/local/include                                    /
/                                                                           /
/ #include <pigpio.h> in your source files                                  /
/                                                                           /
/ Assuming your source is in example.c use the following command to build   /
/                                                                           /
/ gcc -o example example.c -lpigpio -lpthread -lrt                          /
/                                                                           /
/ For examples see checklib.c, demolib.c, pigpio.c, pigpiod.c, pig2vcd.c,   /
/ and pigs.c                                                                /
/                                                                           /
****************************************************************************/

#include <stdint.h>

#define PIGPIO_VERSION 4

/*-------------------------------------------------------------------------*/

/* 

Function                   Usage
--------                   -----

gpioInitialise             Initialise library.
gpioTerminate              Terminate library.

gpioSetMode                Set a gpio mode.
gpioGetMode                Get a gpio mode.

gpioSetPullUpDown          Set/clear gpio pull up/down resistor.

gpioRead                   Read a gpio.
gpioWrite                  Write a gpio.

gpioPWM                    Start/stop PWM pulses on a gpio.

gpioSetPWMrange            Configure PWM range for a gpio.
gpioGetPWMrange            Get configured PWM range for a gpio.
gpioGetPWMrealRange        Get underlying PWM range for a gpio.

gpioSetPWMfrequency        Configure PWM frequency for a gpio.
gpioGetPWMfrequency        Get configured PWM frequency for a gpio.

gpioServo                  Start/stop servo pulses on a gpio.

gpioSetAlertFunc           Request a gpio change callback.
gpioSetAlertFuncEx         Request a gpio change callback, extended.

gpioNotifyOpen             Open a gpio(s) changed notification.
gpioNotifyBegin            Begin a gpio(s) changed notification.
gpioNotifyPause            Pause a gpio(s) changed notification.
gpioNotifyClose            Close a gpio(s) changed notification.

gpioSetWatchdog            Set a watchdog on a gpio.

gpioSetGetSamplesFunc      Requests a gpio samples callback.
gpioSetGetSamplesFuncEx    Requests a gpio samples callback, extended.

gpioSetTimerFunc           Request a regular timed callback.
gpioSetTimerFuncEx         Request a regular timed callback, extended.

gpioSetSignalFunc          Request a signal callback.
gpioSetSignalFuncEx        Request a signal callback, extended.

gpioRead_Bits_0_31         Read gpios in bank 1.
gpioRead_Bits_32_53        Read gpios in bank 2.

gpioWrite_Bits_0_31_Clear  Clear gpios in bank 1.
gpioWrite_Bits_32_53_Clear Clear gpios in bank 2.

gpioWrite_Bits_0_31_Set    Set gpios in bank 1.
gpioWrite_Bits_32_53_Set   Set gpios in bank 2.

gpioTime                   Get current time.

gpioSleep                  Sleep for specified time.
gpioDelay                  Delay for microseconds.

gpioTick                   Get current tick (microseconds).

gpioHardwareRevision       Get hardware version.

gpioCfgBufferSize          Configure the gpio sample buffer size.
gpioCfgClock               Configure the gpio sample rate.
gpioCfgDMAchannel          Configure the DMA channel.
gpioCfgInterfaces          Configure user interfaces.
gpioCfgInternals           Configure internals.
gpioCfgSocketPort          Configure socket port.

*/

/*-------------------------------------------------------------------------*/


#define PI_INPFIFO "/dev/pigpio"
#define PI_OUTFIFO "/dev/pigout"
#define PI_ERRFIFO "/dev/pigerr"

#define PI_ENVPORT "PIGPIO_PORT"
#define PI_ENVADDR "PIGPIO_ADDR"

#define PI_LOCKFILE "/var/run/pigpio.pid"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   uint32_t cmd;
   uint32_t p1;
   uint32_t p2;
   uint32_t res;
} cmdCmd_t;

typedef struct
{
   uint32_t tick;
   uint32_t level;
} gpioSample_t;

typedef struct
{
   uint16_t seqno;
   uint16_t flags;
   uint32_t tick;
   uint32_t level;
} gpioReport_t;

typedef struct
{
   uint32_t gpioOn;
   uint32_t gpioOff;
   uint32_t usDelay;
} gpioPulse_t;

typedef struct
{
   char   * buf;
   uint32_t bufSize;
   int      readPos;
   int      writePos;
} gpioRx_t;

typedef void (*gpioAlertFunc_t)    (int      gpio,
                                    int      level,
                                    uint32_t tick);

typedef void (*gpioAlertFuncEx_t)  (int      gpio,
                                    int      level,
                                    uint32_t tick,
                                    void *   userdata);

typedef void (*gpioTimerFunc_t)    (void);

typedef void (*gpioTimerFuncEx_t)  (void * userdata);

typedef void (*gpioSignalFunc_t)   (int    signum);

typedef void (*gpioSignalFuncEx_t) (int    signum,
                                    void * userdata);

typedef void (*gpioGetSamplesFunc_t)   (const gpioSample_t * samples,
                                        int                  numSamples);

typedef void (*gpioGetSamplesFuncEx_t) (const gpioSample_t * samples,
                                        int                  numSamples,
                                        void *               userdata);

/*
   All the functions which return an int return < 0 on error.
*/

 

/*-------------------------------------------------------------------------*/
int gpioInitialise(void);
/*-------------------------------------------------------------------------*/
/* Initialises the library.

   Call before using the other library functions.

   The only exception is the gpioCfg* functions, see later.
*/



/*-------------------------------------------------------------------------*/
void gpioTerminate(void);
/*-------------------------------------------------------------------------*/
/* Terminates the library.

   Call before program exit.

   NOTES:

   This function resets the DMA and PWM peripherals, releases memory, and
   terminates any running threads.
*/



/*-------------------------------------------------------------------------*/
int gpioSetMode(unsigned gpio,
                unsigned mode);
/*-------------------------------------------------------------------------*/
/* Sets the gpio mode, typically input or output.

   Arduino style: pinMode.
*/

/* gpio: 0-53 */

#define PI_MIN_GPIO       0
#define PI_MAX_GPIO      53

/* mode: 0-7 */

#define PI_INPUT  0
#define PI_OUTPUT 1
#define PI_ALT0   4
#define PI_ALT1   5
#define PI_ALT2   6
#define PI_ALT3   7
#define PI_ALT4   3
#define PI_ALT5   2



/*-------------------------------------------------------------------------*/
int gpioGetMode(unsigned gpio);
/*-------------------------------------------------------------------------*/
/* Gets the gpio mode.
*/



/*-------------------------------------------------------------------------*/
int gpioSetPullUpDown(unsigned gpio,
                      unsigned pud);
/*-------------------------------------------------------------------------*/
/* Sets or clears resistor pull ups or downs on the gpio.
*/

/* pud: 0-2 */

#define	PI_PUD_OFF  0
#define	PI_PUD_DOWN 1
#define	PI_PUD_UP   2



/*-------------------------------------------------------------------------*/
int gpioRead (unsigned gpio);
/*-------------------------------------------------------------------------*/
/* Reads the gpio level, on or off.

   Arduino style: digitalRead.
*/

/* level: 0-1 */

#define	PI_OFF   0
#define	PI_ON    1

#define	PI_CLEAR 0
#define	PI_SET   1

#define	PI_LOW   0
#define	PI_HIGH  1

/* level: only reported for gpio timeout, see gpioSetWatchdogTimeout */

#define PI_TIMEOUT 2



/*-------------------------------------------------------------------------*/
int gpioWrite(unsigned gpio,
              unsigned level);
/*-------------------------------------------------------------------------*/
/* Sets the gpio level, on or off.

   Arduino style: digitalWrite
*/



/*-------------------------------------------------------------------------*/
int gpioPWM(unsigned user_gpio,
            unsigned dutycycle);
/*-------------------------------------------------------------------------*/
/* Starts PWM on the gpio, dutycycle between 0 (off) and range (fully on).
   Range defaults to 255.

   Arduino style: analogWrite

   NOTES:

   This and the servo functionality use the DMA and PWM or PCM peripherals
   to control and schedule the pulse lengths and duty cycles.

   The gpioSetPWMrange funtion can change the default range of 255.
*/

/* user_gpio: 0-31 */

#define PI_MAX_USER_GPIO 31

/* dutycycle: 0-range */

#define PI_DEFAULT_DUTYCYCLE_RANGE   255



/*-------------------------------------------------------------------------*/
int gpioSetPWMrange(unsigned user_gpio,
                    unsigned range);
/*-------------------------------------------------------------------------*/
/* Selects the dutycycle range to be used for the gpio.  Subsequent calls
   to gpioPWM will use a dutycycle between 0 (off) and range (fully on).

   If PWM is currently active on the gpio its dutycycle will be scaled
   to reflect the new range.

   Upon success the function returns the real range for the given
   gpio's frequency.

   NOTES:

   The real range, the number of steps between fully off and fully
   on for each frequency, is given in the following table.

     25,   50,  100,  125,  200,  250,  400,   500,   625,
    800, 1000, 1250, 2000, 2500, 4000, 5000, 10000, 20000

    The real value set by gpioPWM is

       (dutycycle * real range) / range.
*/

/* range: 25-40000 */

#define PI_MIN_DUTYCYCLE_RANGE        25
#define PI_MAX_DUTYCYCLE_RANGE     40000



/*-------------------------------------------------------------------------*/
int gpioGetPWMrange(unsigned user_gpio);
/*-------------------------------------------------------------------------*/
/* Returns the dutycycle range used for the gpio.
*/



/*-------------------------------------------------------------------------*/
int gpioGetPWMrealRange(unsigned user_gpio);
/*-------------------------------------------------------------------------*/
/* Returns the real underlying range used for the gpio.
*/



/*-------------------------------------------------------------------------*/
int gpioSetPWMfrequency(unsigned user_gpio,
                        unsigned frequency);
/*-------------------------------------------------------------------------*/
/* Sets the frequency to be used for the gpio.

   The selectable frequencies depend upon the sample rate which
   may be 1, 2, 4, 5, 8, or 10 microseconds (default 5).

   Each gpio can be independently set to one of 18 different PWM
   frequencies.

   The numerically closest frequency will be selected.

   If PWM is currently active on the gpio it will be
   switched off and then back on at the new frequency.

   Upon success the function returns the selected frequency.

   NOTES:

   The frequencies for each sample rate are:

                             Hertz

       1: 40000 20000 10000 8000 5000 4000 2500 2000 1600
           1250  1000   800  500  400  250  200  100   50 

       2: 20000 10000  5000 4000 2500 2000 1250 1000  800
            625   500   400  250  200  125  100   50   25 

       4: 10000  5000  2500 2000 1250 1000  625  500  400
            313   250   200  125  100   63   50   25   13 
sample
 rate
 (us)  5:  8000  4000  2000 1600 1000  800  500  400  320
            250   200   160  100   80   50   40   20   10 

       8:  5000  2500  1250 1000  625  500  313  250  200
            156   125   100   63   50   31   25   13    6 

      10:  4000  2000  1000  800  500  400  250  200  160
            125   100    80   50   40   25   20   10    5
*/



/*-------------------------------------------------------------------------*/
int gpioGetPWMfrequency(unsigned user_gpio);
/*-------------------------------------------------------------------------*/
/* Returns the frequency (Hz) used for the gpio.
*/



/*-------------------------------------------------------------------------*/
int gpioServo(unsigned user_gpio,
              unsigned pulsewidth);
/*-------------------------------------------------------------------------*/
/* Starts servo pulses on the gpio, 0 (off), 500 (most anti-clockwise) to
   2500 (most clockwise).

   NOTES:

   The range supported by servos varies and should probably be determined
   by experiment.  A value of 1500 should always be safe and represents
   the mid-point of rotation.  You can DAMAGE a servo if you command it
   to move beyond its limits.

   EXAMPLE:

   ...
   gpioServo(17, 1500);
   ...

   This example causes an on pulse of 1500 microseconds duration to be
   transmitted on gpio 17 at a rate of 50 times per second.

   This will command a servo connected to gpio 17 to rotate to
   its mid-point.
*/

/* pulsewidth: 0, 500-2500 */

#define PI_SERVO_OFF 0
#define PI_MIN_SERVO_PULSEWIDTH 500
#define PI_MAX_SERVO_PULSEWIDTH 2500


/*-------------------------------------------------------------------------*/
int gpioSetAlertFunc(unsigned        user_gpio,
                     gpioAlertFunc_t f);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) when the specified
   gpio changes state.

   One function may be registered per gpio.

   The function is passed the gpio, the new level, and the tick.

   The alert may be cancelled by passing NULL as the function.

   EXAMPLE:

   void aFunction(int gpio, int level, uint32_t tick)
   {
      printf("gpio %d became %d at %d\n", gpio, level, tick);
   }
   ...
   gpioSetAlertFunc(4, aFunction);
   ...

   This example causes aFunction to be called whenever
   gpio 4 changes state.

   NOTES:

   The gpios are sampled at a rate set when the library is started.

   If a value isn't specifically set the default of 5 us is used.

   The number of samples per second is given in the following table.

                 samples
                 per sec

            1  1,000,000
            2    500,000
   sample   4    250,000
   rate     5    200,000
   (us)     8    125,000
           10    100,000

   Level changes of less than the sample rate may be missed.

   The thread which calls the alert functions is triggered nominally
   1000 times per second.  The active alert functions will be called
   once per level change since the last time the thread was activated.
   i.e. The active alert functions will get all level changes but there
   will be a latency.

   The tick value is the time stamp of the sample in microseconds, see
   gpioTick for more details.
*/


/*-------------------------------------------------------------------------*/
int gpioSetAlertFuncEx(unsigned          user_gpio,
                       gpioAlertFuncEx_t f,
                       void *            userdata);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) when the specified
   gpio changes state.

   One function may be registered per gpio.

   The function is passed the gpio, the new level, the tick, and
   the userdata pointer.

   Only one of gpioSetAlertFunc or gpioSetAlertFuncEx can be
   registered per gpio.

   See gpioSetAlertFunc for further details.
*/



/*-------------------------------------------------------------------------*/
int gpioNotifyOpen(void);
/*-------------------------------------------------------------------------*/
/* This function requests a free notification handle.

   A notification is a method for being notified of gpio state changes
   via a pipe.

   Upon success the function returns a handle greater than or equal
   to zero.

   Notifications for handle x will be available at the pipe
   named /dev/pigpiox (where x is the handle number).  E.g. if the
   function returns 15 then the notifications must be read
   from /dev/pigpio15.

*/



/*-------------------------------------------------------------------------*/
int gpioNotifyBegin(unsigned handle,
                    uint32_t bits);
/*-------------------------------------------------------------------------*/
/* This function starts notifications on a previously opened handle.

   The notification sends state changes for each gpio whose corresponding
   bit in bits is set.

   EXAMPLE:

   gpioNotifyBegin(0, 1234) will start notifications for gpios 1, 4, 6,
   7, 10 (1234 = 0x04D2 = 0b0000010011010010).

   NOTES:

   Each notification occupies 12 bytes in the fifo and has the
   following structure.

   typedef struct
   {
      uint16_t seqno;
      uint16_t flags;
      uint32_t tick;
      uint32_t level;
   } gpioReport_t;

   seqno starts at 0 each time the handle is opened and then increments
   by one for each report.

   flags, if bit 5 is set then bits 0-4 of the flags indicate a gpio
   which has had a watchdog timeout.

   tick is the number of microseconds since system boot.

   level indicates the level of each gpio.
*/

#define PI_NTFY_FLAGS_WDOG     (1 <<5)
#define PI_NTFY_FLAGS_BIT(x) (((x)<<0)&31)



/*-------------------------------------------------------------------------*/
int gpioNotifyPause(unsigned handle);
/*-------------------------------------------------------------------------*/
/* This function pauses notifications on a previously opened handle.

   Notifications for the handle are suspended until gpioNotifyBegin
   is called again.
*/



/*-------------------------------------------------------------------------*/
int gpioNotifyClose(unsigned handle);
/*-------------------------------------------------------------------------*/
/* This function stops notifications on a previously opened handle
   and releases the handle for reuse.
*/



/*-------------------------------------------------------------------------*/
int gpioSetWatchdog(unsigned user_gpio,
                    unsigned timeout);
/*-------------------------------------------------------------------------*/
/* Sets a watchdog for a gpio.
   
   The watchdog is nominally in milliseconds.

   One watchdog may be registered per gpio.

   The watchdog may be cancelled by setting timeout to 0.

   If no level change has been detected for the gpio for timeout
   milliseconds:-

   1) any registered alert function for the gpio is called with
      the level set to PI_TIMEOUT.
   2) any notification for the gpio has a report written to the
      fifo with the flags set to indicate a watchdog timeout.

   EXAMPLE:

   void aFunction(int gpio, int level, uint32_t tick)
   {
      printf("gpio %d became %d at %d\n", gpio, level, tick);
   }
   ...
   gpioSetAlertFunc(4, aFunction);
   gpioSetWatchdogTimeout(4, 5);
   ...

   This example causes aFunction to be called whenever
   gpio 4 changes state or approximately every 5 ms.
*/

/* timeout: 0-60000 */

#define PI_MIN_WDOG_TIMEOUT 0
#define PI_MAX_WDOG_TIMEOUT 60000


/*-------------------------------------------------------------------------*/
int gpioSetGetSamplesFunc(gpioGetSamplesFunc_t f,
                          uint32_t             bits);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) every millisecond
   with the latest gpio samples.

   The function is passed a pointer to the samples and the number
   of samples.

   Only one function can be registered.

   The callback may be cancelled by passing NULL as the function.

   NOTES:

   The samples returned will be the union of bits, plus any active alerts,
   plus any active notifications.

   e.g.  if there are alerts for gpios 7, 8, and 9, notifications for gpios
   8, 10, 23, 24, and bits is (1<<23)|(1<<17) then samples for gpios
   7, 8, 9, 10, 17, 23, and 24 will be reported.
*/



/*-------------------------------------------------------------------------*/
int gpioSetGetSamplesFuncEx(gpioGetSamplesFuncEx_t f,
                            uint32_t               bits,
                            void *                 userdata);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) every millisecond
   with the latest gpio samples.

   The function is passed a pointer to the samples, the number
   of samples, and the userdata pointer.

   Only one of gpioGetSamplesFunc or gpioGetSamplesFuncEx can be
   registered.

   See gpioSetGetSamplesFunc for further details.
*/



/*-------------------------------------------------------------------------*/
int gpioSetTimerFunc(unsigned        timer,
                     unsigned        ms,
                     gpioTimerFunc_t f);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) every ms milliseconds.

   10 timers are supported numbered 0 to 9.

   One function may be registered per timer.

   The timer may be cancelled by passing NULL as the function.

   EXAMPLE:

   ...
   void bFunction(void)
   {
      printf("two seconds have elapsed\n");
   }
   ...
   gpioSetTimerFunc(0, 2000, bFunction);
   ...

   This example causes bFunction to be called every 2000 milliseconds.
*/

/* timer: 0-9 */

#define PI_MIN_TIMER 0
#define PI_MAX_TIMER 9

/* ms: 10-60000 */

#define PI_MIN_MS 10
#define PI_MAX_MS 60000



/*-------------------------------------------------------------------------*/
int gpioSetTimerFuncEx(unsigned          timer,
                       unsigned          ms,
                       gpioTimerFuncEx_t f,
                       void *            userdata);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) every ms milliseconds.

   The function is passed the userdata pointer.

   Only one of gpioSetTimerFunc or gpioSetTimerFuncEx can be
   registered per timer.

   See gpioSetTimerFunc for further details.
*/



/*-------------------------------------------------------------------------*/
int gpioSetSignalFunc(unsigned         signum,
                      gpioSignalFunc_t f);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) when a signal occurs.

   The function is passed the signal number.

   One function may be registered per signal.

   The callback may be cancelled by passing NULL.

   NOTES:

   By default all signals are treated as fatal and cause the library
   to call gpioTerminate() and then exit.
*/

/* signum: 0-63 */

#define PI_MIN_SIGNUM 0
#define PI_MAX_SIGNUM 63



/*-------------------------------------------------------------------------*/
int gpioSetSignalFuncEx(unsigned           signum,
                        gpioSignalFuncEx_t f,
                        void *             userdata);
/*-------------------------------------------------------------------------*/
/* Registers a function to be called (a callback) when a signal occurs.

   The function is passed the signal number and the userdata pointer.

   Only one of gpioSetSignalFunc or gpioSetSignalFuncEx can be
   registered per signal.

   See gpioSetSignalFunc for further details.
*/



/*-------------------------------------------------------------------------*/
uint32_t gpioRead_Bits_0_31(void);
/*-------------------------------------------------------------------------*/
/* Returns the current level of gpios 0-31.
*/



/*-------------------------------------------------------------------------*/
uint32_t gpioRead_Bits_32_53(void);
/*-------------------------------------------------------------------------*/
/* Returns the current level of gpios 32-53.
*/



/*-------------------------------------------------------------------------*/
int gpioWrite_Bits_0_31_Clear(uint32_t levels);
/*-------------------------------------------------------------------------*/
/* Clears gpios 0-31 if the corresponding bit in levels is set.

   EXAMPLE:

   To clear (set to 0) gpios 4, 7, and 15.

   ...
   gpioWrite_Bits_0_31_Clear( (1<<4) | (1<<7) | (1<<15) );
   ...
*/



/*-------------------------------------------------------------------------*/
int gpioWrite_Bits_32_53_Clear(uint32_t levels);
/*-------------------------------------------------------------------------*/
/* Clears gpios 32-53 if the corresponding bit (0-21) in levels is set.
*/



/*-------------------------------------------------------------------------*/
int gpioWrite_Bits_0_31_Set(uint32_t levels);
/*-------------------------------------------------------------------------*/
/* Sets gpios 0-31 if the corresponding bit in levels is set.
*/



/*-------------------------------------------------------------------------*/
int gpioWrite_Bits_32_53_Set(uint32_t levels);
/*-------------------------------------------------------------------------*/
/* Sets gpios 32-53 if the corresponding bit (0-21) in levels is set.

   EXAMPLE:

   To set (set to 1) gpios 32, 40, and 53.

   ...
   gpioWrite_Bits_32_53_Set( (1<<(32-32)) | (1<<(40-32)) | (1<<(53-32)) );
   ...
*/



/*-------------------------------------------------------------------------*/
int gpioTime(unsigned timetype,
             int *    seconds,
             int *    micros);
/*-------------------------------------------------------------------------*/
/* Updates the seconds and micros variables with the current time.

   If timetype is PI_TIME_ABSOLUTE updates seconds and micros with the
   number of seconds and microseconds since the epoch (1st January 1970).

   If timetype is PI_TIME_RELATIVE updates seconds and micros with the
   number of seconds and microseconds since the library was initialised.

   EXAMPLE:

   ...
   int secs, mics;
   ...
   gpioTime(PI_TIME_RELATIVE, &secs, &mics);
   printf("library started %d.%03d seconds ago\n", secs, mics/1000);
   ...
   prints the number of seconds since the library was started.
*/

/* timetype: 0-1 */

#define PI_TIME_RELATIVE 0
#define PI_TIME_ABSOLUTE 1



/*-------------------------------------------------------------------------*/
int gpioSleep(unsigned timetype,
              int      seconds,
              int      micros);
/*-------------------------------------------------------------------------*/
/* Sleeps for the number of seconds and microseconds specified by seconds
   and micros.
  
   If timetype is PI_TIME_ABSOLUTE the sleep ends when the number of seconds
   and microseconds since the epoch (1st January 1970) has elapsed.  System
   clock changes are taken into account.

   If timetype is PI_TIME_RELATIVE the sleep is for the specified number
   of seconds and microseconds.  System clock changes do not effect the
   sleep length.

   NOTES:

   For short delays (say, 250 microseonds or less) use gpioDelayMicroseconds.

   EXAMPLE:

   ...
   gpioSleep(PI_TIME_RELATIVE, 2, 500000); // sleep for 2.5 seconds
   ...
   gpioSleep(PI_TIME_RELATIVE, 0, 100000); // sleep for 1/10th of a second
   ...
   gpioSleep(PI_TIME_RELATIVE, 60, 0);     // sleep for one minute
   ...
*/



/*-------------------------------------------------------------------------*/
uint32_t gpioDelay(uint32_t micros);
/*-------------------------------------------------------------------------*/
/* Delays for at least the number of microseconds specified by micros.

   It returns the actual length of the delay in microseconds.  
*/



/*-------------------------------------------------------------------------*/
uint32_t gpioTick(void);
/*-------------------------------------------------------------------------*/
/* Returns the current system tick.

   Tick is the number of microseconds since system boot.

   NOTES:

   As tick is an unsigned 32 bit quantity it wraps around after
   2^32 microseconds, which is approximately 1 hour 12 minutes.

   You don't need to worry about the wrap around as long as you
   take a tick (uint32_t) from another tick, i.e. the following
   code will always provide the correct difference.

   EXAMPLE:

   uint32_t startTick, endTick;
   int diffTick;
   ...
   startTick = gpioTick();
   ...
   // do some processing
   ...
   endTick = gpioTick();

   diffTick = endTick - startTick;

   printf("some processing took %d microseconds\n", diffTick);
   ...
*/



/*-------------------------------------------------------------------------*/
unsigned gpioHardwareRevision(void);
/*-------------------------------------------------------------------------*/
/* Used to get the hardware revision.

   If the hardware revision can not be found or is not a valid hexadecimal
   number the function returns 0.

   NOTES:

   The hardware revision is the last 4 characters on the Revision line of
   /proc/cpuinfo.

   The revision number can be used to determine the assignment of gpios
   to pins.

   There are at least two types of board.

   Type 1 has gpio 0 on P1-3, gpio 1 on P1-5, and gpio 21 on P1-13.

   Type 2 has gpio 2 on P1-3, gpio 3 on P1-5, gpio 27 on P1-13, and
   gpios 28-31 on P5.

   Type 1 boards have hardware revision numbers of 2 and 3.

   Type 2 boards have hardware revision numbers of 4, 5, 6, and 15.

   EXAMPLES:

   for "Revision	: 0002" the function returns 2.
   for "Revision	: 000f" the function returns 15.
   for "Revision	: 000g" the function returns 0.
*/



/*-------------------------------------------------------------------------*/
int gpioCfgBufferSize(unsigned millis);
/*-------------------------------------------------------------------------*/
/* Configures pigpio to buffer millis milliseconds of gpio samples.

   The default setting is 120 milliseconds.

   NOTES:

   The intention is to allow for bursts of data and protection against
   other processes hogging cpu time.

   I haven't seen a process locked out for more than 100 milliseconds.

   Making the buffer bigger uses a LOT of memory at the more frequent
   sampling rates as shown in the following table in MBs.

                     buffer milliseconds
               120 250 500 1sec 2sec 4sec 8sec

         1      16  31  55  107  ---  ---  ---
         2      10  18  31   55  107  ---  ---
sample   4       8  12  18   31   55  107  ---
 rate    5       8  10  14   24   45   87  ---
 (us)    8       6   8  12   18   31   55  107
        10       6   8  10   14   24   45   87
*/

/* millis */

#define PI_BUF_MILLIS_MIN 100
#define PI_BUF_MILLIS_MAX 10000



/*-------------------------------------------------------------------------*/
int gpioCfgClock(unsigned micros,
                 unsigned peripheral,
                 unsigned source);
/*-------------------------------------------------------------------------*/
/* Configures pigpio to use a sample rate of micros microseconds,
   permitted values are 1, 2, 4, 5, 8 and 10.

   The timings are provided by the specified peripheral (PWM or PCM)
   using the frequency source (OSC or PLLD).

   The default setting is 5 microseconds using the PCM peripheral
   with the PLLD source.

   NOTES:

   The approximate CPU percentage used for each sample rate is:

   sample  cpu
    rate    %

     1     25
     2     16
     4     11
     5     10
     8     15
    10     14

    A sample rate of 5 microseconds seeems to be the sweet spot.

    These readings were done by checking the resources used by
    the demolib program (which is reasonably busy).
*/

/* micros: 1, 2, 4, 5, 8, or 10 */

/* peripheral: 0-1 */

#define PI_CLOCK_PWM 0
#define PI_CLOCK_PCM 1

/* source: 0-1 */

#define PI_CLOCK_OSC  0
#define PI_CLOCK_PLLD 1



/*-------------------------------------------------------------------------*/
int gpioCfgDMAchannel(unsigned channel);
/*-------------------------------------------------------------------------*/
/* Configures pigpio to use the specified DMA channel.

   The default setting is to use channel 14.
*/

/* channel: 0-14 */

#define PI_MIN_DMA_CHANNEL 0
#define PI_MAX_DMA_CHANNEL 14



/*-------------------------------------------------------------------------*/
int gpioCfgDMAchannels(unsigned primaryChannel,
                       unsigned secondaryChannel);
/*-------------------------------------------------------------------------*/
/* Configures pigpio to use the specified DMA channels.

   The default setting is to use channel 14 for the primary channel and
   channel 6 for the secondary channel.
*/

#define PI_MAX_PRIMARY_CHANNEL   14
#define PI_MAX_SECONDARY_CHANNEL  6



/*-------------------------------------------------------------------------*/
int gpioCfgSocketPort(unsigned port);
/*-------------------------------------------------------------------------*/
/* Configures pigpio to use the specified socket port.

   The default setting is to use port 8888.
*/

/* port: 1024-9999 */

#define PI_MIN_SOCKET_PORT 1024
#define PI_MAX_SOCKET_PORT 32000



/*-------------------------------------------------------------------------*/
int gpioCfgInterfaces(unsigned ifFlags);
/*-------------------------------------------------------------------------*/
/* Configures pigpio support of the fifo and socket interfaces.

   The default setting is that both interfaces are enabled.
*/

/* ifFlags: */

#define PI_DISABLE_FIFO_IF 1
#define PI_DISABLE_SOCK_IF 2

/*-------------------------------------------------------------------------*/

/* START EXPERIMENTAL */

int gpioWaveClear          (void);

int gpioWaveAddGeneric     (unsigned numPulses, gpioPulse_t * pulses);

int gpioWaveAddSerial      (unsigned user_gpio,
                            unsigned baud,
                            unsigned numChar,
                            char *   str);

int gpioWaveTxStart        (unsigned mode);

int gpioWaveTxBusy         (void);

int gpioWaveTxStop         (void);

int gpioWaveSerialReadStart(unsigned   user_gpio,
                            unsigned   baud,
                            gpioRx_t * rxp);
int gpioWaveSerialReadStop (unsigned   user_gpio);

int gpioWaveGetMicros      (void);
int gpioWaveGetHighMicros  (void);
int gpioWaveGetMaxMicros   (void);

int gpioWaveGetPulses      (void);
int gpioWaveGetHighPulses  (void);
int gpioWaveGetMaxPulses   (void);

int gpioWaveGetCbs         (void);
int gpioWaveGetHighCbs     (void);
int gpioWaveGetMaxCbs      (void);


#define PI_WAVE_MIN_BAUD      100
#define PI_WAVE_MAX_BAUD      250000
#define PI_WAVE_MAX_CHARS     256
#define PI_WAVE_MAX_PULSES    3000
#define PI_WAVE_MODE_ONE_SHOT 0
#define PI_WAVE_MODE_REPEAT   1

/* END EXPERIMENTAL */



/*-------------------------------------------------------------------------*/
int gpioCfgInternals(unsigned what,
                     int      value);
/*-------------------------------------------------------------------------*/
/* Used to tune internal settings.  Not for general use.
*/



#ifdef __cplusplus
}
#endif

/*-------------------------------------------------------------------------*/

#define PI_CMD_MODES  0
#define PI_CMD_MODEG  1
#define PI_CMD_PUD    2
#define PI_CMD_READ   3
#define PI_CMD_WRITE  4
#define PI_CMD_PWM    5
#define PI_CMD_PRS    6
#define PI_CMD_PFS    7
#define PI_CMD_SERVO  8
#define PI_CMD_WDOG   9
#define PI_CMD_BR1   10
#define PI_CMD_BR2   11
#define PI_CMD_BC1   12
#define PI_CMD_BC2   13
#define PI_CMD_BS1   14
#define PI_CMD_BS2   15
#define PI_CMD_TICK  16
#define PI_CMD_HWVER 17
#define PI_CMD_NO    18
#define PI_CMD_NB    19
#define PI_CMD_NP    20
#define PI_CMD_NC    21
#define PI_CMD_PRG   22
#define PI_CMD_PFG   23
#define PI_CMD_PRRG  24
#define PI_CMD_HELP  25

/*-------------------------------------------------------------------------*/

/* error numbers reported by functions */

#define PI_INIT_FAILED       -1 /* gpioInitialise failed                   */
#define PI_BAD_USER_GPIO     -2 /* gpio not 0-31                           */
#define PI_BAD_GPIO          -3 /* gpio not 0-53                           */
#define PI_BAD_MODE          -4 /* mode not 0-7                            */
#define PI_BAD_LEVEL         -5 /* level not 0-1                           */
#define PI_BAD_PUD           -6 /* pud not 0-2                             */
#define PI_BAD_PULSEWIDTH    -7 /* pulsewidth not 0 or 500-2500            */
#define PI_BAD_DUTYCYCLE     -8 /* dutycycle not 0-255                     */
#define PI_BAD_TIMER         -9 /* timer not 0-9                           */
#define PI_BAD_MS           -10 /* ms not 10-60000                         */
#define PI_BAD_TIMETYPE     -11 /* timetype not 0-1                        */
#define PI_BAD_SECONDS      -12 /* seconds < 0                             */
#define PI_BAD_MICROS       -13 /* micros not 0-999999                     */
#define PI_TIMER_FAILED     -14 /* gpioSetTimerFunc failed                 */
#define PI_BAD_WDOG_TIMEOUT -15 /* timeout not 0-60000                     */
#define PI_NO_ALERT_FUNC    -16 /* DEPRECATED                              */
#define PI_BAD_CLK_PERIPH   -17 /* clock peripheral not 0-1                */
#define PI_BAD_CLK_SOURCE   -18 /* clock source not 0-1                    */
#define PI_BAD_CLK_MICROS   -19 /* clock micros not 1, 2, 4, 5, 8, or 10   */
#define PI_BAD_BUF_MILLIS   -20 /* buf millis not 100-10000                */
#define PI_BAD_DUTY_RANGE   -21 /* dutycycle range not 25-40000            */
#define PI_BAD_SIGNUM       -22 /* signum not 0-63                         */
#define PI_BAD_PATHNAME     -23 /* can't open pathname                     */
#define PI_NO_HANDLE        -24 /* no handle available                     */
#define PI_BAD_HANDLE       -25 /* unknown notify handle                   */
#define PI_BAD_IF_FLAGS     -26 /* ifFlags > 3                             */
#define PI_BAD_CHANNEL      -27 /* DMA channel not 0-14                    */
#define PI_BAD_PRIM_CHANNEL -27 /* DMA primary channel not 0-14            */
#define PI_BAD_SOCKET_PORT  -28 /* socket port not 1024-32000              */
#define PI_BAD_FIFO_COMMAND -29 /* unrecognized fifo command               */
#define PI_BAD_SECO_CHANNEL -30 /* DMA secondary channel not 0-6           */
#define PI_NOT_INITIALISED  -31 /* function called before gpioInitialise   */
#define PI_INITIALISED      -32 /* function called after gpioInitialise    */
#define PI_BAD_WAVE_MODE    -33 /* waveform mode not 0-1                   */
#define PI_BAD_CFG_INTERNAL -34 /* bad parameter in gpioCfgInternals call  */
#define PI_BAD_WAVE_BAUD    -35 /* baud rate not 100-250000                */
#define PI_TOO_MANY_PULSES  -36 /* waveform has too many pulses            */
#define PI_TOO_MANY_CHARS   -37 /* waveform has too many chars             */
#define PI_NOT_SERIAL_GPIO  -38 /* no serial read in progress on gpio      */


/*-------------------------------------------------------------------------*/

#define PI_DEFAULT_BUFFER_MILLIS         120
#define PI_DEFAULT_CLK_MICROS            5
#define PI_DEFAULT_CLK_PERIPHERAL        PI_CLOCK_PCM
#define PI_DEFAULT_CLK_SOURCE            PI_CLOCK_PLLD
#define PI_DEFAULT_IF_FLAGS              0
#define PI_DEFAULT_DMA_CHANNEL           14
#define PI_DEFAULT_DMA_PRIMARY_CHANNEL   14
#define PI_DEFAULT_DMA_SECONDARY_CHANNEL 6
#define PI_DEFAULT_SOCKET_PORT           8888

#endif


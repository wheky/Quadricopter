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
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "pigpio.h"
#include "command.h"

/*
This program provides a socket interface
to the commands available from pigpio.
*/

int main(int argc , char *argv[])
{
   int sock, r, idx, port;
   struct sockaddr_in server;
   cmdCmd_t cmd;
   char * portStr, * addrStr;
   char buf[128];
 
   sock = socket(AF_INET, SOCK_STREAM, 0);
 
   if (sock != -1)
   {
      portStr = getenv(PI_ENVPORT);

      if (portStr) port = atoi(portStr);
      else         port = PI_DEFAULT_SOCKET_PORT;

      addrStr = getenv(PI_ENVADDR);

      if (!addrStr) addrStr="127.0.0.1";

      server.sin_addr.s_addr = inet_addr(addrStr);
      server.sin_family = AF_INET;
      server.sin_port = htons(port);

      if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
      {
         switch(argc)
         {
            case 1:
               exit(0);

            case 2:
               sprintf(buf, "%10s", argv[1]);
               break;

            case 3:
               sprintf(buf, "%10s %10s", argv[1], argv[2]);
               break;

            case 4:
               sprintf(buf, "%10s %10s %10s", argv[1], argv[2], argv[3]);
               break;

            default:
               cmdFatal("what?");
         }

         if ((idx=cmdParse(buf, &cmd)) >= 0)
         {
	     printf("%d %d %d %x\n", cmd.cmd, cmd.p1, cmd.p2, cmd.res);
            if (send(sock, &cmd, sizeof(cmdCmd_t), 0) == sizeof(cmdCmd_t))
            {
               if (recv(sock, &cmd, sizeof(cmdCmd_t), 0) == sizeof(cmdCmd_t))
               {
		   printf("idx=%d rv=%d\n", idx, cmdInfo[idx].rv);
                  switch (cmdInfo[idx].rv)
                  {
                     case 0:
                        r = cmd.res;
                        if (r < 0) cmdFatal("ERROR: %s", cmdErrStr(r));
                        break;

                     case 1:
                        break;

                     case 2:
                        r = cmd.res;
                        if (r < 0) cmdFatal("ERROR: %s", cmdErrStr(r));
                        else printf("%d\n", r);
                        break;

                     case 3:
                        printf("%08X\n", cmd.res);
                        break;

                     case 4:
                        printf("%u\n", cmd.res);
                        break;

                     case 5:
                        printf(cmdUsage);
                        break;
                  }
               }
               else cmdFatal("recv failed, %m");
            }
            else cmdFatal("send failed, %m");
         }
         else cmdFatal("what?");
      }
      else cmdFatal("connect failed, %m");

      close(sock);
   }
   else cmdFatal("socket failed, %m");

   return 0;
}


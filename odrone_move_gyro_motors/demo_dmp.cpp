#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <pigpio.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/time.h>

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "streamGyro.h"
#include "pid.h"

int sock;

void manage_balancing(float ref, float angle, int speed_min, int speed_max, int current_speed, int pin, Pid& pid);

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;

// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
//#define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
//#define OUTPUT_READABLE_WORLDACCEL

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
//#define OUTPUT_TEAPOT

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

Pid::Pid(float p_error_sum, float p_prev_error, float p_Kp, float p_Ki, float p_Kd) :
    error_sum(p_error_sum), prev_error(p_prev_error),
    Kp(p_Kp), Ki(p_Ki), Kd(p_Kd), time(0.0)
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void streamGyro::setup() {
    // initialize device
    printf("Initializing I2C devices...\n");
    mpu.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        printf("DMP ready!\n");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
}

streamGyro::streamGyro(int fd) : _fd(fd)
{
    setup();
    _pid[23] = Pid(0.0, 0.0, 0.7, 0.000092, 0.0000018);/*18*/
    _pid[24] = Pid(0.0, 0.0, 0.7, 0.000092, 0.0000018);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void streamGyro::run()
{
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    if (fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        printf("FIFO overflow!\n");
	write(_fd, "FIFO overflow!", 14);

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (fifoCount >= 42) {
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        #ifdef OUTPUT_READABLE_QUATERNION
            // display quaternion values in easy matrix form: w x y z
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            //printf("quat %7.2f %7.2f %7.2f %7.2f    ", q.w,q.x,q.y,q.z);
            //asprintf(&buff, "quat %7.2f %7.2f %7.2f %7.2f    ", q.w,q.x,q.y,q.z);
	    //write(_fd, buff, strlen(buff));

        #endif


        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

#if 0
            printf("%f", ypr[1] * 180/M_PI);
#endif

            // true = +, false = -
	    /*
	     manage_balancing((ypr[1] * 180/M_PI), 35, 1300, 1500, 50, 17, false);

	     manage_balancing((ypr[1] * 180/M_PI), 25, 1900, 2300, 60, 22, true);

	     manage_balancing((ypr[2] * 180/M_PI), 25, 1850, 2000, 60, 23, false);

	     manage_balancing((ypr[2] * 180/M_PI), 35, 1400, 1600, 40, 24, true);*/
	     manage_balancing(0, (ypr[1] * 180/M_PI), 1000, 1150, 1250, 17, _pid[23]);
	     manage_balancing(0, (ypr[1] * 180/M_PI), 1000, 1150, 1100, 22, _pid[23]);
	     manage_balancing(0, (ypr[2] * 180/M_PI), 1000, 1150, 1100, 23, _pid[24]);
	     manage_balancing(0, (ypr[2] * 180/M_PI), 1000, 1150, 1100, 24, _pid[24]);
            
        #endif
    }
}

void	callback(int signal)
{
    (void)signal;
	printf("a +\n");
	exit(0);
}

// TODO: refaire propre :D
int          set_speed(int pin, int speed)
{
    cmdCmd_t cmd;

    cmd.cmd = PI_CMD_SERVO;
    cmd.p1  = pin;
    cmd.p2  = speed;

    if (send(sock, &cmd, sizeof(cmdCmd_t), 0) == sizeof(cmdCmd_t))
        return 1;
    return -1;
}


int     main() {

    int r, idx, port;
    struct sockaddr_in server;
    char * portStr, * addrStr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error socket\n");
        exit(-1);
    }

    portStr = getenv(PI_ENVPORT);

    if (portStr) port = atoi(portStr);
    else         port = PI_DEFAULT_SOCKET_PORT;

    addrStr = getenv(PI_ENVADDR);

    if (!addrStr) addrStr="127.0.0.1";

    server.sin_addr.s_addr = inet_addr(addrStr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        printf("Error connect\n");
        exit(-1);
    }

    signal(SIGINT, callback);
    streamGyro g(0);
    usleep(100000);
    for (;;)
        g.run();

    return 0;
}


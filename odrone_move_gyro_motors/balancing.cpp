//
// balancing.cpp for  in /root/odrone_move_gyro_motors
// 
// Made by root
// Login   <root@epitech.net>
// 
// Started on  Thu Jan 01 02:35:21 1970 root
// Last update Thu Jan 01 02:35:21 1970 root
//
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>
#include <sys/time.h>
#include "pid.h"

int set_speed(int, int);

int	get_power(int min, int max, int pourcentage)
{
    return (min + (((max - min) / 100) * pourcentage));
}

void manage_balancing(float ref, float angle, int speed_min, int speed_max, int current_speed, int pin, Pid& pid)
{
    struct timeval tv;
    long time = 0;
    long deltaTime = 0;
    float error = 0;
    float derivative = 0;
    int cmd = 0;

    gettimeofday(&tv, 0);
    time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    deltaTime = time - pid.time;

    error = ref - angle;
    derivative = (error - pid.prev_error) * pid.Kd / deltaTime;
    pid.error_sum += error * deltaTime * pid.Ki;

    pid.prev_error = error;
    pid.time = time;

    cmd = (error * pid.Kp) + derivative + pid.error_sum;
    printf("pin:%d\n\t-cmd:%d (error:%f|integral:%f|derivative:%f)\n\n", pin, cmd, error, pid.error_sum, derivative);
    if (pin == 23 || pin == 17)
	set_speed(pin, current_speed - cmd);
    else
	set_speed(pin, current_speed + cmd);
}


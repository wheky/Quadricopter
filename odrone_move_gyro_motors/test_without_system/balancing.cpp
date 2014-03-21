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

int	get_power(int min, int max, int pourcentage)
{
    return (min + (((max - min) / 100) * pourcentage));
}

void set_speed(int, int);
void	manage_balancing(float angle, int max_angle, int min, int max, int base, int pin, bool sens)
{
    /*
    int speed;
    int val;


    val = ((int)angle * 100) / max_angle;
    if (!sens)
	speed = (2 * get_power(min, max, base)) - get_power(min, max, val);
    else
	speed = get_power(min, max, val);

#if 0
    printf("pin = %d, speed = %d\n", pin, speed);
#endif
*/
#define REF	0
#define Kp 	0.005
#define Ki	0.1
#define Kd	0.1
#define	DELTA	(1500.0 - 1100.0) // max - min

    char buffer[1024] = {0};
    float error = REF - angle;
    static float error_sum = 0;
    static float prev_error = 0;
    error_sum += error;
    float delta = error - prev_error;
    prev_error = error;
    int cmd = 1100 + (1 * (((Kp * error) + (Ki * error_sum) + (Kd * delta)) / (100.0 / DELTA)));

    /*
    snprintf(buffer, 1024, "pigs s 24 %d", cmd);
    printf("->%s\n", buffer);
    system(buffer);
    */
    printf("->%d\n", cmd);
    set_speed(24, cmd);
    usleep(100000);

    //gpioServo(pin, speed);
    //system("pigs s 23 1500");
    //sleep(1);
}

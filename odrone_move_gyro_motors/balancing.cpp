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

#define _GNU_SOURCE

int	get_power(int min, int max, int pourcentage)
{
    return (min + (((max - min) / 100) * pourcentage));
}

void	manage_balancing(float angle, int max_angle, int min, int max, int base, int pin, bool sens)
{
    char	*string;
    int speed;

    int val = ((int)angle * 100) / max_angle;

    if (!sens)
        speed = (2 * get_power(min, max, base)) - get_power(min, max, val);
    else
        speed = get_power(min, max, val);

    asprintf(&string, "pigs s %d %d\n", pin ,speed);
    printf(string);
    system(string);
    usleep(20000);

}

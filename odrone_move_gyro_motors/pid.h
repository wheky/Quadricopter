/*
** pid.h for  in /root/Quadricopter/Quadricopter/odrone_move_gyro_motors
** 
** Made by root
** Login   <root@epitech.net>
** 
** Started on  Fri Mar 21 16:03:33 2014 root
** Last update Fri Mar 21 16:03:33 2014 root
*/

#ifndef PID_H
#define PID_H

struct Pid
{
    Pid(float p_error_sum = 0.0, float p_prev_error = 0.0, float Kp = 0.0, float Ki = 0.0, float Kd = 0.0);
    float error_sum;
    float prev_error;
    float Kp;
    float Ki;
    float Kd;
    long time;
};

#endif /* PID_H */

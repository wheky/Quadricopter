/*
** streamGyro.h for  in /home/simoni_w/robotique/dev/network/server/streamGyro
** 
** Made by 
** Login   <simoni_w@epitech.net>
** 
** Started on  Wed Jul 31 00:59:30 2013 
** Last update Wed Jul 31 00:59:30 2013 
*/

#ifndef STREAMGYRO_H
#define STREAMGYRO_H

class streamGyro
{
    public:
	streamGyro(int fd);
	void	setup();
	void 	run();

    private:
	int	_fd;
};

#endif /* STREAMGYRO_H */

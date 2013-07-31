/*
** Network.h for  in /home/simoni_w/robotique/dev/network/client
** 
** Made by 
** Login   <simoni_w@epitech.net>
** 
** Started on  Tue Jul 30 00:05:29 2013 
** Last update Tue Jul 30 00:05:29 2013 
*/

#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <vector>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>

// les streams
#include "streamGyro/streamGyro.h"

/* TODO UDP */
class	Network
{
    public:
	Network(int port = 4242);
	~Network();
	int     newConnection();
	void	createSocket();
	void	run();
	int     setFd(fd_set *fd);
	int     receptCmd(fd_set *fd);
	void	manageStream(const std::string &cmd, int fd);

    private:
	int			_port;
	int			_sock;
	std::vector<int>	_client;
};

#endif /* NETWORK_H */

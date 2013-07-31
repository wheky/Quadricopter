//
// main.cpp for  in /home/simoni_w/robotique/dev/network/client
// 
// Made by 
// Login   <simoni_w@epitech.net>
// 
// Started on  Mon Jul 29 23:58:49 2013 
// Last update Mon Jul 29 23:58:49 2013 

#include "Network.h"

int main(int ac, char **av)
{
    int    	port;

    if (ac == 2)
	port = atoi(av[1]);
    else
	port = 4242;

    Network s(port);
    s.run();

    return 0;
}

//
// Network.cpp for  in /home/simoni_w/robotique/dev/network/client
// 
// Made by 
// Login   <simoni_w@epitech.net>
// 
// Started on  Tue Jul 30 00:11:10 2013 
// Last update Tue Jul 30 00:11:10 2013 
//

#include "Network.h"

Network::Network(int port) : _port(port)
{
    createSocket();
}

Network::~Network()
{
    // TODO On fermera les connections ici
}

static void	fill_sockaddr(int port, struct sockaddr_in *addr)       
{
    addr->sin_family = AF_INET;                                                 
    addr->sin_port = htons(port);                                               
    addr->sin_addr.s_addr = INADDR_ANY;                                         
}                                              

static int	get_sock(int *boolean)                                  
{                                                                               
    struct protoent     *proto;                                                 

    *boolean = 1;                                                               
    proto = getprotobyname("TCP");                                              
    return (socket (AF_INET, SOCK_STREAM, proto->p_proto));                     
}                                                                               
                                                                                              
int    			Network::newConnection()
{
    int                 fd;
    struct sockaddr_in  sin;
    socklen_t           len;

    len = sizeof(struct sockaddr_in);
    if ((fd = accept(_sock, (struct sockaddr *)&sin, &len)) == -1)
    {
	perror("accept");
	return (-1);
    }
    _client.push_back(fd);
    return (fd);
}

int		Network::setFd(fd_set *fd)                      
{                                                              
    int         max_fd;                                        

    max_fd = -1;                                               
    for (std::vector<int>::iterator it = _client.begin() ; it != _client.end(); ++it)
    {
	FD_SET(*it, fd);
	if (*it > max_fd)
	    max_fd = *it;
    }
    return (max_fd);                                           
}                                                              

void			Network::createSocket()
{
    int 		boolean;
    struct sockaddr_in  addr;

    if ((_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
	perror("socket");
	return ;
    }
    bzero((char *) &addr, sizeof(addr));
    _sock = (get_sock(&boolean));
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &boolean, sizeof(int));
    fill_sockaddr(_port, &addr);
    if (bind(_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
	perror("bind");
	exit(-1);
    }
    if (listen(_sock, 42) == -1)
    {
	perror("listen");
	exit(-1);
    }
}

void		Network::manageStream(const std::string &cmd, int fd)
{
    std::cout << "le client a envoyer [" << cmd << "]" << std::endl;
    if (cmd == "gyro")
    {
	streamGyro getG(fd);	
	usleep(100000);

	// TODO A definir suivant quand ca quitte
	for(;;)
	    getG.run();
    }
}

/* TODO refaire Proprement */
int             Network::receptCmd(fd_set *fd)
{                                                               
    char	*buff;
    int		ret;
    bool	end = false;
    int		i = 0;

    buff = (char *)malloc(1024);
    for (std::vector<int>::iterator it = _client.begin() ; it != _client.end(); ++it, i++)
    {
	if (FD_ISSET(*it, fd))
	{
	    while (!end)
	    {
		memset(buff, 0, 1023);
		if ((ret = read(*it, buff, 1023)) > 0)
		{
		    if (ret >= 1 && buff[ret - 1] == '\n')
		    {
			buff[ret - 1] = 0;
			if (ret >= 2 && buff[ret - 2] == '\r')
			    buff[ret - 2] = 0;
			end = true;
		    }
		}
		if (ret == 0 || strncmp(buff, "quit", 4) == 0)
		{
		    std::cout << "client " << *it << " leave" << std::endl;
		    close(*it);
		    _client.erase(_client.begin() + i);	
		    return (0);
		}
	    }
	    manageStream(buff, *it);
	    return (0);
	}
    }
    return (0);
}                                                               

void	Network::run()
{
    int         max_fd;
    fd_set      fd;

    while (1)
    {
	FD_ZERO(&fd);
	FD_SET(_sock, &fd);
	max_fd = setFd(&fd);
	max_fd = (max_fd < _sock) ? _sock : max_fd;
	if (select(max_fd + 1, &fd, NULL, NULL, NULL) == -1)
	{
	    perror("select");
	    exit(-1);
	}
	if (FD_ISSET(_sock, &fd))
	    newConnection();
	else
	    receptCmd(&fd);
    }
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_process_select.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slahlou <slahlou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 12:45:35 by amahla            #+#    #+#             */
/*   Updated: 2022/10/23 21:27:45 by amahla           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <unistd.h>
# include <cstring>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/select.h>
# include <errno.h>
# include <vector>
# include <algorithm>
# include "WebServException.hpp"
# include <cstdlib>
# include <signal.h>
# include <sstream>
# include "webserv.h"

void	signal_handler(int sig)
{
	std::stringstream stringBuf;

	stringBuf << "\r" << "==  Webserv was ended by a signal ( sig code : " << sig << " ) ==";
	throw WebServException( stringBuf.str().c_str() );
}

void	setFds( int & maxSock, fd_set* readFds, std::vector<Client> & clients, std::vector<Server> & servers )
{
	FD_ZERO(readFds);

	for (std::size_t i = 0; i < servers.size(); i++ )
	{
		maxSock = (servers[i].getSock() > maxSock) ? servers[i].getSock() : maxSock;
		FD_SET( servers[i].getSock(), readFds );
	}

	for (std::size_t i = 0; i < clients.size(); i++ )
	{
		maxSock = (clients[i].getSock() > maxSock) ? clients[i].getSock() : maxSock;
		FD_SET(clients[i].getSock(), readFds);
	}
}

void	waitRequest( fd_set* readFds, std::vector<Client> & clients, std::vector<Server> & servers )
{
	int				status = 0;
	int				maxSock = 0;
	struct timeval	timeout;

	while (!status)
	{
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		setFds( maxSock, readFds, clients, servers );

		if (( status = select( maxSock + 1, readFds, NULL, NULL, &timeout )) < 0 )
			throw WebServException( "serv_process_select.cpp", "waitRequest", "select() failed" );
	}
}

void	newConnection( std::vector<Server> & servers, std::vector<Client> & clients, fd_set* readFds )
{
	struct sockaddr_in	temp;
	int					addrlen = sizeof(temp);
	int					newConnection;

	memset( (char *)&temp, 0, sizeof(temp) );

	for (std::size_t i = 0; i < servers.size(); i++ )
	{
		if ( FD_ISSET( servers[i].getSock(), readFds ) )
		{
			if ( (( newConnection = accept( servers[i].getSock(), reinterpret_cast< struct sockaddr *>(&temp),
								reinterpret_cast< socklen_t * >(&addrlen) ) ) < 0) )
			{
				if ( errno != EWOULDBLOCK )
					throw WebServException( "serv_process_select.cpp", "newConnection", "accept() failed" );
			}

//			else ( newConnection peut-il etre egal a 0 ? )
			if (newConnection > 0)			
			{
				std::cout << YELLOW << "Connection accepted" << SET << std::endl;
				nonBlockSock( newConnection );
				clients.push_back( Client( newConnection ) );
			}
		}
	}
}

void	ioData( fd_set* readFds, std::vector<Client> & clients )
{
	char		buffer_read[1024];
	const char	*buffer_write;
	int		rd;

	for (std::size_t i = 0; i < clients.size(); i++ )
	{
		if ( FD_ISSET( clients[i].getSock(), readFds ) )
		{
			if ( (rd = recv( clients[i].getSock() , buffer_read, 1023, 0 )) <= 0 )
			{
				if ( rd < 0 )
					std::cout << RED << "Connexion client lost" << SET << std::endl;
				else
					std::cout << RED << "Connexion client is closed" << SET << std::endl;
				FD_CLR( clients[i].getSock(), readFds);
				close( clients[i].getSock() );
				clients.erase( clients.begin() + i );
			}
			else
			{
				buffer_read[rd] = '\0';
				// clients[i].getRequest().getStringRequest() += buffer_read; For concatenate recv
				clients[i].getRequest().getStringRequest() = buffer_read;

				buffer_write = clients[i].getResponse().getStringResponse().c_str();
				std::cout << GREEN << "Server side receive from client : " << clients[i].getRequest().getStringRequest() << SET << std::endl;
				if ( send( clients[i].getSock(), buffer_write, strlen(buffer_write), 0 ) < 0 )
					std::cout << RED << "Connexion client lost" << SET << std::endl;
			}
		}
	}
}

void	appServ( std::vector<Server> & servers )
{
	std::vector<Client>	clients;
	fd_set	readFds;

	signal(SIGINT, &signal_handler);

	std::cout << std::endl << "\r" << YELLOW << "-------------- Server is on ---------------" << std::endl << SET << std::endl;
	try
	{
		while (1)
		{
			waitRequest( &readFds, clients, servers );
			newConnection( servers, clients, &readFds );
			ioData( &readFds, clients );
		}
	}
	catch ( std::exception & e )
	{
		for (std::size_t i = 0; i < servers.size(); i++ )
		{
			FD_CLR( servers[i].getSock(), &readFds );
			close( servers[i].getSock() );
		}
		for (std::size_t i = 0; i < clients.size(); i++ )
		{
			FD_CLR( clients[i].getSock(), &readFds );
			close( clients[i].getSock() );
		}
		throw ( WebServException( e.what() ) );
	}
}

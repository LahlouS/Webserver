/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_settings.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 12:45:35 by amahla            #+#    #+#             */
/*   Updated: 2022/10/31 18:54:15 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>

# include <cstring>
# include <sys/socket.h>
# include <sys/types.h>
# include <fcntl.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include "WebServException.hpp"
# include <cstdlib>
# include "webserv.h"

int		createSock( int & servSock )
{
	int	on = 1;

	if (( servSock = socket(AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		std::cout << "socket() failed" << std::endl;
		exit( EXIT_FAILURE );
	}

	if ( setsockopt( servSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		&on, sizeof(int) ) < 0 )
	{
		throw WebServException( "socket_setting.cpp", "createSock", "setsockopt() failed" );
	}

	return ( servSock );
}

void	nonBlockSock( int & servSock )
{
	int flag;

	if ( (flag = fcntl( servSock, F_GETFL )) < 0 )
		throw WebServException( "socket_setting.cpp", "nonBlockSock", "fcntl() F_GETFL failed" );

	if ( fcntl( servSock, F_SETFL, flag | O_NONBLOCK ) < 0 )
		throw WebServException( "socket_setting.cpp", "nonBlockSock", "fcntl() F_SETFL failed" );
}

void	nameSock( int & servSock, Server & server )
{
	struct sockaddr_in	address;
	memset( (char *)&address, 0, sizeof(address) );
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl( INADDR_ANY );
	address.sin_port = htons( server.getPort() );

	if ( bind( servSock, reinterpret_cast< struct sockaddr * >(&address),
		sizeof(address)) < 0 )
	{
		throw WebServException( "socket_setting.cpp", "nameSock", "bind() failed" );
	}

	if ( listen( servSock, 100) < 0 )
		throw WebServException( "socket_setting.cpp", "nameSock", "listen() failed" );

}

void	setServerSockets( std::vector<Server> & servers )
{
	bool	check;
	try
	{
		for ( std::size_t i(0); i < servers.size(); i++ )
		{
			check = false;
			for ( std::size_t j(0); j < i; j++ )
			{
				if ( servers[j].getPort() == servers[i].getPort() )
				{
					servers[i].getSock() = -1;
					check = true;
					break ;
				}
				
			}
			if ( check )
				continue ;
			createSock( servers[i].getSock() );
			nonBlockSock( servers[i].getSock() );
			nameSock( servers[i].getSock(), servers[i] );
		}

	}
	catch ( std::exception & e )
	{
		for ( std::size_t i(0); i < servers.size(); i++ )
		{
			if ( servers[i].getSock() >= 0 )
				close( servers[i].getSock() );
		}
		throw WebServException( e.what() );
	}
}

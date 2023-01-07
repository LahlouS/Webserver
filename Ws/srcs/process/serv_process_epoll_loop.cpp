/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_process_epoll_loop.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 12:45:35 by amahla            #+#    #+#             */
/*   Updated: 2022/11/12 16:31:11 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <cstring>
# include <netinet/in.h>
# include <errno.h>
# include <signal.h>
# include "webserv.h"
# include "epoll.h"
# include <unistd.h>


void	waitRequest( t_epoll & epollVar )
{
	int	timeout = 400;

	if (( epollVar.maxNbFd = epoll_wait( epollVar.epollFd, epollVar.events, MAXFD, timeout) ) < 0 )
		throw WebServException( "serv_process_epoll_process.cpp", "waitRequest", "epoll_wait() failed" );
}

void	readData( std::vector<Client> & clients, itClient it, t_epoll & epollVar, int i )
{

	it->setRequest( epollVar, i );
	if ( it->getReadStatus() <= 0 )
		clients.erase( it );

}


void	sendData( std::vector<Client> & clients, itClient it, t_epoll & epollVar, int i )
{
		it->getResponse(*(it->getServer()),
			it->getRequest(), epollVar.events[i].data.fd);

		unsigned char	buffer_write[it->getResponse().getBufferVec().size() + 1];
		it->getResponse().fillbuf(buffer_write);
		if (!(it->getResponse().getIsCGI()) &&  send( epollVar.events[i].data.fd, buffer_write, it->getResponse().getBufferVec().size(), 0 ) <= 0 )
		{
			std::cout << RED << "Connexion client lost" << SET << std::endl;
			clients.erase( it );
			epoll_ctl(epollVar.epollFd, EPOLL_CTL_DEL, epollVar.events[i].data.fd, NULL);
			close( epollVar.events[i].data.fd );
		}

		epollVar.new_event.events = EPOLLIN;
		epollVar.new_event.data.fd = epollVar.events[i].data.fd;
		epoll_ctl(epollVar.epollFd, EPOLL_CTL_MOD, epollVar.events[i].data.fd, &epollVar.new_event);
}

bool	ioData( std::vector<Client> & clients, t_epoll & epollVar, int i)
{
	itClient	it = find( clients, epollVar.events[i].data.fd );

	if ( epollVar.events[i].events & EPOLLIN )
	{
		readData( clients, it, epollVar, i );
		return ( true );
	}
	else if ( epollVar.events[i].events & EPOLLOUT )
	{
		sendData( clients, it, epollVar, i );
		return ( true );
	}
	return ( false );
}

void	addConnection( std::vector<Client> & clients, std::vector< Server > & servers, t_epoll & epollVar,
			Server & serverToConnect )
{
	struct sockaddr_in	temp;
	int					addrlen = sizeof(temp);
	int					newConnection;

	memset( (char *)&temp, 0, sizeof(temp) );

	if ( ( newConnection = accept( serverToConnect.getSock(), reinterpret_cast< struct sockaddr *>(&temp),
					reinterpret_cast< socklen_t * >(&addrlen) ) ) < 0 )
	{
		if ( errno != EWOULDBLOCK )
			throw WebServException( "serv_process_epoll_loop.cpp", "newConnection", "accept() failed" );
	}

	else if ( newConnection >= 0 )
	{

		std::cout << std::endl;
		nonBlockSock( newConnection );

		epollVar.new_event.data.fd = newConnection;
		epollVar.new_event.events = EPOLLIN;
		epoll_ctl( epollVar.epollFd, EPOLL_CTL_ADD, newConnection, &(epollVar.new_event ));

		clients.push_back( Client( newConnection ) );
		if ( !whichAddrServer( servers, clients.back(), temp.sin_addr.s_addr, serverToConnect.getPort() ) )
		{
			std::cout << YELLOW << "Connection refused" << SET << std::endl;
			epoll_ctl( epollVar.maxNbFd, EPOLL_CTL_DEL, newConnection, NULL);
			clients.pop_back();
			close( newConnection );
		}
		else
			std::cout << YELLOW << "Connection accepted" << SET << std::endl;
	}
}

bool	newConnection( std::vector<Client> & clients, std::vector<Server> & servers, t_epoll & epollVar, int i )
{
	Server	* serverToConnect;

	if ( epollVar.events[i].events & EPOLLIN &&
		( serverToConnect = isServer( servers, epollVar.events[i].data.fd ) ) != NULL  )
	{
		addConnection( clients, servers, epollVar, *serverToConnect );
		return ( true );
	}
	return ( false );
}

bool	errorEpoll( std::vector<Server> & servers, std::vector<Client> & clients, t_epoll & epollVar, int i )
{
	if ( epollVar.events[i].events & EPOLLERR || epollVar.events[i].events & EPOLLHUP )
	{
		int	fdToClear = epollVar.events[i].data.fd;

		epoll_ctl( epollVar.maxNbFd, EPOLL_CTL_DEL, epollVar.events[i].data.fd, NULL);
		if ( isServer( servers, fdToClear ) == NULL )
			clients.erase( find( clients, fdToClear ) );
		else
			servers.erase( find( servers, fdToClear ) );
		close( fdToClear );
		return ( true );
	}
	return ( false );
}

void	servProcess( std::vector<Server> & servers, std::vector<Client> & clients, t_epoll & epollVar )
{

	setEpollQueue( epollVar, servers );

	while (1)
	{

		waitRequest( epollVar );
		for ( int i(0); i < epollVar.maxNbFd; i++)
		{
			if ( errorEpoll( servers, clients, epollVar, i ) )
				continue ;
			if ( !newConnection( clients, servers, epollVar, i ) )
				ioData( clients, epollVar, i );

		}
	}
}

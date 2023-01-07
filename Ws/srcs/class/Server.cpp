/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slahlou <slahlou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 14:51:31 by amahla            #+#    #+#             */
/*   Updated: 2022/11/12 14:34:50 by slahlou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include "../../headers/defines.h"
#include <iostream>
#include <sstream>
#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>

Server::Server( void ) : _servSock(0), _port(8080), _inetAddr(INADDR_ANY), _clientBody(16000), _autoindex( false ), _root("html/"),
_allowDelete(false), _allowGet(false), _allowPost(false), _redirect(false)
{
	if ( DEBUG )
		std::cout << "Server Default Constructor" << std::endl;

	this->_index.push_back("index.html");
	this->_serverName.push_back("");

	this->_is_set["listen"] = false;
	this->_is_set["server_name"] = false;
	this->_is_set["location"] = false;
	this->_is_set["autoindex"] = false;
	this->_is_set["clientbody"] = false;
	this->_is_set["error_page"] = false;
	this->_is_set["index"] = false;
	this->_is_set["root"] = false;
	this->_is_set["allowMethods"] = false;
}

Server::Server( const Server & rhs )
{
	if ( DEBUG )
		std::cout << "Server copy Constructor" << std::endl;
	*this = rhs;
}

Server::~Server( void )
{
	if ( DEBUG )
		std::cout << "Server Default Destructor" << std::endl;
}

Server &			Server::operator=( const Server & rhs )
{
	if ( this != &rhs )
	{
		this->_serverName = rhs._serverName;
		this->_servSock = rhs._servSock;
		this->_port = rhs._port;
		this->_clientBody = rhs._clientBody;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_error_pages = rhs._error_pages;
		this->_is_set = rhs._is_set;
		this->_location = rhs._location;
		this->_autoindex = rhs._autoindex;
		this->_inetAddr = rhs._inetAddr;
		this->_portStr = rhs._portStr;
		this->_allowDelete = rhs._allowDelete;
		this->_allowGet = rhs._allowGet;
		this->_allowPost = rhs._allowPost;
		this->_redirect = rhs._redirect;
		this->_redirectStr = rhs._redirectStr;
	}
	return ( *this );
}

std::map< std::string, bool >	& Server::getMap( void )
{
	return ( this->_is_set );
}

int								& Server::getSock( void )
{
	return ( this->_servSock );
}

const int								& Server::getSock( void ) const
{
	return ( this->_servSock );
}

std::vector< std::string >				& Server::getServerName( void )
{
	return ( this->_serverName );
}

std::map< std::string, Server >			& Server::getLocation( void )
{
	return ( this->_location );
}

const short								& Server::getPort( void ) const
{
	return ( this->_port );
}

short								& Server::getPort( void )
{
	return ( this->_port );
}

std::string	const						& Server::getPortStr( void ) const
{
	return (this->_portStr);
}

in_addr_t								& Server::getInetAddr( void )
{
	return (this->_inetAddr);
}

std::map< std::string, std::string >	& Server::get_error_pages(void)
{
	return (this->_error_pages);
}

bool									Server::get_is_set( std::string attribute )
{
	return ( this->_is_set[attribute] );
}

void									Server::set_is_set( std::string attribute )
{
	this->_is_set[attribute] = true;
}

int										Server::get_clientBody(void) const
{
	return (this->_clientBody);
}

void									Server::set_clientBody( const int val )
{
	this->_clientBody = val;
}

std::string								Server::get_root(void) const
{
	return ( this->_root );
}

bool									Server::getAutoindex( void ) const
{
	return ( this->_autoindex);
}

bool    								Server::getAllowGet( void )
{
	return (_allowGet);
}

bool    								Server::getAllowPost( void )
{
	return (_allowPost);
}

bool    								Server::getAllowDelete( void )
{
	return (_allowDelete);
}

bool									Server::getRedirect( void )
{
	return (_redirect);
}

std::string const &						Server::getRedirectStr( void )
{
	return (_redirectStr);
}

void									Server::set_root( const std::string root )
{
	_root.clear();
	_root = root;
}

std::vector< std::string >				& Server::get_index(void)
{
	return ( this->_index );
}

void									Server::setSock( const int servSock )
{
	this->_servSock = servSock;
}

void									Server::setAddr(in_addr_t addr)
{
	this->_inetAddr = addr;
}

void									Server::setPort( const int port )
{
	this->_port = port;
}

void									Server::setPort( std::string const & port )
{
	this->_portStr = port;
}

void									Server::setAutoindex( bool onOff )
{
	this->_autoindex = onOff;
}

void    								Server::setAllowDelete(bool val)
{
	_allowDelete = val;
}

void    								Server::setAllowPost(bool val)
{
	_allowPost = val;
}

void									Server::setAllowGet(bool val)
{
	_allowGet = val;
}

std::string								Server::getAddrStr( void ) const
{
	std::stringstream	ss;

	for ( int i = 3, bit = 0; i >= 0; i-- )
	{
		int addr = ( this->_inetAddr >> bit) & 255;
		ss << addr;
		if ( i )
			ss << ".";
		bit += 8;
	}
	return( ss.str() );
}

void									Server::setRedirect( bool redirect )
{
	_redirect = redirect;
}

void									Server::setRedirectStr( std::string redirectStr )
{
	_redirectStr = redirectStr;
}

std::ostream	& operator<<( std::ostream & o, Server rhs )
{
	o << "Server :" << std::endl << std::endl;
	if ( !rhs.getServerName().empty() )
	{
		o << "	server_name: ";
		for ( std::size_t i(0); i < rhs.getServerName().size(); i++ )
			o << "	" << rhs.getServerName()[i] << " ";
		o << std::endl;
	}
	o << "	socket_fd :	" << rhs.getSock() << std::endl;
	o << "	port:		" << rhs.getPort() << std::endl;
	o << "	addr:		";
	for ( int i = 3, bit = 0; i >= 0; i-- )
	{
		int addr = ( rhs.getInetAddr() >> bit) & 255;
		o << addr;
		if ( i )
			o << ".";
		bit += 8;
	}
	o << std::endl << "	clientbody:	" << rhs.get_clientBody() << std::endl;
	o << "	autoindex:	" << (rhs.getAutoindex() ? "true" : "false") << std::endl;
	o << "	root:		" << rhs.get_root() << std::endl;
	if ( !rhs.get_index().empty() )
	{
		o << "	index: " << std::endl;
		for ( std::size_t i(0); i < rhs.get_index().size(); i++ )
			o << "			" << rhs.get_index()[i] << std::endl;
		o << std::endl;
	}
	if ( !rhs.get_error_pages().empty() )
	{
		std::cout << "	map:" << std::endl;
		for ( std::map< std::string, std::string >::iterator it( rhs.get_error_pages().begin() );
				it != rhs.get_error_pages().end(); it++ )
			o << "			" << it->first << " => " << it->second << std::endl;
	}
	o << "	methodes:" << std::endl;
	if (rhs.getAllowDelete())
		o << "			DELETE = true\n" ;
	else
		o << "			DELETE = false\n" ;
	if (rhs.getAllowPost())
		o << "			POST = true\n" ;
	else
		o << "			POST = false\n" ;
	if (rhs.getAllowGet())
		o << "			GET = true\n" ;
	else
		o << "			GET = false\n" ;
	if ( !rhs.getLocation().empty() )
	{
		for ( std::map< std::string, Server >::iterator it( rhs.getLocation().begin() );
			it != rhs.getLocation().end(); it++ )
		{
			std::cout << std::endl << "	Location for file";
			o << " \"" << it->first << "\" infos " << it->second;
		}
		o << std::endl;
	}
	o << "\n allowed methodes : \n";
	if (rhs.getAllowDelete())
		o << "--> DELETE = true\n" ;
	else
		o << "--> DELETE = false\n" ;
	if (rhs.getAllowPost())
		o << "--> POST = true\n" ;
	else
		o << "--> POST = false\n" ;
	if (rhs.getAllowGet())
		o << "--> GET = true\n" ;
	else
		o << "--> GET = false\n" ;

	if (rhs.getRedirect())
		std::cout << "there is a redirection into : \n\t\t-->" << rhs.getRedirectStr() << "<--\n\n";
	else
		std::cout << "\n\n no redirection set\n";
	return ( o );
}

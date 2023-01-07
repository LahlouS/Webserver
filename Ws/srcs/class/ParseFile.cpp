/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseFile.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:41:45 by amahla            #+#    #+#             */
/*   Updated: 2022/11/12 11:14:21 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <cstdlib>
# include "ParseFile.hpp"
# include <fstream>
# include "WebServException.hpp"


ParseFile::ParseFile( const char *av )
{
	if ( DEBUG )
		std::cout << "ParseFile Default Constructor" << std::endl;

	this->_ft[0] = &ParseFile::setServerName;
	this->_ft[1] = &ParseFile::error_page;
	this->_ft[2] = &ParseFile::client_body;
	this->_ft[3] = &ParseFile::root;
	this->_ft[4] = &ParseFile::index;
	this->_ft[5] = &ParseFile::listenParse;
	this->_ft[6] = &ParseFile::autoindexParse;
	this->_ft[7] = &ParseFile::allowedMethodsParse;
	this->_ft[8] = &ParseFile::redirectParse;
	this->readFile( av );
	// reset les location Server en fonction du server parent
	// resetLocation();
}

ParseFile::ParseFile( const ParseFile & rhs )
{
	if ( DEBUG )
		std::cout << "ParseFile copy Constructor" << std::endl;
	*this = rhs;
}

ParseFile::~ParseFile( void )
{
	if ( DEBUG )
		std::cout << "ParseFile Default Destructor" << std::endl;
}

ParseFile &	ParseFile::operator=( const ParseFile & rhs )
{
	if ( this != &rhs )
		this->_servers = rhs._servers;
	return ( *this );
}

std::vector<Server>	& ParseFile::getServers( void )
{
	return ( this->_servers );
}

const std::vector<Server>	& ParseFile::getServers( void ) const
{
	return ( this->_servers );
}

int	whileSpace( std::string temp )
{
	int	i = 0;

	while ( temp[i] == ' ' || temp[i] == '\t' )
		i++;
	return ( i );
}

void	ParseFile::readContent( std::ifstream & ifs, std::string temp, const std::string file, Server *parent )
{
	Server	server;
	int		i, j;

	if ( parent )
	{
		server = *parent;
		for (std::map< std::string, bool >::iterator it = server.getMap().begin();
				it != server.getMap().end(); it++ )
			it->second = false;
		server.setAllowDelete(false);
		server.setAllowGet(false);
		server.setAllowPost(false);
		server.getLocation().clear();
		server.setRedirect(false);
	}

	while ( std::getline( ifs, temp ).good() )
	{
		i = 0;
		i += whileSpace( temp.c_str());
		if ( !temp[i] )
			continue ;
		if ( temp[i] == '}' )
		{
			i++;
			i += whileSpace( temp.c_str() + i);
			if ( temp[i] )
				throw WebServException( "ParseFile.cpp", "readFile", "Invalid format config file" );
			else
			{
				if ( !parent )
					this->_servers.push_back( server );
				else
					parent->getLocation()[ file ] = server;
				break ;
			}
		}

		for ( j = 0; j < 9 ; j++ )
		{
			if ( parent && ( j == 5 || j == 0 || j == 2 )){
				continue ;
			}
			if ( (this->*_ft[j])( temp.c_str() + i, server ) ){
				break ;
			}
		}
		if ( j == 9 && !setLocation( ifs, temp.c_str() + i, server ) )
			throw WebServException( "ParseFile.cpp", "readFile", "Invalid format config file" );
	}
}

void	ParseFile::FormatFile( std::ifstream & ifs, std::string temp )
{
	int	i;

	do
	{
		i = 0;
		i += whileSpace( temp.c_str() + i );
		if ( !temp[i] )
			continue ;
		if ( temp[i] && temp.compare( i, 6, "server" ) != 0 )
			throw WebServException( "ParseFile.cpp", "readFile", "Invalid format config file" );
		else
		{
			i += 6;
			i += whileSpace( temp.c_str() + i );
			if ( temp[i++] == '{' )
			{
				i += whileSpace( temp.c_str() + i );
				if ( temp[i] )
					throw WebServException( "ParseFile.cpp", "readFile", "Invalid format config file" );
				else
					readContent( ifs, temp.c_str() + i, "", NULL );
			}
			else
				throw WebServException( "ParseFile.cpp", "readFile", "Invalid format config file" );
		}
	} while ( std::getline( ifs, temp ).good() );
	if ( ifs.bad() )
		throw WebServException( "ParseFile.cpp", "readFile", "An error occurred while reading the config file" );
}

void	ParseFile::readFile( const char *file )
{
	std::ifstream		ifs( file, std::ifstream::in );
	std::string			temp;

	if ( !ifs.is_open() )
		throw WebServException( "ParseFile.cpp", "readFile", "File not found or opening impossible" );
	std::getline( ifs, temp );
	if ( ifs.eof() && temp.empty() )
		throw WebServException( "ParseFile.cpp", "readFile", "Empty config file" );
	FormatFile( ifs, temp );

	ifs.close();
}

bool	ParseFile::setServerName( const std::string str, Server & server )
{
	std::string	temp;
	int			i = 0;

	if ( server.getMap()["server_name"] )
		return ( false );
	if ( str.compare( i, 11, "server_name" ) == 0 )
	{
		i += 11;
		while ( str[i] && str[i] != ';' )
		{
			i += whileSpace( str.c_str() + i );
			if ( str.compare( i, 3, "\"\"" ) == 0 )
				i += 2;
			if ( str[i] == ';' )
				break ;
			while ( str[i] && !( str[i] == ' ' || str[i] == '\t' || str[i] == ';' ) )
				temp.push_back( str[i++] );
			server.getServerName().push_back( temp );
			temp.clear();
		}
		i = i + whileSpace( str.c_str() + i ) + 1;
		if ( str[i] )
				return ( false );
	}
	else
		return ( false );

	server.getMap()["server_name"] = true;
	return ( true );
}

bool	ParseFile::setLocation( std::ifstream & ifs, std::string temp, Server & server )
{
	std::string	file;
	int			i = 0;

	try
	{
		if ( temp.compare( i, 8, "location" ) != 0 )
			throw WebServException( "ParseFile.cpp", "setLocation", "Invalid format config file" );
		i += 8;
		i += whileSpace( temp.c_str() + i );
		while ( temp[i] >= 33 && temp[i] <= 126 )
			file.push_back( temp[i++] );
		i += whileSpace( temp.c_str() + i );
		if ( temp[i++] == '{' )
		{
			i += whileSpace( temp.c_str() + i );
			if ( temp[i] || file.empty() )
				throw WebServException( "ParseFile.cpp", "setLocation", "Invalid format config file" );
			else
				readContent( ifs, temp.c_str() + i, file, &server );
		}
		else
			throw WebServException( "ParseFile.cpp", "setLocation", "Invalid format config file" );
	}
	catch ( std::exception & e )
	{
		return ( false );
	}
    server.set_is_set("location");
	return ( true );
}

//			Code Max

bool	ParseFile::error_page(const std::string line_const, Server &server)
{
    std::string line = line_const;
    std::string rslt;
    std::map< std::string, std::string > error_map;

    if (!get_the_info_i_need(line, "error_page", rslt))
        return (false);

    if (!get_key_map_error(rslt, error_map))
        return (false);

    if (!get_value_map_error(rslt, error_map))
        return (false);

//	std::map< std::string, std::string > &map_error = server.get_error_pages();
	if ( !error_map.empty() )
		server.get_error_pages().insert(error_map.begin(), error_map.end());
    server.set_is_set("error_page");

    //default value: none
    return (true);
}

bool	ParseFile::client_body(const std::string line_const, Server &server)
{
	std::string line = line_const;
	std::string rslt;
	int     i = 0;

	if (!get_the_info_i_need(line, "client_body_buffer_size", rslt)
		|| line.find("-", 0) != std::string::npos)
		return (false);

	while ( std::isdigit(rslt[++i]) );

	if (rslt[i] && ( std::tolower(rslt[i]) == 'k') && !rslt[i + 1])
	{
		rslt.erase(i, 1);
		rslt += "000";
	}
	else if (rslt[i])
		return (false);

	if (server.get_is_set("clientbody"))
		return (false);

	server.set_clientBody( std::atoi(rslt.c_str()) );
	server.set_is_set("clientbody");

	//default value depend of architecture: 8K if 32 bits / 16k if 64 bits
	return (true);

}

bool	ParseFile::root(const std::string line_const, Server &server)
{
	std::string	rslt;
	std::string	back_slash = "";
	std::string	line = line_const;

	if (!get_the_info_i_need(line, "root", rslt))
		return (false);

	if (server.get_is_set("root") || get_nb_of_file(rslt) != 1)
		return (false);

	server.set_is_set("root");

	if ( *(rslt.rbegin()) != '/' )
		back_slash = "/";
	server.set_root(rslt + back_slash);

	return (true);
}

bool	ParseFile::index(const std::string line_const, Server &server)
{
	std::string line = line_const;
	std::string rslt;
	int len = 0;
	std::string tmp;
	std::vector<std::string>  temp;
	std::string::iterator it;

	if (!get_the_info_i_need(line, "index", rslt))
		return (false);
	it = rslt.begin();
	while (it != rslt.end())
	{
		if (is_white_space(*it))
		{
			while ((it != rslt.end()) && is_white_space(*it))
				it++;
		}
		else
		{
			len = 0;
			while ((it + len != rslt.end()) && !is_white_space(*(it + len)))
				len++;
			tmp.insert(tmp.begin(), it, it + len);
			if (tmp != "index.html")
				temp.push_back(tmp);
			tmp.clear();
			it += len;
		}
	}
	if ( server.get_is_set("index") )
		return (false);
	std::vector<std::string> &index_vector = server.get_index();
	index_vector.insert(index_vector.end(), temp.begin(), temp.end());
	server.set_is_set("index");
	//default value: index.html
	return (true);
}

//			Code Sacha

bool	ParseFile::listenParse( const std::string str_const, Server & serv )
{
	int				i = 0;
	std::string	str = str_const;

	if ( serv.get_is_set( "listen" ) )
		return ( false );
	if ( !str.compare(0, 6, "listen") )
	{
		i += 6;
		while ( str[i] == ' ' || str[i] == '\t' )
			i++;
		if ( !checkSyntax( str.c_str() + i ) )
			return ( false );
		if (checkOccurance(str, ":") == 1)
		{
			if ( addrIsGood(serv, str.substr(i, str.find(":", 0) - i) )
				&& portIsGood(serv, str.substr((str.find(":", 0) + 1),
				( (str.find(";", 0)) - (str.find(":", 0) + 1) )) ))
			{
				serv.set_is_set( "listen" );
				return (true);
			}
		}
		else
		{
			if ( addrIsGood(serv, str.substr(i, str.find(";", 0) - i)) ){
				serv.setPort(8080);
				serv.set_is_set( "listen" );
				return (true);
			} else if ( portIsGood(serv, str.substr(i, str.find(";", 0) - i)) ) {
				serv.set_is_set( "listen" );
				serv.setAddr((in_addr_t) INADDR_ANY);
				return (true);
			} else {
				return (false);
			}
		}
	}
	serv.setAddr((in_addr_t) INADDR_ANY);
	return (false);
}

bool	ParseFile::autoindexParse( const std::string str_const, Server & serv )
{
	int				i = 0;
	std::string	str = str_const;
	if ( !( str.compare(0, 9, "autoindex") ) )
	{
		i += 9;
		while ( str[i] == ' ' || str[i] == '\t' )
			i++;
		if ( !checkSyntaxIndex( (str.c_str() + i ) ) )
			return ( false );
		if ( !str.compare(i, 4, "off;") )
		{
			serv.set_is_set( "listen" );
			serv.setAutoindex(false);
			return (true);
		}
		else if (!str.compare(i, 3, "on;") )
		{
			serv.set_is_set( "listen" );
			serv.setAutoindex(true);
			return (true);
		} else
			return (false);
	}
	return (false);
}


bool	ParseFile::allowedMethodsParse(std::string str, Server & serv)
{
    int i = 0;
	if ( serv.get_is_set( "allowMethods" ) )
		return ( false );
    if ( !(str.compare(0, 8, "methodes")) && checkOccurance(str, ";") == 1 && afterSemiColon(str))
	{
        for (i = 8; str[i] == ' ' || str[i] == '\t'; i++)
			;
        if (checkMethodes(str.c_str() + i, serv))
		{
			serv.set_is_set( "allowMethods" );
		    return (true);
		}
    }
    return (false);
}

bool	ParseFile::redirectParse( const std::string str_const, Server & serv )
{
	int i = 0;
	std::string	str = str_const;
	if (serv.get_is_set( "redirect" ))
		return (false);
	if ((str.compare(0, 9, "redirect ") == 0 || str.compare(0, 9, "redirect\t") == 0) && checkOccurance(str, ";") == 1 && afterSemiColon(str))
	{
		for (i = 8; str[i] == ' ' || str[i] == '\t'; i++){ /* just skipping space and tab */}
		serv.setRedirectStr( str.substr(i, str.find(";", 0) - i) );
		serv.setRedirect(true);
		serv.set_is_set( "redirect" );
		return (true);
	}
	else
		return (false);
}

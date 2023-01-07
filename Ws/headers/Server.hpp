/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 14:46:05 by amahla            #+#    #+#             */
/*   Updated: 2022/11/10 14:33:11 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__
# include <map>
# include <string>
# include <vector>

//using namespace std;

# include <map>
# include <vector>
# include <netinet/in.h>

class Server
{

	private:

		std::vector< std::string >			_serverName;
		int									_servSock;
		short								_port;
		std::string							_portStr;
		int									_addr;
		in_addr_t							_inetAddr;
		int									_clientBody;
		bool								_autoindex;
		std::string							_root;
		std::vector< std::string >			_index;
		std::map< std::string, std::string>	_error_pages;
		std::map< std::string, bool >		_is_set;
		std::map< std::string, Server >		_location;
		bool								_allowDelete;
		bool								_allowGet;
		bool								_allowPost;
		bool								_redirect;
		std::string							_redirectStr;

	public:

		Server( void );
		Server( const int port );
		Server( const Server & rhs );

		~Server( void );

		Server &	operator=( const Server & rhs );

		std::map< std::string, bool >	& getMap( void );
		bool				get_is_set( std::string attribute );
		void				set_is_set( std::string attribute );

		int	&									getSock( void );
		const int								& getSock( void ) const;
		std::vector< std::string >				& getServerName( void );
		std::map< std::string, std::string >	& get_error_pages(void);
		std::map< std::string, Server >			& getLocation( void );
		const short								& getPort( void ) const;
		short									& getPort( void );
		std::string const 						& getPortStr(void) const;
		const in_addr_t							& getInetAddr( void ) const;
		in_addr_t								& getInetAddr( void );
		int										get_clientBody( void ) const;
		std::string								get_root( void ) const;
		std::string								getAddrStr( void ) const;
		std::vector< std::string >				& get_index( void );
		bool									getAutoindex( void ) const;
		bool									getAllowGet( void );
		bool								    getAllowPost( void );
		bool								    getAllowDelete( void );
		bool									getRedirect( void );
		std::string const &						getRedirectStr( void );

		void									setAutoindex( bool onOff );
		void									set_clientBody( const int val );
		void									set_root( const std::string root );
		void									setSock( const int sock );
		void									setAddr( in_addr_t addr );
		void									setPort( const int port );
		void									setPort( std::string const & port);

		void									setAllowDelete(bool val);
		void									setAllowPost(bool val);
		void									setAllowGet(bool val);
		void									setRedirect( bool redirect );
		void									setRedirectStr( std::string redirectStr );


};

std::ostream	& operator<<( std::ostream & ifs, Server rhs );

#endif

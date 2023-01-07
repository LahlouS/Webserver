/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 14:46:05 by amahla            #+#    #+#             */
/*   Updated: 2022/11/12 18:12:43 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__
# include "Request.hpp"
#include "Server.hpp"

# include "Server.hpp"
# include "Request.hpp"

# include <iostream>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>
# include <sstream>
# include <cstring>

class Response
{

	private:

		int			_fd;
		std::string	_response;
		bool		_isCGI;
		int			_status;
		Server		&_serv;
		Request		&_req;
		Server		_initserv;
		Request		_initreq;
		std::vector<unsigned char>	_buffer;

		char    	**_getArgv(std::string script);
		void    	_clear_argv(char **argv);
		void		_clear_env(char **env);
		int    		_execCGI(std::string script, char **env);
		std::string _getEnv(std::string key, char **env);
		void		_readFile(std::string path);
		void		_readFile(std::string path, Server &serv);
		void		_initVar(std::string *var);
		void		_printErrorPage(void);
		void		_getErrorPage(void);
		char**		_buildCGIenv();
		char**		_ft_split(char const *s, char c);
		bool		_checkFileToDelete(std::string const & script);
		std::string	_getType(std::string str);
		bool		_pathMatchRedirect(Server &serv, Request &req);
		bool		_compareLocation(std::string servRedir, std::string reqPath);
		bool		_isAccept(std::string str);
		void		_getPage(std::string page);
		void		_getPage(std::string page, int _status);
		void		_fillVector(std::string buf);
		bool		_checkScript(std::string str);

	public:
		Response( void );
		Response( Server & serv, Request & req, int fd);
		Response( const Response & rhs );
		~Response( void );
		Response &	operator=( const Response & rhs );

		std::string					& getStringResponse( void );
		bool						getIsCGI(void);
		const std::string			& getStringResponse( void ) const;
		int							& getStatus(void);
		std::vector<unsigned char>	& getBufferVec( void );

		void				GET_response(void);
		void				POST_response(void);
		void				DELETE_response(void);

		void				REDIR_response(std::string const & redirectStr);

		void				fillbuf(unsigned char *buf);

};

char	*ft_find_wrd(char const *s, char set, int wordneeded);
char	*ft_get_wrd(char const *s, int e);
int		ft_count_wrd(char const *s, char c);
int		ft_o(char c1, char c2);

#endif

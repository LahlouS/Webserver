/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseFile.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slahlou <slahlou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:38:07 by amahla            #+#    #+#             */
/*   Updated: 2022/11/09 17:23:42 by slahlou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEFILE_HPP__
# define __PARSEFILE_HPP__

# include "webserv.h"
# include <string>

class ParseFile
{

	private:

		std::vector<Server>	_servers;
		bool				(ParseFile::*_ft[9])( const std::string str,
								Server & server );

	public:

		ParseFile( const char *argv );
		ParseFile( const ParseFile & rhs );

		~ParseFile( void );

		ParseFile &	operator=( const ParseFile & rhs );

		const std::vector<Server>	& getServers( void ) const;
		std::vector<Server>	& getServers( void );

		void	readContent( std::ifstream & ifs, std::string temp,
		const std::string file, Server *parent );
		void	FormatFile( std::ifstream & ifs, std::string temp );
		void	readFile( const char *file );

		bool	setServerName( const std::string str, Server & server );
		bool	setLocation( std::ifstream & ifs, std::string temp, Server & server );
		bool	error_page(const std::string line_const, Server &server);
		bool	client_body( const std::string line_const, Server &server );
		bool	root( const std::string line_const, Server &server );
		bool	index(const std::string line_const, Server &server);
		bool	listenParse( const std::string str_const, Server & serv);
		bool	autoindexParse( const std::string str_const, Server & serv );
		bool	allowedMethodsParse(std::string str, Server & serv);
		bool	redirectParse(std::string str_const, Server & serv);

};

			/*	ParseFileUtils.cpp	*/
//			max code
bool		is_white_space(char val);
int			suppr_white_space(std::string &line);
bool		check_end_line(std::string &line);
std::string	parse(const std::string &line, std::string comp);
bool		get_the_info_i_need(std::string &line,
				std::string atribut, std::string &rslt);
size_t		get_nb_of_file(std::string line);
bool		get_key_map_error(std::string &rslt,
				std::map<std::string, std::string> &error_map);
bool		get_value_map_error(std::string &rslt,
				std::map<std::string, std::string> &error_map);

//			sacha code
int		checkOccurance(std::string & str, const char * toFind);
bool	checkSyntax( std::string str );
bool	checkSyntaxIndex( std::string str );
bool	onlyDiggit(std::string const & str);
bool	addrIsValid(std::string	addr);
bool	addrIsGood(Server & serv, std::string addr);
bool	portIsGood(Server & serv, std::string port);
bool    afterSemiColon(std::string & str);
bool    checkMethodes(std::string str, Server & serv);

#endif

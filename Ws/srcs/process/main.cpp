/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 21:10:19 by amahla            #+#    #+#             */
/*   Updated: 2022/11/12 14:11:07 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.h"
# include <iostream>
# include <cstdlib>
# include "WebServException.hpp"

int main( int ac, char **av )
{
	if ( ac > 2 )
	{
		std::cout << "Error: needed zero ( server set by default ) or just one ( config server file ) argument" << std::endl;
		return ( EXIT_FAILURE );
	}
	try
	{
		if ( ac == 2 )
			webServ( av[1] );
		else
			webServ( "./config_file/config_default" );
	}
	catch ( std::exception & e )
	{
		std::cout << RED << e.what() << std::endl << SET;
		return ( EXIT_FAILURE );
	}
	return ( EXIT_SUCCESS );
}

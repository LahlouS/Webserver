/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meudier <meudier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 12:45:35 by amahla            #+#    #+#             */
/*   Updated: 2022/11/12 14:06:27 by meudier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.h"

void	webServ( const char *av )
{
	ParseFile	file( av );
	std::vector<Server> servers = file.getServers();
	servers = file.getServers();
	setServerSockets( file.getServers() );
	appServ( file.getServers() );
}

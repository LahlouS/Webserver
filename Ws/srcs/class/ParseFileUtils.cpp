# include "ParseFile.hpp"
# include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

/*					Code Max		*/

bool	is_white_space(char val)
{
	char delimiter[] = { ' ', '\t' };
	size_t i = 0;
	while (i < (sizeof(delimiter) / sizeof(char)))
	{
		if (val == delimiter[i])
			return (true);
		i++;
	}
	return (false);
}

int		suppr_white_space(std::string &line)
{
	int pos = 0;
	int size = line.size();

	while ((pos < size) && is_white_space(line[0]))
	{
		line.erase(0, 1);
		pos++;
	}
	return (pos);
}

bool	check_end_line(std::string &line)
{
	for (std::string::iterator ite = line.end() - 1; ite != line.begin(); ite--)
	{
		if (!is_white_space(*ite))
		{
			if (*ite == ';')
			{
				line.erase(ite, line.end());
				return (true);
			}
			return (false);
		}
	}
	return (false);
}

std::string	parse(const std::string &line, std::string comp)
{
	std::string cpy = line;
	size_t  found;

	found = cpy.find(comp);
	if (found)
		return ("error");
	cpy.erase(0, comp.size());

	suppr_white_space(cpy);

	if (cpy[0] == ':' || cpy[0] == '=')
		cpy.erase(0, 1);

	suppr_white_space(cpy);

	return (cpy);
}

bool	get_the_info_i_need(std::string &line, std::string atribut, std::string &rslt)
{
	if (!check_end_line(line))
		return (false);
	rslt = parse(line, atribut);
	if (rslt == "error")
		return (false);
	return (true);
}

size_t	get_nb_of_file(std::string line)
{
	size_t count = 0;
	size_t i = 0;
	while (i < line.size())
	{
		if (!is_white_space(line[i]))
		{
			count++;
			while (i < line.size() && !is_white_space(line[i]))
				i++;
		}
		else
			i++;
	}
	return (count);
}

bool	get_key_map_error(std::string &rslt, std::map<std::string, std::string> &error_map)
{
	std::string  temp;
	std::pair<std::string, std::string>    tmp;
	int i = 0;
	int len;

	while (rslt[i] && (rslt[i] != '/'))
	{
		len = 0;
		while ( std::isdigit(rslt[++len]) );
		if (len % 3)
			return (false);

		temp.insert(temp.begin(), rslt.begin(), rslt.begin() + 3);
		tmp = std::pair<std::string, std::string>(temp, "");
		error_map.insert(tmp);
		temp.clear();
		rslt.erase(0, 3);
		suppr_white_space(rslt);
	}
	return (true);
}

bool	get_value_map_error(std::string &rslt, std::map<std::string, std::string> &error_map)
{
	std::map<std::string, std::string>::iterator it;
	std::string temp;
	size_t i;
	size_t nb_of_file;
	if (rslt[0] == '/')
		rslt.erase(0, 1);
	else
		return (false);

	suppr_white_space(rslt);

	it = error_map.begin();
	temp = rslt;
	nb_of_file = get_nb_of_file(rslt);
	if (nb_of_file > 1)
	{
		if (error_map.size() != nb_of_file )
			return (false);
		while (it != error_map.end())
		{
			i = 0;
			while(i < rslt.size() && !is_white_space(rslt[i]))
				i++;
			it->second.insert(it->second.begin(), rslt.begin(), rslt.begin() + i);
			rslt.erase(0, i);
			suppr_white_space(rslt);
			it++;
		}
	}
	else
	{
		while (it != error_map.end())
		{
			i = 3;
			rslt = temp;
			while (i--)
			{
				if (rslt[i] == 'x')
					rslt[i] = it->first[i];
			}
			it->second = rslt;
			it++;
		}
	}
	return (true);
}

/*				code Sacha				*/

int	checkOccurance(std::string & str, const char * toFind)
{
	int	i = 0;
	size_t occurs = 0;

	while (((occurs = str.find(toFind, occurs))) != std::string::npos){
		occurs++;
		i++;
	}
	return (i);
}

bool	checkSyntax(std::string str)
{
	int i = 0;
	if (checkOccurance(str, ";") != 1 || checkOccurance(str, ":") > 1 || checkOccurance(str, ".") > 3)
		return (false);
	while (str[i] != ';')
		i++;
	i++;
	while (str[i]){
		if (str[i] != ' ' && str[i] != '\t')
			return (false);
		i++;
	}
	return (true);
}

bool	onlyDiggit(std::string const & str)
{
	int i = 0;

	while (str[i]){
		if (str[i] > '9' || str[i] < '0'){
			return (false);
		}
		i++;
	}
	return (true);
}

bool	addrIsValid(std::string	addr)
{
	size_t	occurence = 0;
	int	pos = 0;

	while ( ( occurence = addr.find(".", occurence) ) != std::string::npos){
		if (!onlyDiggit(addr.substr(pos, occurence - pos)) || addr.substr(pos, occurence - pos).size() == 0 )
			return (false);
		if (addr.substr(pos, occurence - pos).size() > 1 && (addr.substr(pos, occurence - pos))[0] == '0')
			return (false);
		occurence++;
		pos = occurence;
	}
	if (!onlyDiggit(addr.substr(pos, occurence - pos)) || addr.substr(pos, occurence - pos).size() == 0)
	{
		return (false);
	}
	return (true);
}

bool	addrIsGood(Server & serv, std::string addr)
{
	int		test = 0;
	int		res = 0;
	size_t	i = 0;

	if (!addr.compare("localhost") || !addr.compare("*")){
		serv.setAddr(inet_addr("127.0.0.1"));
		return (true);
	} else if (checkOccurance(addr, ".") == 3 && addrIsValid(addr)) {
		while (i < addr.size()){
			test = atoi(addr.c_str() + i);
			if (test < 0 || test > 255)
				return (false);
			res |= test;
			while (addr[i] && addr[i] != '.')
				i++;
			if (addr[i] == '.')
				res <<= 8;
			i++;
		}

		serv.setAddr(inet_addr(addr.c_str()));
		return (true);
	} else {
		return (false);
	}
}

bool	portIsGood(Server & serv, std::string port)
{
	int test = 0;
	if (!port.compare("default") || !port.compare("*")){
		serv.setPort(8080);
		return (true);
	}
	if (port[0] == '0')
			return (false);
	if (!onlyDiggit(port) || (test = atoi(port.c_str())) > MAX_PORT || (test = atoi(port.c_str())) < MIN_PORT)
		return (false);
	serv.setPort(test);
	serv.setPort(port);
	return (true);
}

bool	checkSyntaxIndex(std::string str)
{
	int i = 0;

	if (checkOccurance(str, ";") != 1)
		return (false);
	while (str[i] != ';')
		i++;
	i++;
	while (str[i]){
		if (str[i] != ' ' && str[i] != '\t')
			return (false);
		i++;
	}
	return (true);
}


bool    afterSemiColon(std::string & str)
{
    for (std::string::reverse_iterator it = str.rbegin(); *it != ';' && it != str.rend(); it++ )
	{
        if (*it != '\t' && *it != ' ')
            return (false);
    }
    return (true);
}

bool    checkMethodes(std::string str, Server & serv)
{
    if ((str.substr(0, 3)).find("GET", 0) != std::string::npos )
	{
        if (str[3] == ':') {
            serv.setAllowGet(true);
            return (checkMethodes(str.c_str() + 4, serv));
        }
        if (str[3] == ';'){
            serv.setAllowGet(true);
            return (true);
        }
    }
    else if ((str.substr(0, 4)).find("POST", 0) != std::string::npos )
	{      
        if (str[4] == ':'){
            serv.setAllowPost(true);
            return (checkMethodes(str.c_str() + 5, serv));
        }
        if (str[4] == ';') {
            serv.setAllowPost(true);
            return (true);
        }
    }
    else if ((str.substr(0, 6)).find("DELETE", 0) != std::string::npos )
	{
        if (str[6] == ':'){
            serv.setAllowDelete(true);
            return (checkMethodes(str.c_str() + 7, serv));
        }
        if (str[6] == ';'){
            serv.setAllowDelete(true);
            return (true);
        }
    }
    return (false);
}

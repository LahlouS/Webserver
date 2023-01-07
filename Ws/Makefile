# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: meudier <meudier@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/17 21:07:29 by amahla            #+#    #+#              #
#    Updated: 2022/11/12 13:32:28 by amahla           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL				=	/bin/sh

PROGNAME			:=	webserv

SRCDIR				:=	srcs

INCLUDEDIR			:= headers

CGIDIR				:=	cgi
OBJDIR				:=	./obj
DEBUGDIR			:=	./debugobj

SRCSLINUXSELECT		:=	$(addprefix	select/,	serv_process_select.cpp	)

SRCSLINUXEPOLL		:=	$(addprefix	process/,	serv_process_epoll.cpp			\
												serv_process_epoll_loop.cpp		\
												serv_process_epoll_utils.cpp)

SRCSSHARE			:=	$(addprefix process/,	main.cpp					\
												webserv.cpp					\
												socket_settings.cpp		)	\
						$(addprefix class/,		WebServException.cpp		\
												ParseFile.cpp				\
												ParseFileUtils.cpp			\
												Server.cpp					\
												Client.cpp					\
												Request.cpp					\
												Response.cpp			)

SRCS				:=	$(SRCSSHARE) 							\
						$(SRCSLINUXEPOLL)

SRCSCGI1			:=	$(addprefix	cgi/,	autoindex.cpp)

SRCSCGI2			:=	$(addprefix	cgi/,	signIn.cpp)

CC					:=	c++
RM					:=	rm

CCFLAGS				:=  -std=c++98 -Wall -Wextra -Werror
OPTFLAG				:=

NAMECGI1			:=	$(addprefix	cgi/,	autoindex.cgi)

NAMECGI2			:=	$(addprefix	cgi/,	signIn.cgi)

NAME				:=	$(PROGNAME)

OUTDIR				:=	$(OBJDIR)

DEBUGNAME			:=	$(addsuffix .debug,$(PROGNAME))

ifdef DEBUG
	OPTFLAG 		:=	-g
	NAME			:=	$(DEBUGNAME)
	OUTDIR			:=	$(DEBUGDIR)
endif

ifdef SELECT
	SRCS				:=	$(SRCSSHARE) 							\
							$(addprefix process/,	$(SRCSLINUXSELECT))
endif

all					:	$(NAME) $(NAMECGI1) $(NAMECGI2)

debug				:
ifndef DEBUG
	$(MAKE) DEBUG=1
endif

select				:
ifndef SELECT
	$(MAKE) SELECT=1
endif

$(OUTDIR)/%.o		:	$(SRCDIR)/%.cpp | $(OUTDIR)
	@mkdir -p $(dir $@)
	$(CC) -c -MMD -MP $(CCFLAGS) $(OPTFLAG) $(addprefix -I ,$(INCLUDEDIR)) $< -o $@

$(NAME)				:	$(addprefix $(OUTDIR)/,$(SRCS:.cpp=.o))
	$(CC) $(CCFLAGS) $(OPTFLAG) -o $@ $^
	@mkdir -p $(dir $(CGIDIR))

$(NAMECGI1)			:	$(addprefix $(OUTDIR)/,$(SRCSCGI1:.cpp=.o))
	@mkdir -p $(CGIDIR)
	$(CC) $(CCFLAGS) $(OPTFLAG) -o $@ $<

$(NAMECGI2)			:	$(addprefix $(OUTDIR)/,$(SRCSCGI2:.cpp=.o))
	@mkdir -p $(CGIDIR)
	$(CC) $(CCFLAGS) $(OPTFLAG) -o $@ $<


$(OUTDIR)			:
	mkdir $(OUTDIR)

clean				:
	$(RM) -rf $(OBJDIR) $(DEBUGDIR)

fclean				:	clean
	$(RM) -rf $(PROGNAME) $(addsuffix .mac,$(PROGNAME)) $(DEBUGNAME) $(CGIDIR)

re					:	fclean
	$(MAKE) $(NAME) $(NAMECGI1) $(NAMECGI2)

.PHONY				:	all clean fclean re debug mac select

-include	$(addprefix $(OUTDIR)/,$(SRCS:.cpp=.d))
-include	$(addprefix $(OUTDIR)/,$(SRCSCGI1:.cpp=.d))
-include	$(addprefix $(OUTDIR)/,$(SRCSCGI2:.cpp=.d))

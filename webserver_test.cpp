#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <iostream>

int main(){
	int chaussette = socket(AF_INET, SOCK_STREAM, 0);
	int flags;
	/*
	 * on declare une socket qu'on initialise avec la fonction socket, qui renvoir le fd surlequel nous alons echanger les donnes
	 * AF_INET specifie la famille de nom utilise, en l'occurence c'est la famille ipv4
	 * SOCK_STREAM specifie le type de communication, ici SOCK_STREAM a des specificite qui font aue notre communication sera TCP compliant
	 * le dernier argument est le protocole utilise, il decoule du type, seulement SOCK_STREAM sous entend TCP donc
	*/
	if ((flags = fcntl(chaussette, F_GETFL)) < 0)
	{
		std::cerr << "could not get file flags" << std::endl;
		exit(1);
	}
	/*
	 ici on lit dans un premier temp les attribut du fd de notre socket, la fonction fcntl() a laquelle on passe un FD ainsi
	 que le flag F_GETFL permet de renvoyer les attribut de notre FD.
	 */
	if (fcntl(chaussette, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		std::cerr << "could not set file flags" << std::endl;
		exit(1);
	}
	/*
	 * on utilise ensuite la fonction fcntl a nouveau afin de rajouter le O_NONBLOCK a nos parametres de base
	 * d'ou l'utilisation du ou binaire afin de ne pas perdre nos attribut de base
	 * O_NONBLOCK indique au ce fd ne doit pas bloquer sur les appels recv, send, accept... lorsqu'une tache n'est pas encore
	 * possible (ce qu'il se passe avec les pipe par exemple)
	*/

	struct sockaddr_in chaussette_params = {0, 0, 0, 0};
	if (chaussette < 0){
		perror("invalid chaussette");
		exit(1);
	}
	chaussette_params.sin_addr.s_addr = htonl(INADDR_ANY);
	chaussette_params.sin_port = htons(8080);
	chaussette_params.sin_family = AF_INET;
	/*
	 * On initialise notre structure sockaddr_in avec une adresse ip sur laquel elle va ecouter, dans notre cas la macro
	 * INADDR_ANY est le code specifiant que tout adresse expeditrice sera accepte
	 * on lui donne aussi le port sur lequel elle va ecouter ici 1080
	 * on utilise les fonction htonl et s (host to network) afin de s'assurer que nos addresses et ports sont coder selon le endian du network et ainsi bien interprete
	 * on precise dernierement la famille d'adresse de communication ici AF_INET pour ipv4
	 * */

	if (bind(chaussette, (struct sockaddr *) &chaussette_params, sizeof(chaussette_params))){
		perror("chaussette fail to bind");
		exit(1);
	}
	/*
	 * On vient ensuite adresser notre fd (chaussette) en la "bindant" avec notre structure sockaddr_in
	 */
	if (listen(chaussette, 10) < 0){
		perror("listenning failed for the chaussette");
		exit(1);
	}
	/*
	 * Pour finir l'initialisation de notre interface, la fonction listen met notre sockette en attente passive
	 * c-a-d une socket qui attendra les requete de connexion entrante et qui utilisera la fonction accept pour accepter ces demande
	 * elle prend notre socket en argument ainsi qu'une limite de file d'attente des clients pour la connexion sur notre socket
	 */
	fd_set readfilesock;
	int clientChaussette = -1;
	struct sockaddr_in client_addr = {0, 0, 0, 0};
	socklen_t csize = sizeof(client_addr);
	std::cout << "SERVER IS RUNNING >\n";
	const char *buf2 = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	const char *buf3 = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nAAAAAAAAAAAA";
	/*
	 * Ici je declare un fd_set, qui est un champ de bit dans lequel je vais pouvoir stocker mes fd de sockets
	 * Aussi, comme j esais que j ene vais avoir qu'un seul client je prepare un fd ainsi qu'une structure sockaddr_in afin de generer ma socket client
	 * buf2 est un buffer contenant une requete http afin de tester la connexion avec mon client
	 */
	while (1){

		FD_ZERO(&readfilesock);
		FD_SET(chaussette, &readfilesock);
		/*
		 * je commence par set mon fd_set a 0, puis je lui injecte ma socket server
		 * il faut comprendre que les MACRO FD_SET FD_ZERO FD_ISSET FD_CLEAR... fonctionnent en dependance avec
		 * la fonction select et le type fd_set etant donne qu'elle bit wise les fd
		 * */
		if (clientChaussette != -1){
			FD_SET(clientChaussette, &readfilesock); // si j'ai deja un client alors je l'inject aussi dans on fd_set
		}
		if (select(100, &readfilesock, NULL, NULL, NULL) == -1){
			perror("chaussette select failed");
			exit(errno);
		}
		/*
		 * c'est la que la fonction select() intervient, elle va parcourir notre fd_set (en mode read et en mode write)
		 * et attendra un temp donne par la structure timeout donne en dernier argument (ici a NULL)
		 * elle modifira nos fd_set afin de ne laisser dedans seulement les fd qui sont accessible pour le
		 * service au'on lui demande.
		 * A chaque tour de boucle, il faudra donc reInjecter notre socket server et nos socket client
		 * afin que select puisse anouveau faire son traville
		 */
		if (FD_ISSET(chaussette, &readfilesock)){ //we have a new client !!, en effet, si la socket server est encore presente dans le fd_set, c'est qu'il y a une nouvlle demande de connexion
			clientChaussette = accept(chaussette, (struct sockaddr *) &client_addr, &csize);
			if (clientChaussette < 0){
				perror("CLIENTCHAUSSETTE");
			}
				int n = 0;
				char buf[4096] = {0};

				if ((n = recv(clientChaussette, buf, 4095, 0))< 0){
					perror("recv from enw client");
					break ;
				}
				buf[n] = 0;
				std::cout << "Contenu du BUFFER --->" << buf << "\n";

				if(send(clientChaussette, buf2, strlen(buf2), 0) < 0)
 				{
					perror("send()");
					exit(errno);
				}
				/*
				 * on a detecter un nouveaux client grace a FD_ISSET, on peut maintenant generer une
				 * socket client (clientChaussette = accept(...))
				 * c'est avec cette socket que l'on va pouvoircommuniquer avec notre client tout le long de la connexion
				 * il faut donc aussi linjecter dans notre fd_set et c'est cette socket qui sera garde par le fonction select lorsque
				 * le client enverra une nouvelle requete
				 */
		}
		else if (FD_ISSET(clientChaussette, &readfilesock)){
		std::cout << "test----------------\n";
			if(send(clientChaussette, buf3, strlen(buf3), 0) < 0)
 			{
				perror("send()");
				exit(errno);
			}
		}
	}
	FD_CLR(chaussette, &readfilesock);
	close(clientChaussette);
	close(chaussette);


	return (0);
}


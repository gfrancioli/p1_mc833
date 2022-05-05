#include "params.h"
#define SA struct sockaddr

void print_menu(){
	printf("\n/===============================/\n\n");
	printf("1 - Cadastro\n");
	printf("2 - Listar todos os filmes\n");
	printf("3 - Listar com opcoes\n");
	printf("4 - Remover um filme\n");
	printf("5 - Adicionar um genero a um filme\n");
	printf("\n/===============================/\n\n");
}

void func(int sockfd)
{
	char buff[MAX], opt[MAX];
	for (;;) {
		//get entry in menu
		bzero(buff, sizeof(buff));
		print_menu();
		fgets(buff, MAX,stdin);
        strtok(buff, "\n");
		write(sockfd, buff, sizeof(buff));

		//add new movie
        if (!strcmp(CADASTRO, buff)) {
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            while(strcmp("end", buff)){
				//until the end we add all the informations about the movie
                printf("%s", buff);
                bzero(buff, sizeof(buff));
                fgets(buff, MAX,stdin);
                strtok(buff, "\n");
				
				//compare if has input
                if (strlen(buff) > 0)
                    write(sockfd, buff, sizeof(buff));
                bzero(buff, sizeof(buff));
                read(sockfd, buff, sizeof(buff));
            }
        }

		//list all
        if(!strcmp(LISTA_TODOS, buff)){
            //get return
			bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
			printf("\n\nResultados: \n");
			printf("%s\n", buff);
			bzero(buff, sizeof(buff));
        }

		//list with options
		if(!strcmp(LISTA_OPT, buff)){
			//get and send the option
			printf("1 - Somente os Titulos\n2 - Por Genero\n3 - Por ID\n");
			fgets(opt, MAX,stdin);
        	strtok(opt, "\n");
			write(sockfd, opt, sizeof(opt));

			//check if iption is 2 or 3(type or id)
			if(!strcmp("2", opt) || !strcmp("3", opt)){
				bzero(buff, sizeof(buff));
				read(sockfd, buff, sizeof(buff));
				printf("%s\n", buff);
				bzero(opt, sizeof(opt));
				fgets(opt, MAX,stdin);
				strtok(opt, "\n");
				write(sockfd, opt, sizeof(opt));
			}
			//get return
            bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));
			printf("\n\nResultados: \n");
			printf("%s\n", buff);
			bzero(buff, sizeof(buff));
        }
		
		//delete movie
		if(!strcmp(DELETE, buff)){
			printf("Qual ID deseja deletar?\n");
			fgets(opt, MAX,stdin);
        	strtok(opt, "\n");
			write(sockfd, opt, sizeof(opt));

			//get return
            bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));
			printf("%s\n", buff);
			bzero(buff, sizeof(buff));
			
		}

		//add type
		if(!strcmp(GENERO, buff)){
			printf("Qual ID deseja adicionar um genero?\n");
			fgets(opt, MAX,stdin);
        	strtok(opt, "\n");
			write(sockfd, opt, sizeof(opt));

			bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));
			printf("%s\n", buff);
			
			//add specific type
			bzero(opt, sizeof(opt));
			fgets(opt, MAX,stdin);
        	strtok(opt, "\n");
			write(sockfd, opt, sizeof(opt));

			//get return
            bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));
			printf("%s\n", buff);
			bzero(buff, sizeof(buff));
			
		}
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Cliente saiu.\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Falha na criacao do socket\n");
		exit(0);
	}
	else
		printf("Socket criado com sucesso!\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Falha na conexão com o server\n");
		exit(0);
	}
	else
		printf("Conectado ao server\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}

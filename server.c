#include "params.h"
#include <pthread.h>
#define SA struct sockaddr

pthread_mutex_t lock;
const char MOVIE_FILE[] = "movies.txt";
void cadastra(int sock, int flag);
void lista_todos(int sockfd);
void lista_opt(int sockfd, char* buff);
void remove_filme(int sockfd, char* buff);
void adiciona_genero(int sockfd, char* buff);

void cadastra(int sock, int flag) {
    FILE *movie_file;
    char buff[MAX], id[MAX];
    movie new_movie;

	// communication to all inputs
	sprintf(id, "%d", flag);
	strcpy(new_movie.id, id);
	strcpy(new_movie.deleted, "0");

    write(sock, "Titulo: ", strlen("Titulo: "));
    read(sock, buff, sizeof(buff));
    strcpy(new_movie.titulo, buff);
    bzero(buff, sizeof(buff));

    write(sock, "Genero: ", strlen("Genero: "));
    read(sock, buff, sizeof(buff));
    strcpy(new_movie.genero, buff);
    bzero(buff, sizeof(buff));

    write(sock, "Diretor: ", strlen("Diretor: "));
    read(sock, buff, sizeof(buff));
    strcpy(new_movie.diretor, buff);
    bzero(buff, sizeof(buff));

    write(sock, "Ano Lancamento: ", strlen("Ano Lancamento: "));
    read(sock, buff, sizeof(buff));
    strcpy(new_movie.ano_lanc, buff);
    bzero(buff, sizeof(buff));

	//save new file
    write(sock, "end", strlen("end"));
    movie_file = fopen(MOVIE_FILE, "a");
    fwrite(&new_movie, sizeof(new_movie), 1, movie_file);
    fclose(movie_file);

}

void lista_todos(int sockfd) {
	FILE *ptr;
    movie read_movie;
	char* lista[MAX*10];
	ptr = fopen(MOVIE_FILE, "r");
	if (!ptr){
		write(sockfd, "Erro ao abrir arquivo", strlen("Erro ao abrir arquivo"));
		return;
	} 
	// concat the output
    while (fread(&read_movie, sizeof(movie), 1, ptr)){
		strcat(lista,"Id -> ");
		strcat(lista, read_movie.id);
		strcat(lista," | Titulo -> ");
		strcat(lista, read_movie.titulo);
		strcat(lista," | Genero -> ");
		strcat(lista, read_movie.genero);
		strcat(lista," | Diretor -> ");
		strcat(lista, read_movie.diretor);
		strcat(lista," | Ano Lancamento -> ");
		strcat(lista, read_movie.ano_lanc);
		strcat(lista,"\n");
	}
	if (strlen(lista) > 0){
		write(sockfd, lista, strlen(lista));
		memset(lista, 0, sizeof(lista));
		bzero(lista, sizeof(lista));
	}
	else
		write(sockfd, "Nao existe filmes na lista", strlen("Nao existe filmes na lista"));
	fclose(ptr);
}

void lista_opt(int sockfd, char* buff) {
	FILE *ptr;
    movie read_movie;
	char* lista[MAX*10], *temp[MAX*2];
	ptr = fopen(MOVIE_FILE, "r");

	//by title
	if(!strcmp("1", buff)){
		while (fread(&read_movie, sizeof(movie), 1, ptr)){
			strcat(lista,"Id -> ");
			strcat(lista, read_movie.id);
			strcat(lista," | Titulo -> ");
			strcat(lista, read_movie.titulo);
			strcat(lista,"\n");
		}
	}
	//by type
	if(!strcmp("2", buff)){
		write(sockfd, "Qual genero? ", strlen("Qual genero? "));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));

		while (fread(&read_movie, sizeof(movie), 1, ptr)){
			strcpy(temp, read_movie.genero);
			char * token = strtok(temp, ",");
			
			while (token != NULL){
				if(!strcmp(token, buff)){
					strcat(lista,"Titulo -> ");
					strcat(lista, read_movie.titulo);
					strcat(lista," | Genero -> ");
					strcat(lista, read_movie.genero);
					strcat(lista," | Diretor -> ");
					strcat(lista, read_movie.diretor);
					strcat(lista,"\n");
					break;
				}
				token = strtok(NULL, ",");
			}
			//free(token);
		}
	}
	//by id
	if(!strcmp("3", buff)){
		write(sockfd, "Qual ID? ", strlen("Qual ID? "));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		while (fread(&read_movie, sizeof(movie), 1, ptr)){
			if (!strcmp(read_movie.id, buff)){
				strcat(lista,"Titulo -> ");
				strcat(lista, read_movie.titulo);
				strcat(lista," | Genero -> ");
				strcat(lista, read_movie.genero);
				strcat(lista," | Diretor -> ");
				strcat(lista, read_movie.diretor);
				strcat(lista," | Ano Lancamento -> ");
				strcat(lista, read_movie.ano_lanc);
				strcat(lista,"\n");
			}
		}
	}
	write(sockfd, lista, strlen(lista));
	memset(lista, 0, sizeof(lista));
	bzero(lista, sizeof(lista));
	fclose(ptr);
}

void remove_filme(int sockfd, char* buff){
	FILE *ptr, *temp;
    movie read_movie;
	int flag = 0;
	ptr = fopen(MOVIE_FILE, "r");
	temp = fopen("temp.txt", "w");

	//find movie and copy the others to temp file
	while (fread(&read_movie, sizeof(movie), 1, ptr)){
		if (!strcmp(read_movie.id, buff)){
			flag++;
		}
		else {
			fwrite(&read_movie, sizeof(read_movie), 1, temp);
		}
	}

	//remove older file and rename temp file
	fclose(ptr);
	fclose(temp);
	remove(MOVIE_FILE);
	rename("temp.txt", MOVIE_FILE);

	if (flag){
		write(sockfd, "Filme deletado", strlen("Filme deletado!"));
		flag = 0;
	}
	else
		write(sockfd, "Nenhum filme encontrado com esse id", strlen("Nenhum filme encontrado com esse id"));
}

void adiciona_genero(int sockfd, char* buff){
	FILE *ptr, *temp;
    movie read_movie;
	int flag = 0;
	ptr = fopen(MOVIE_FILE, "r");
	temp = fopen("temp.txt", "w");

	//find movie, add type and copy others to temp file
	while (fread(&read_movie, sizeof(movie), 1, ptr)){
		if (!strcmp(read_movie.id, buff)){
			//wait response from client
			write(sockfd, "Qual genero deseja adicionar?", strlen("Qual genero deseja adicionar?"));
			bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));

			strcat(read_movie.genero, ",");
			strcat(read_movie.genero, buff);
			fwrite(&read_movie, sizeof(read_movie), 1, temp);
			flag++;
		}
		else
			fwrite(&read_movie, sizeof(read_movie), 1, temp);
	}

	//remove older file and rename temp file
	fclose(ptr);
	fclose(temp);
	remove(MOVIE_FILE);
	rename("temp.txt", MOVIE_FILE);
	if (flag){
		write(sockfd, "Genero aplicado!", strlen("Genero aplicado!"));
		flag = 0;
	}
	else
		write(sockfd, "Nenhum filme encontrado com esse id", strlen("Nenhum filme encontrado com esse id"));

}

// control the chat
void *controller(void *connfd)
{
	char buff[MAX];
    int sock = *(int*)connfd;
	int flag_id = 1;

	for (;;) {
		// read the message from client
		memset(buff, 0, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sock, buff, sizeof(buff));

		//filter command
        if (!strcmp(CADASTRO, buff)) {
            cadastra(sock, flag_id);
			flag_id++;
        }
        if (!strcmp(LISTA_TODOS, buff)){
            lista_todos(sock);
        }
		if (!strcmp(LISTA_OPT, buff)){
			bzero(buff, sizeof(buff));
			read(sock, buff, sizeof(buff));
            lista_opt(sock, buff);
        }
		if (!strcmp(DELETE, buff)){
			bzero(buff, sizeof(buff));
			read(sock, buff, sizeof(buff));
            remove_filme(sock, buff);
        }
		if (!strcmp(GENERO, buff)){
			bzero(buff, sizeof(buff));
			read(sock, buff, sizeof(buff));
            adiciona_genero(sock, buff);
        }

		// if msg contains "Exit" then server exit
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server saiu.\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;
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
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// binding new socket
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Falha na conexão com o IP!\n");
		exit(0);
	}
	else
		printf("Conexão com o ip realizada.\n");

	// server listen
	if ((listen(sockfd, 5)) != 0) {
		printf("Erro ao aguardar comunicacao.\n");
		exit(0);
	}
	else
		printf("Aguardando comunicacao...\n");

	if (pthread_mutex_init(&lock, NULL) != 0) {
        return 1;
    }
	len = sizeof(cli);

	while (1) {
        connfd = accept(sockfd, (SA*)&cli, &len);
        
        // verificacao da conexao de um novo cliente
        pthread_t thread_id;
        if (connfd < 0) {
			printf("Erro ao conectar novo cliente\n");
		 } else {
			printf("Novo cliente conectado\n");
            pthread_create(&thread_id, NULL, controller, (void*) &connfd);
        }
    }

    pthread_mutex_destroy(&lock);

	return 0;
}

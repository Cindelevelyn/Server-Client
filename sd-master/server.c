// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

// variaveis do semaforo

sem_t x, y;
pthread_t threads;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

// função para ler
void *reader(void *param)
{
    // Lock the semaphore
    sem_wait(&x);
    readercount++;

    if (readercount == 1)
        sem_wait(&y);

    // Unlock the semaphore
    sem_post(&x);

    printf("\n%d reader is inside",
           readercount);

    sleep(5);

    // Lock the semaphore
    sem_wait(&x);
    readercount--;

    if (readercount == 0)
    {
        sem_post(&y);
    }

    // Lock the semaphore
    sem_post(&x);

    printf("\n%d Reader is leaving",
           readercount + 1);
    pthread_exit(NULL);
}

// função para escrever
void *writer(void *param)
{
    printf("\nWriter is trying to enter");

    // Lock the semaphore
    sem_wait(&y);

    printf("\nWriter has entered");

    // Unlock the semaphore
    sem_post(&y);

    printf("\nWriter is leaving");
    pthread_exit(NULL);
}

typedef struct cont
{
    char nome[100];
    char endereco[200];
    int idade;
} Contato;

int getFileSize();
void insertContato(Contato contato, FILE *fp);

int main(int argc, char const *argv[])
{
    Contato contato;
    FILE *fp;

    int server_fd, new_socket, valread, readOp, op, sz;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    // char buffer[1024] = { 0 };
    char hello[1024];
    strcpy(hello, "Hello from server");

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[100]; // array para as threads
    int i=0;

    /*ARQUIVO*/
    fp = fopen("dados.dat", "rb+");

    if (fp == NULL)
    {
        fp = fopen("dados.dat", "wb+");
        if (fp == NULL)
        {
            printf("\nErro na criacao do arquivo!");
            return 1;
        }
    }

    /*-------------------------------*/

    /*MENU*/
    do
    {
        readOp = read(new_socket, &op, sizeof(op));

        if (op != 4)
        {
            switch (op)
            {
            case 1:
                pthread_create(&readerthreads[i++], NULL,reader, &new_socket);
                valread = read(new_socket, &contato, sizeof(contato));
                sz = getFileSize(fp);
                if (sz == 0)
                {
                    fwrite(&contato, sizeof(contato), 1, fp);
                    fflush(fp);
                }
                else
                {
                    insertContato(contato, fp);
                }
                break;
            case 2:
                pthread_create(&readerthreads[i++], NULL,reader, &new_socket);
                send(new_socket, &fp, sizeof(fp), 0);
                break;
            case 3:
                pthread_create(&readerthreads[i++], NULL,reader, &new_socket);
                send(new_socket, &fp, sizeof(fp), 0);
                break;
            case 4:
                break;
            default:
                printf("\nOpcao invalida!!!");
            }
        }
    } while (op != 4);

    close(new_socket);

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int getFileSize(FILE *fp)
{
    int sz;
    rewind(fp);
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    return sz;
}

void insertContato(Contato c, FILE *fp)
{
    Contato cAux;
    int achei = 0, nR = 0;
    rewind(fp);

    // se arquivo vazio
    if ((nR = fread(&cAux, sizeof(Contato), 1, fp)) == 0)
    {
        fwrite(&c, sizeof(Contato), 1, fp);
        fflush(fp);
        return;
    }
    rewind(fp);

    // procurando a posicao certa
    do
    {
        nR = fread(&cAux, sizeof(Contato), 1, fp);
        if (nR == 1)
        {
            if (strcmp(cAux.nome, c.nome) > 0)
            {
                achei = 1;
            }
        }
    } while (achei == 0 && nR == 1);

    // se o que estou guardando eh o ultimo nome
    if (achei == 0)
    {
        fwrite(&c, sizeof(Contato), 1, fp);
        fflush(fp);
    }
    else
    { // senao, devo fazer o shift
        do
        {
            fseek(fp, -sizeof(Contato), SEEK_CUR);
            fwrite(&c, sizeof(Contato), 1, fp);
            c = cAux;
            nR = fread(&cAux, sizeof(Contato), 1, fp);
        } while (nR == 1);

        fwrite(&c, sizeof(Contato), 1, fp);
    }

    return;
}
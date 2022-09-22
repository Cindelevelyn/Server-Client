// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct cont
{
    char nome[100];
    char endereco[200];
    int idade;
} Contato;

void buscarContato(FILE *fp);
int getFileSize();
void insertContato(Contato contato, FILE *fp);
void exibeContato(Contato *contato);
void exibeContatos(FILE *fp);

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
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    /*ARQUIVOOOOOOOOOOOOO*/

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

    /*MENUUUUUUUUUUUUUUUUUUU*/

    do
    {
        readOp = read(new_socket, &op, sizeof(op));
        printf("%d\n\n\n\n\n", readOp);
        // valread = read(new_socket, &contato, sizeof(contato));
        // printf("%s\n", buffer);

        if (readOp != 4)
        {
            switch (readOp)
            {
            case 1:
                printf("chegou");
                valread = read(new_socket, &contato, sizeof(contato));
                sz = getFileSize(fp);
                if (sz == 0)
                {
                    fwrite(&contato, sizeof(Contato), 1, fp);
                    printf("escrevi");
                }
                else
                {
                    insertContato(contato, fp);
                    printf("escrevi no else");
                }
                break;
            case 2:
                printf("exibi????");
                exibeContatos(fp);
                break;
            case 3:
                buscarContato(fp);
                break;
            case 4:
                break;
            default:
                printf("\nOpcao invalida!!!");
            }
        }
    } while (readOp != 4);

    /*-------------------------------*/

    // printf("%s\n", contato.endereco);
    // printf("%s\n", contato.nome);
    // printf("%d\n", contato.idade);

    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");

    // closing the connected socket
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

void exibeContato(Contato *contato)
{
    printf("\nNome: %s", contato->nome);
    printf("\nEndereco: %s", contato->endereco);
    printf("\n%d\n", contato->idade);
}

void exibeContatos(FILE *fp)
{
    Contato aux;
    rewind(fp);
    while (fread(&aux, sizeof(Contato), 1, fp))
    {
        exibeContato(&aux);
    }
    printf("\nDigite qualquer coisa para voltar...");
    getchar();
    getchar();
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

void buscarContato(FILE *fp)
{
    char nome[100];
    printf("\nDigite o nome: ");
    scanf(" %100[0-9a-zA-Z ]", nome);
    Contato aux;
    rewind(fp);
    while (fread(&aux, sizeof(Contato), 1, fp))
    {
        if (strcmp(aux.nome, nome) == 0)
        {
            exibeContato(&aux);
        }
    }
    printf("\nDigite qualquer coisa para voltar...");
    getchar();
    getchar();
}
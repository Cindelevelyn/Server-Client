// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


typedef struct cont
{
    char nome[100];
    char endereco[200];
    int idade;
} Contato;

int exibeMenu();
void getContato(Contato *contato);
void exibeContato(Contato *contato);
void exibeContatos(FILE *fp);
void buscarContato(FILE *fp);

int main(int argc, char const *argv[])
{

    Contato contato;
    pthread_t threads;

    // stpcpy(contato.endereco, "Zanete Dantte");
    // strcpy(contato.nome, "Cintra");
    // contato.idade = 23;

    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char hello[1024];
    strcpy(hello, "Hello from Client");
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr,
                             sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int op;
    FILE *fp;

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

    int leitura;

    /*MENU*/
    do
    {
        op = exibeMenu();
        if (op != 4)
        {
            switch (op)
            {
            case 1:
                send(sock, &op, sizeof(op), 0);
                getContato(&contato);
                send(sock, &contato, sizeof(contato), 0);
                break;
            case 2:
                send(sock, &op, sizeof(op), 0);
                read(sock, fp, sizeof(fp));
                exibeContatos(fp);
                break;
            case 3:
                send(sock, &op, sizeof(op), 0);
                read(sock, fp, sizeof(fp));
                buscarContato(fp);
                break;
            case 4:
                break;
            default:
                printf("\nOpcao invalida!!!");
            }
        }

    } while (op != 4);

    // closing the connected socket
    close(client_fd);
    return 0;
}

int exibeMenu()
{
    system("clear");
    int op;
    printf("\n1-) Cadastrar novo contato");
    printf("\n2-) Exibir todos os contatos");
    printf("\n3-) Buscar contato por nome");
    printf("\n4-) Sair\n");
    scanf("%d", &op);
    return op;
}

void getContato(Contato *contato)
{
    printf("\nDigite o nome: ");
    scanf(" %100[0-9a-zA-Z ]", contato->nome);
    printf("\nDigite o endereco: ");
    scanf(" %100[0-9a-zA-Z ]", contato->endereco);
    printf("\nDigite a idade: ");
    scanf("%d", &contato->idade);
    printf("\nDigite qualquer coisa para voltar...");
    getchar();
    getchar();
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

void buscarContato(FILE *fp)
{
    char nome[100];
    printf("\nDigite o nome: ");
    scanf(" %100[0-9a-zA-Z ]", nome);
    Contato aux;
    rewind(fp);
    int achou = 0;
    while (fread(&aux, sizeof(Contato), 1, fp))
    {
        if (strcmp(aux.nome, nome) == 0)
        {
            exibeContato(&aux);
            achou = 1;
        }
    }
    if(achou == 0)
    {
        printf("Não há clientes com esse nome...");
    }
    printf("\nDigite qualquer coisa para voltar...");
    getchar();
    getchar();
}
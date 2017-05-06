
// sgsize must be >= 5 && <= 10
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <string.h>

#include "clique-count.c";

int random_int(int min, int max);
void load_counterexample(char *name, int m);
void print_counterexample(int *g, int m);
int random_int(int min, int max) { return min + rand() % (max - min); }

int *g;
int m;

void print_counterexample(int *g, int m)
{
    int i, j;
    for (i = 0; i < (m - 11) / 2; i++)
    {
        printf("*-");
    }
    printf(" Counter example at %d ", m);
    for (i = 0; i < (m - 11) / 2; i++)
    {
        printf("-*");
    }
    printf("\n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < m; j++)
        {
            printf("%d ", g[i * m + j]);
        }
        printf("\n");
    }
}

void write_counterexample(int *g, int m)
{
    char name[40];
    sprintf(name, "counterexamples/%d.txt", m);

    FILE *out = fopen(name, "w");
    fprintf(out, "%d %d\n", m, 0);

    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < m; j++)
        {
            fprintf(out, "%d ", g[i * m + j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

int get_best_example()
{
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[2000];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return -1;
    }

    puts("Connected\n");

    // keep communicating with server

    // Send some data

    char clientMessage[100];
    sprintf(clientMessage, "ClientHello %d\0", m);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0)
    {
        puts("Send failed");
    }
    else
    {
        puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    if (recv(sock, server_reply, 2000, 0) < 0)
    {
        puts("recv failed");
    }
    else
    {
        printf("sr: %s \n", server_reply);
    }

    // if (strcmp(server_reply, "CONTINUE\0") == 0)
    if(server_reply[0] == 'C')
    {
        printf("server reply: %s\n", server_reply); 
        close(sock);
        return m;
    }
    else
    {    
        close(sock);
        printf("server reply: %s\n", server_reply); 
        int best = atoi(server_reply);
        printf("Best: %d\n", best); 
        get_best_sample_file(best);
        return best;
    }

    close(sock);
    return -1;
}


void load_counterexample_from_string(char *s, int m)
{
    free(g);
    // printf("M: %d\n", m);
    // printf("M: %s\n", s);

    g = (int *)malloc(m * m * sizeof(int));

    int i;
    for (i = 0; i < m * m; i++)
    {
        g[i] = s[i] - '0';
    }
    // print_counterexample(g, m);

}

int get_best_sample_file(int best)
{
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[500000];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return -1;
    }

    puts("Connected\n");

    // keep communicating with server

    // Send some data

    char clientMessage[100];
    sprintf(clientMessage, "GetSample %d\0", best);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0)
    {
        puts("GetSample Send failed");
    }
    else
    {
        puts("GetSample send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    ssize_t res = recv(sock, server_reply, 500000, 0); 
    // printf("Server reply %s \n", server_reply); 
    m = best; 
    if (res < 0)
    {
        puts("GetSample recv failed");
    }
    
    else if(res < best * best) get_best_sample_file(best); 
    else
    {
        // printf("%s\n", server_reply); 
        load_counterexample_from_string(server_reply, best);
    }

    close(sock);
    return -1;
}


void load_counterexample(char *name, int m)
{
    free(g);
    FILE *file = fopen(name, "r");

    printf("%s\n", name);

    int count;
    fscanf(file, "%d", &m);
    fscanf(file, "%d", &count);
    printf("M: %d\n", m);
    printf("Count: %d\n", count);

    g = (int *)malloc(m * m * sizeof(int));

    int i;
    for (i = 0; i < m * m; i++)
    {
        fscanf(file, "%d", &g[i]);
    }

    fclose(file);
}

void increment_counter()
{
    int *g2 = (int *)malloc((m + 1) * (m + 1) * sizeof(int));
    memset(g2, 0, sizeof(g2[0]) * (m + 1) * (m + 1));

    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = i; j < m; j++)
        {
            g2[i * (m + 1) + j] = g[i * m + j];
        }
    }
    m++;
    free(g);
    g = g2;
}

void flip_entry(int *matrix, int row, int column, int m)
{
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        FILE *file = fopen(argv[1], "r");
        int count;
        fscanf(file, "%d", &m);
        fscanf(file, "%d", &count);
        printf("M: %d\n", m);
        printf("Count: %d\n", count);

        g = (int *)malloc(m * m * sizeof(int));

        int i;
        for (i = 0; i < m * m; i++)
        {
            fscanf(file, "%d", &g[i]);
        }

        fclose(file);
    }

    else
    {
        // Start from best at server
        m = get_best_example();
        char name[40];
        sprintf(name, "counterexamples/%d.txt", m);
        load_counterexample(name, m);
    }

    int c_count = INT_MAX;
    int previous = INT_MAX;
    while (1)
    {
        int row = random_int(0, m);
        int column = random_int(row, m);

        flip_entry(g, row, column, m);
        c_count = CliqueCount(g, m);
        printf("Number of cliques at %d: %d\n", m, c_count);
        if (c_count == 0)
        {
            //print_counterexample(g, m);
            write_counterexample(g, m);
            increment_counter();
        }

        // Flip back if worse
        if (c_count > previous)
        {
            flip_entry(g, row, column, m);
        }

        previous = c_count;

        m = get_best_example();
        // if (m2 > m)
        // {
        //     m = m2;
        //     previous = INT_MAX;
        //     char name[40];
        //     sprintf(name, "counterexamples/%d.txt", m);
        //     load_counterexample(name, m);
        // }
    }
}


// sgsize must be >= 5 && <= 10
#include <arpa/inet.h>
#include <arpa/inet.h> //inet_addr
#include <dirent.h>
#include <errno.h>
#include <fcntl.h> //fcntl
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> //printf
#include <stdlib.h>
#include <string.h>
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h> //usleep

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <string.h>

#include "clique-count.c"

#define CHUNK_SIZE 512

int random_int(int min, int max);

void print_counterexample(int* g, int m);

int build_socket();
void reset_state();
void update_best_clique();
void send_counterexample(char* alg_name, int* g, int m);
bool send_all(int socket, char* alg_name, int* buffer, size_t length);

int random_int(int min, int max) { return min + rand() % (max - min); }

char* server_ip;
int server_port;
int* g;
int* best_g;
int m, best_m, previous_clique_count, clique_count, best_clique_count,
    update_interval;

void print_counterexample(int* g, int m) {
    int i, j;
    for (i = 0; i < (m - 11) / 2; i++) {
        printf("*-");
    }
    printf(" Counter example at %d ", m);
    for (i = 0; i < (m - 11) / 2; i++) {
        printf("-*");
    }
    printf("\n");
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            printf("%d ", g[i * m + j]);
        }
        printf("\n");
    }
}

void get_next_work(char* alg_name) {
    int sock = build_socket();
    char clientMessage[100], server_reply[600000], buf[512];

    sprintf(clientMessage, "GetNextWork %s %d %d", alg_name, best_m,
            best_clique_count);

    printf("%s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        // puts("Send failed");
    } else {
        // puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    size_t count = 0, i = 0;
    size_t res = recv(sock, buf, 512, 0);
    while (res != 0 && count < 600000) {
        for (i = 0; i < res; i++) {
            server_reply[count] = buf[i];
            count++;
        }
        res = recv(sock, buf, 512, 0);
    }
    shutdown(sock, 2); // outgoing
    close(sock);
    if (server_reply[0] != 'C') {
        reset_state();
        char m_string[600000];
        sscanf(server_reply, "%d %s", &m, m_string);
        printf("************************ We got a new work item: %d "
               "************************\n",
               m);

        g = (int*)malloc(m * m * sizeof(int));

        for (i = 0; i < m * m; i++) {
            g[i] = m_string[i] - '0';
        }
        // print_counterexample(g, m);
    } else {
        printf("************************ SERVER REPLY: CONTINUE "
               "************************\n");
    }
}

int build_socket() {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    // puts("Socket created");

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    // Connect to remote server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed. Retrying in 20 seconds");
        sleep(20);
        return build_socket();
    }

    // puts("Connected\n");
    return sock;
}

bool send_all(int socket, char* alg_name, int* buffer, size_t length) {
    char message_head[40];
    sprintf(message_head, "PostExample %s %d %d ", alg_name, best_m,
            best_clique_count);
    // printf("Message head: %s \n", message_head);

    char message_body[length];
    int i;
    for (i = 0; i < length; i++) {
        sprintf(&message_body[i], "%d", buffer[i]);
    }

    char total_message[strlen(message_head) + length];

    for (i = 0; i < strlen(message_head); i++) {
        total_message[i] = message_head[i];
    }

    for (i = strlen(message_head); i < length + strlen(message_head); i++) {
        total_message[i] = message_body[i - strlen(message_head)];
    }

    printf("%s\n", message_head);
    send(socket, total_message, strlen(total_message), 0);

    return true;
}

void send_counterexample(char* alg_name, int* g, int m) {
    int sock = build_socket();
    send_all(sock, alg_name, best_g, best_m * best_m);
    close(sock);
}

void flip_entry(int* matrix, int row, int column, int m) {
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

void update_best_clique() {
    free(best_g);
    best_g = (int*)malloc(m * m * sizeof(int));
    int i;
    for (i = 0; i < m * m; i++) {
        best_g[i] = g[i];
    }
    best_m = m;
    best_clique_count = clique_count;
}

void reset_state() {
    clique_count = INT_MAX;
    best_clique_count = INT_MAX;
    previous_clique_count = INT_MAX;
    m = 0;
    best_m = 0;
    free(g);
    free(best_g);
    g = NULL;
    best_g = NULL;
}

void best_clique() {
    char* alg_name = "BestClique";
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    reset_state();

    get_next_work(alg_name);

    while (1) {
        int row = random_int(0, m);
        int column = random_int(row, m);

        flip_entry(g, row, column, m);
        clique_count = CliqueCount(g, m);
        printf("Number of cliques at %d: %d - best is %d\n", m, clique_count,
               best_clique_count);

        if (clique_count < best_clique_count) {
            printf("Updating best clique count: %d \n", clique_count);
            update_best_clique();
        } else {
            flip_entry(g, row, column, m);
        }

        if (clique_count == 0) {
            send_counterexample(alg_name, g, m);
            get_next_work(alg_name);
        } else {
            gettimeofday(&now, NULL);
            timediff = (now.tv_sec - begin.tv_sec) +
                       1e-6 * (now.tv_usec - begin.tv_usec);
            if (timediff > update_interval) {
                gettimeofday(&begin, NULL);
                send_counterexample(alg_name, g, m);
                get_next_work(alg_name);
            }
        }
    }
}

void end_flip(int number_of_bits_request) {
    char* alg_name = "EndFlip";
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    reset_state();

    get_next_work(alg_name);

    int i;
    while (1) {
        int row, column;
        int number_of_bits = number_of_bits_request;

        for (i = 0; i < number_of_bits; i++) {
            row = random_int(0, m);
            column = m - 1;
            flip_entry(g, row, column, m);
        }

        clique_count = CliqueCount(g, m);
        printf("Number of cliques at %d: %d - best is %d\n", m, clique_count,
               best_clique_count);

        if (clique_count < best_clique_count) {
            printf("Updating best clique count: %d \n", clique_count);
            update_best_clique();
        } else {
            flip_entry(g, row, column, m);
        }

        if (clique_count == 0) {
            send_counterexample(alg_name, g, m);
            get_next_work(alg_name);
        } else {
            gettimeofday(&now, NULL);
            timediff = (now.tv_sec - begin.tv_sec) +
                       1e-6 * (now.tv_usec - begin.tv_usec);
            if (timediff > update_interval) {
                gettimeofday(&begin, NULL);
                send_counterexample(alg_name, g, m);
                get_next_work(alg_name);
            }
        }
    }
}

int main(int argc, char** argv) {
    update_interval = 70;

    if (argc != 5)
        printf("Wrong number of arguments");
    else if (argc == 5) {
        server_ip = argv[1];
        server_port = atoi(argv[2]);
        int alg_type = atoi(argv[3]);
        int arg = atoi(argv[4]);
        //TODO make a case 1
        switch (alg_type) {
        case 2:
            best_clique();
            break;
        case 3:
            end_flip(arg);
            break;
        }
    }
}

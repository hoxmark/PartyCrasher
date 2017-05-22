#include "clique-count.c"
#include "clique-count-extend.c"
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h> //fcntl
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //socket
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

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
int server_port, update_interval;
struct PartyState {
    int width;
    int clique_count;
    int* g;
    int num_calculations;
};

struct PartyState* currentState;
struct PartyState* bestState;

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

    sprintf(clientMessage, "GetNextWork %s %d %d", alg_name, bestState->width,
            bestState->clique_count);

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
        sscanf(server_reply, "%d %d %s", &currentState->width,
               &currentState->clique_count, m_string);
        printf("************************ We got a new work item: %d - %d "
               "************************\n",
               currentState->width, currentState->clique_count);

        currentState->g = (int*)malloc(currentState->width *
                                       currentState->width * sizeof(int));

        for (i = 0; i < currentState->width * currentState->width; i++) {
            currentState->g[i] = m_string[i] - '0';
        }

        update_best_clique();
    } else if( server_reply[0]== 'C') {
        printf("************************ SERVER REPLY: CONTINUE "
               "************************\n");            
    } else if( server_reply[0]== 'R'){
        printf("************************ SERVER REPLY: RETRY "
               "************************\n Sleeping for 20 sec\n");  
        sleep(20);
        get_next_work(alg_name);
    } else {
        printf("************************ SERVER REPLY: UNKNOWN "
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

void send_counterexample(char* alg_name, int* buffer, int m) {
    int length = m * m;
    char message_head[40];
    sprintf(message_head, "PostExample %s %d %d %d ", alg_name,
            bestState->width, bestState->clique_count,
            bestState->num_calculations);

    // Copy the contents of the int array to our send body buffer
    char message_body[length];
    int i;
    for (i = 0; i < length; i++) {
        sprintf(&message_body[i], "%d", buffer[i]);
    }

    // WHole message we will send
    char total_message[strlen(message_head) + length];

    for (i = 0; i < strlen(message_head); i++) {
        total_message[i] = message_head[i];
    }

    for (i = strlen(message_head); i < length + strlen(message_head); i++) {
        total_message[i] = message_body[i - strlen(message_head)];
    }

    printf("%s\n", message_head);

    int sock = build_socket();
    send(sock, total_message, strlen(total_message), 0);

    // Server has taken our calculations into account - reset counter
    bestState->num_calculations = 0;
    close(sock);
}

void flip_entry(int* matrix, int row, int column, int m) {
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

void update_best_clique() {
    free(bestState->g);
    bestState->g = NULL;
    bestState->g =
        (int*)malloc(currentState->width * currentState->width * sizeof(int));
    int i;
    for (i = 0; i < currentState->width * currentState->width; i++) {
        bestState->g[i] = currentState->g[i];
    }
    bestState->width = currentState->width;
    bestState->clique_count = currentState->clique_count;
    printf("Best clique is now %d - %d \n", bestState->width,
           bestState->clique_count);
}

void update_current_clique() {
    free(currentState->g);
    currentState->g = NULL;
    currentState->g =
        (int*)malloc(bestState->width * bestState->width * sizeof(int));
    int i;
    for (i = 0; i < bestState->width * bestState->width; i++) {
        currentState->g[i] = bestState->g[i];
    }
    currentState->width = bestState->width;
    currentState->clique_count = bestState->clique_count;
    printf("Current clique is now %d - %d \n", currentState->width,
           currentState->clique_count);
}

void reset_state() {
    currentState->clique_count = INT_MAX;
    currentState->width = 0;
    currentState->num_calculations = 0;
    bestState->clique_count = INT_MAX;
    bestState->width = 0;
    bestState->num_calculations = 0;

    free(currentState->g);
    free(bestState->g);
    currentState->g = NULL;
    bestState->g = NULL;
}

void best_clique() {
    char* alg_name = "BestClique";
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    reset_state();

    get_next_work(alg_name);

    while (1) {
        int row = random_int(0, currentState->width);
        int column = random_int(row, currentState->width);

        flip_entry(currentState->g, row, column, currentState->width);
        currentState->clique_count = CliqueCount(currentState->g, currentState->width);
        bestState->num_calculations++
        ;
        printf("Number of cliques at %d: %d - best is %d\n",
               currentState->width, currentState->clique_count,
               bestState->clique_count);

        if (currentState->clique_count <= bestState->clique_count) {
            update_best_clique();
        } else if (currentState->clique_count > bestState->clique_count) {
            update_current_clique();
        }

        if (currentState->clique_count == 0) {
            send_counterexample(alg_name, currentState->g, currentState->width);
            get_next_work(alg_name);
        } else {
            gettimeofday(&now, NULL);
            timediff = (now.tv_sec - begin.tv_sec) +
                       1e-6 * (now.tv_usec - begin.tv_usec);
            if (timediff > update_interval) {
                gettimeofday(&begin, NULL);
                send_counterexample(alg_name, currentState->g,
                                    currentState->width);
                get_next_work(alg_name);
            }
        }
    }
}

void end_flip(int num_flips) {
    char* alg_name = "EndFlip";
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    reset_state();
    get_next_work(alg_name);

    int i;
    while (1) {
        int row, column;

        for (i = 0; i < num_flips; i++) {
            row = 0;
            column = random_int(0, currentState->width);
            flip_entry(currentState->g, row, column, currentState->width);
        }

        currentState->clique_count =
            CliqueCountExtend(currentState->g, currentState->width);
        bestState->num_calculations++;

        printf("Number of cliques at %d: %d - best is %d\n",
               currentState->width, currentState->clique_count,
               bestState->clique_count);

        if (currentState->clique_count <= bestState->clique_count) {
            update_best_clique();
        } else {
            update_current_clique();
        }

        if (currentState->clique_count == 0) {
            send_counterexample(alg_name, currentState->g, currentState->width);
            get_next_work(alg_name);
        } else {
            gettimeofday(&now, NULL);
            timediff = (now.tv_sec - begin.tv_sec) +
                       1e-6 * (now.tv_usec - begin.tv_usec);
            if (timediff > update_interval) {
                gettimeofday(&begin, NULL);
                send_counterexample(alg_name, currentState->g,
                                    currentState->width);
                get_next_work(alg_name);
            }
        }
    }
}

int main(int argc, char** argv) {
    srand(getpid());
    update_interval = 50;

    // Initialize structs
    struct PartyState standard, standard2;
    currentState = &standard;
    bestState = &standard2;

    // Allocate to one int to be able to free it the first time
    currentState->g = (int*)malloc(sizeof(int));
    bestState->g = (int*)malloc(sizeof(int));

    if (argc != 5)
        printf("Wrong number of arguments");
    else if (argc == 5) {
        server_ip = argv[1];
        server_port = atoi(argv[2]);
        int alg_type = atoi(argv[3]);
        int arg = atoi(argv[4]);
        // TODO make a case 1
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

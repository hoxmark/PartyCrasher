#ifndef UTILS_C
#define UTILS_C

#include <math.h>

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sockets
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <ctype.h>

// Sleep
#include <unistd.h>

// Globals
#include "globals.h"
#include "queue.c"

#define CHUNK_SIZE 512
void print_counterexample(int* g, int m);

int build_socket();
void get_tabu_list();
int get_next_work();
void send_counterexample(char* alg_name, int* buffer, int m, int clique_count, int calculations);
bool send_all(int socket, char* alg_name, int* buffer, size_t length);
int random_int(int min, int max);
struct PairTuple get_tabu_flip_index();

int random_int(int min, int max) { return min + rand() % (max - min); }

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

void update_best_clique() {
    free(bestState->g);
    bestState->g = NULL;
    bestState->g = (int*)malloc(currentState->width * currentState->width * sizeof(int));
    int i;
    for (i = 0; i < currentState->width * currentState->width; i++) {
        bestState->g[i] = currentState->g[i];
    }
    bestState->width = currentState->width;
    bestState->clique_count = currentState->clique_count;
    printf("Best clique is now %d - %d \n", bestState->width, bestState->clique_count);
}

void flip_entry(int* matrix, int row, int column, int m) {
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

void update_current_clique() {
    free(currentState->g);
    currentState->g = NULL;
    currentState->g = (int*)malloc(bestState->width * bestState->width * sizeof(int));
    int i;
    for (i = 0; i < bestState->width * bestState->width; i++) {
        currentState->g[i] = bestState->g[i];
    }
    currentState->width = bestState->width;
    currentState->clique_count = bestState->clique_count;
    printf("Current clique is now %d - %d \n", currentState->width, currentState->clique_count);
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

int get_next_work() {
    int sock = build_socket();
    char clientMessage[100], server_reply[600000], buf[CHUNK_SIZE];

    sprintf(clientMessage, "GetNextWork %s %s %d %d %d", uuid_str, alg_name, bestState->width, bestState->clique_count, bestState->num_calculations);

    printf("%s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        // puts("Send failed");
    } else {
        // puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    size_t count = 0, i = 0;
    size_t res = recv(sock, buf, CHUNK_SIZE, 0);
    while (res != 0 && count < 600000) {
        for (i = 0; i < res; i++) {
            server_reply[count] = buf[i];
            count++;
        }
        res = recv(sock, buf, CHUNK_SIZE, 0);
    }
    shutdown(sock, 2); // outgoing
    close(sock);

    // N means change algorithm
    if (server_reply[0] == 'N') {
        // char first_letter[10];
        // char new_algorithm[40];
        // sscanf(server_reply, "%s %s", first_letter, new_algorithm);
        // if(strcmp(&new_algorithm[0], "TabuSearch") == 0){
        //     alg_name = "TabuSearch";
        // }  else if(strcmp(&new_algorithm[0], "EndFlip") == 0){
        //     alg_name = "EndFlip";
        // }
        //
        // reset_state();
        // return get_next_work();
    }

    else if (server_reply[0] != 'C') {
        reset_state();

        char m_string[600000];
        sscanf(server_reply, "%d %d %s", &currentState->width, &currentState->clique_count, m_string);
        printf("************************ We got a new work item: %d - %d "
               "************************\n",
               currentState->width, currentState->clique_count);

        currentState->g = (int*)malloc(currentState->width * currentState->width * sizeof(int));

        for (i = 0; i < currentState->width * currentState->width; i++) {
            currentState->g[i] = m_string[i] - '0';
        }

        update_best_clique();
        // print_counterexample(currentState->g, currentState->width);
        // If we have tabusearch, get the tabu list
        // Dirtyfix because the tabu list never reset.
        // if (strcmp(alg_name, "TabuSearch") == 0 && currentState->clique_count != 2147483647) {
        // get_tabu_list();
        // }

        return 1;
    } else if (server_reply[0] == 'C') {
        printf("************************ SERVER REPLY: CONTINUE "
               "************************\n");
        return 0;
    } else if (server_reply[0] == 'R') {
        printf("************************ SERVER REPLY: RETRY "
               "************************\n Sleeping for 20 sec\n");
        sleep(20);
        return get_next_work(alg_name);
    } else {
        printf("************************ SERVER REPLY: UNKNOWN "
               "************************\n");
        return 0;
    }
}

struct PairTuple get_tabu_flip_index() {
    struct PairTuple ret;
    ret.i = 0;
    ret.j = 0;

    char clientMessage[100], server_reply[600000], buf[CHUNK_SIZE];
    sprintf(clientMessage, "GetNextTabuFlipIndex %s %s", uuid_str, alg_name);

    printf("%s \n", clientMessage);

    int sock = build_socket();

    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        // puts("Send failed");
    } else {
        // puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    size_t count = 0, i = 0;
    size_t res = recv(sock, buf, CHUNK_SIZE, 0);
    while (res != 0 && count < 600000) {
        for (i = 0; i < res; i++) {
            server_reply[count] = buf[i];
            count++;
        }
        res = recv(sock, buf, CHUNK_SIZE, 0);
    }
    shutdown(sock, 2); // outgoing
    close(sock);

    if (server_reply[0] != 'W') {
        sscanf(server_reply, "%d %d", &ret.i, &ret.j);
        printf("FLIP TO DO: %d %d\n", ret.i, ret.j);
        return ret;
    } else {
        // TODO Check this
        sleep(10);
        // return get_endflip_tabu_index();
        get_next_work();
        return get_tabu_flip_index();
    }
}

void get_tabu_list() {
    char clientMessage[100], server_reply[600000], buf[CHUNK_SIZE];
    sprintf(clientMessage, "GetTabuList %s %s", uuid_str, alg_name);

    printf("%s \n", clientMessage);

    int sock = build_socket();

    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        // puts("Send failed");
    } else {
        // puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    size_t count = 0, i = 0;
    size_t res = recv(sock, buf, CHUNK_SIZE, 0);
    while (res != 0 && count < 600000) {
        for (i = 0; i < res; i++) {
            server_reply[count] = buf[i];
            count++;
        }
        res = recv(sock, buf, CHUNK_SIZE, 0);
    }
    shutdown(sock, 2); // outgoing
    close(sock);

    QueueReset();
    int total_n, x, y;
    total_n = x = y = 0;
    count = 0;
    int temp, n;
    char* s = &server_reply[0];

    while (1 == sscanf(s + total_n, "%*[^0123456789]%d%n", &temp, &n)) {
        if (count % 2 == 0)
            x = temp;
        else {
            y = temp;
            QueuePut(x, y);
        }
        total_n += n;
        count++;
        // printf("%d \n", x);
    }

    QueuePrint();
}

void send_counterexample(char* alg_name, int* buffer, int m, int clique_count, int calculations) {
    int length = m * m;
    char message_head[100];

    // if (strcmp(alg_name, "TabuSearch") == 0) {
    sprintf(message_head, "PostExample %s %s %d %d %d ", uuid_str, alg_name, m, clique_count, calculations);
    // } else {
    // sprintf(message_head, "PostExample %s %s %d %d %d ", uuid_str, alg_name, bestState->width, bestState->clique_count,
    // bestState->num_calculations);
    // }

    // Copy the contents of the int array to our send body buffer
    char message_body[length];
    int i;
    for (i = 0; i < length; i++) {
        sprintf(&message_body[i], "%d", buffer[i]);
    }

    // Whole message we will send
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

//
void send_counterexample_tabu(char* alg_name, struct PairTuple doneFlip, int* buffer, int m, int clique_count, int calculations) {
    int length = m * m;
    char message_head[100];
    sprintf(message_head, "PostTabuExample %s %s %d %d %d %d %d ", uuid_str, alg_name, doneFlip.i, doneFlip.j, m, clique_count, calculations);

    // Copy the contents of the int array to our send body buffer
    char message_body[length];
    int i;
    for (i = 0; i < length; i++) {
        sprintf(&message_body[i], "%d", buffer[i]);
    }

    // Whole message we will send
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

void generate_random_uuid() {
    int t = 0;
    char* szTemp = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    char* szHex = "0123456789ABCDEF-";
    int nLen = strlen(szTemp);

    for (t = 0; t < nLen + 1; t++) {
        int r = rand() % 16;
        char c = ' ';

        switch (szTemp[t]) {
        case 'x': {
            c = szHex[r];
        } break;
        case 'y': {
            c = szHex[(r & 0x03) | 0x08];
        } break;
        case '-': {
            c = '-';
        } break;
        case '4': {
            c = '4';
        } break;
        }

        uuid_str[t] = (t < nLen) ? c : 0x00;
    }
}

#endif

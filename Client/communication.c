#ifndef COMMUNICATIONS_C
#define COMMUNICATIONS_C

// Sockets
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Sleep
#include <unistd.h>

#include "globals.h"
#include "queue.c"
#include "utils.c"

// Chunk size used in recieves
#define CHUNK_SIZE 512

int build_socket(char* serverIp, int serverPort);
int get_next_work();
struct PairTuple get_tabu_flip_index();
void get_tabu_list();
void send_counterexample(char* alg_name, int* buffer, int m, int clique_count, int calculations);
void send_counterexample_tabu(char* alg_name, struct PairTuple doneFlip, int* buffer, int m, int clique_count, int calculations);
void getServerIp();

int build_socket(char* serverIp, int serverPort) {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    // puts("Socket created");

    struct timeval timeout;
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
        printf("setsockopt failed\n");

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
        printf("setsockopt failed\n");

    server.sin_addr.s_addr = inet_addr(serverIp);
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        // Socket timed out. Get a new IP from the load balancer
        perror("Socket error");
        printf(" Wait for 10 seconds, then get a new IP from load balancer. \n");
        close(sock);
        sleep(10);
        getServerIp();
        return build_socket(serverIp, serverPort);
    }

    return sock;
}

int get_next_work() {
    int sock = build_socket(server_ip, server_port);
    char clientMessage[100], server_reply[600000], buf[CHUNK_SIZE];

    sprintf(clientMessage, "GetNextWork %s %s %d %d %d", uuid_str, alg_name, bestState->width, bestState->clique_count, bestState->num_calculations);

    printf("%s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        printf("Send failed\n");
    } else {
        printf("Send success\n");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    size_t count = 0, i = 0;
    size_t res = recv(sock, buf, CHUNK_SIZE, 0);
    if (res == -1) {
        printf("Oh dear, something went wrong with recv()! %s\n", strerror(errno));
        close(sock);
        sleep(10);
        return get_next_work();
    } else {

        while (res != 0 && count < 600000) {
            for (i = 0; i < res; i++) {
                server_reply[count] = buf[i];
                count++;
            }
            res = recv(sock, buf, CHUNK_SIZE, 0);
        }
        shutdown(sock, 2); // outgoing
        close(sock);

        if (server_reply[0] == 'R') {
            printf("************************ SERVER REPLY: RETRY "
                   "************************\n Sleeping for 20 sec\n");
            sleep(20);
            return get_next_work(alg_name);
        } else if (server_reply[0] == 'N') {
            // printf("N\n");
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

            if (currentState->width == 0)
                return get_next_work();

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
        } else {
            printf("************************ SERVER REPLY: UNKNOWN "
                   "************************\n");
            return 0;
        }
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

    int sock = build_socket(server_ip, server_port);

    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        puts("Send failed");
    } else {
        puts("send succes");
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

    if (server_reply[0] == 'R') {
        printf("************************ SERVER REPLY: RETRY "
               "************************\n Sleeping for 20 sec\n");
        sleep(20);
        return get_tabu_flip_index();

    } else if (server_reply[0] != 'W') {
        sscanf(server_reply, "%d %d", &ret.i, &ret.j);
        printf("FLIP TO DO: %d %d\n", ret.i, ret.j);
        return ret;
    } else {
        sleep(10);
        get_next_work();
        return get_tabu_flip_index();
    }
}

void get_tabu_list() {
    char clientMessage[100], server_reply[600000], buf[CHUNK_SIZE];
    sprintf(clientMessage, "GetTabuList %s %s", uuid_str, alg_name);

    printf("%s \n", clientMessage);

    int sock = build_socket(server_ip, server_port);

    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        puts("Send failed");
    } else {
        puts("send succes");
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

    if (server_reply[0] == 'R') {
        printf("************************ SERVER REPLY: RETRY "
               "************************\n Sleeping for 20 sec\n");
        sleep(20);
        get_tabu_list();
    } else {
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
        }
        QueuePrint();
    }
}

void send_counterexample(char* alg_name, int* buffer, int m, int clique_count, int calculations) {
    int length = m * m;
    char message_head[100];

    sprintf(message_head, "PostExample %s %s %d %d %d ", uuid_str, alg_name, m, clique_count, calculations);

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

    int sock = build_socket(server_ip, server_port);
    send(sock, total_message, strlen(total_message), 0);

    // Server has taken our calculations into account - reset counter
    bestState->num_calculations = 0;
    currentState->num_calculations = 0;
    close(sock);
}

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

    int sock = build_socket(server_ip, server_port);
    send(sock, total_message, strlen(total_message), 0);

    // Server has taken our calculations into account - reset counter
    bestState->num_calculations = 0;
    currentState->num_calculations = 0;
    close(sock);
}

void getServerIp() {
    printf("GET SERVER IP");

    int sock = build_socket(load_balancer_ip, load_balancer_port);
    char clientMessage[100], server_reply[600000], buf[512];

    sprintf(clientMessage, "GetServerIp");

    printf("%s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        puts("Send failed");
    } else {
        puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    if (server_reply[0] == 'R') {
        close(sock);
        printf("************************ SERVER REPLY: RETRY "
               "************************\n Sleeping for 20 sec\n");
        sleep(20);
        getServerIp();
    } else {
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

        printf("%s\n", server_reply);
        char* temp = (char*)malloc(40 * sizeof(char));
        sscanf(server_reply, "%s %d", temp, &server_port);
        server_ip = temp;
        printf("We got a new server IP: %s %d \n", server_ip, server_port);
    }
}
#endif

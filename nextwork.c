
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
void load_counterexample(char* name, int m);
void print_counterexample(int* g, int m);
int get_best_example(char* alg_name);
int get_best_sample_file(char* alg_name, int best);
int build_socket();
void increment_counter();
void send_counterexample(char* alg_name, int* g, int m);
bool send_all(int socket, char* alg_name, int* buffer, size_t length);

int random_int(int min, int max) { return min + rand() % (max - min); }

int* g;
int m, clique_count;

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

void write_counterexample(int* g, int m) {
    char name[40];
    sprintf(name, "counterexamples/%d.txt", m);

    FILE* out = fopen(name, "w");
    fprintf(out, "%d %d\n", m, 0);

    int i, j;
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            fprintf(out, "%d ", g[i * m + j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

int recv_timeout(int s, int timeout) {
    int size_recv = 0, total_size = 0, j = 0;
    struct timeval begin, now;
    char chunk[CHUNK_SIZE];
    double timediff;

    gettimeofday(&begin, NULL);
    int count = 0;

    while (1) {
        gettimeofday(&now, NULL);
        // time elapsed in seconds
        timediff =
            (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
        // if you got some data, then break after timeout
        if (total_size > 0 && timediff > timeout) {
            break;
        }
        // if you got no data at all, wait a little longer, twice the timeout
        else if (timediff > timeout * 2) {
            break;
        }
        memset(chunk, 0, CHUNK_SIZE); // clear the variable
        if ((size_recv = recv(s, chunk, CHUNK_SIZE, 0)) < 0) {
            // if nothing was received then we want to wait a little before
            // trying again, 0.1 seconds

            usleep(100000);
        } else {
            total_size += size_recv;
            for (j = 0; j < size_recv; j++) {
                g[count] = chunk[j] - '0';
                count++;
                if (count > m * m)
                    break;
                // printf("\n%s\n", chunk);
            }
        }
    }
    // print_counterexample(g, m);
    return total_size;
}

void get_next_work(char *alg_name) {
    int sock = build_socket();
    char clientMessage[100], server_reply[600000], buf[512];

    sprintf(clientMessage, "GetNextWork %s %d %d", alg_name, m, clique_count);

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
    while(res != 0 && count < 600000){
        for(i = 0; i < res ; i++){
            server_reply[count] = buf[i];
            count++;
        }
        res = recv(sock, buf, 512, 0);
    }
    shutdown(sock, 2); // outgoing
    close(sock);
    printf("Server reply: %c\n", server_reply[0]);
    if(server_reply[0] != 'C'){
        m = 0;
        char m_string[600000];
        sscanf(server_reply, "%d %s", &m, m_string);

        printf("We got a new work item. At %d \n", m);

        free(g);
        g = (int *) malloc(m * m * sizeof(int));

        for(i = 0; i < m * m; i++){
            g[i] = m_string[i] - '0';
        }
        // print_counterexample(g, m);
    }
}

int get_best_example(char* alg_name) {
    int sock = build_socket();
    char clientMessage[100], server_reply[2000];

    sprintf(clientMessage, "ClientHello %s %d %d", alg_name, m, clique_count);
    printf("Sending message: %s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        // puts("Send failed");
    } else {
        // puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    if (recv(sock, server_reply, 2000, 0) < 0) {
        // puts("recv failed");
    } else {
        // Error. TODO
        // printf("sr: %s \n", server_reply);
    }

    // if (strcmp(server_reply, "CONTINUE\0") == 0)
    if (server_reply[0] == 'C') {
        // printf("server reply: %s\n", server_reply);
        close(sock);
        return m;
    } else {
        close(sock);
        // printf("server reply: %s\n", server_reply);

        m = atoi(server_reply);
        printf("The best at server is %d. \n", m);

        // If server has the better example - get it and start working on next

        get_best_sample_file(alg_name, m);
        increment_counter();
        // }

        // If not, get the current best and improve it
        printf("Now working on %d \n", m);
        // print_counterexample(g, m);
        return m;
    }
    return -1;
}

void get_best_clique_count(char* alg_name) {
    int sock = build_socket();
    char clientMessage[100], server_reply[2000];

    sprintf(clientMessage, "GetCliqueCount %s %d %d", alg_name, m,
            clique_count);
    printf("Sending message: %s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        // puts("Send failed");
    } else {
        // puts("send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server
    if (recv(sock, server_reply, 2000, 0) < 0) {
        // puts("recv failed");
    } else {
        // Error. TODO
        // printf("sr: %s \n", server_reply);
    }

    if (server_reply[0] == 'C') {
        // printf("server reply: %s\n", server_reply);
        close(sock);
    } else {

        free(g);
        g = (int*)malloc(m * m * sizeof(int));
        recv_timeout(sock, 5);
        close(sock);
    }
}

void load_counterexample_from_string(char* s, int m) {
    free(g);

    g = (int*)malloc(m * m * sizeof(int));

    int i;
    for (i = 0; i < m * m; i++) {
        g[i] = s[i] - '0';
    }
    print_counterexample(g, m);
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

    server.sin_addr.s_addr = inet_addr("104.198.30.238");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    // Connect to remote server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed. Retrying in 20 seconds");
        sleep(20);
        return build_socket();
    }

    // puts("Connected\n");
    return sock;
}

int get_best_sample_file(char* alg_name, int best) {
    int sock = build_socket();
    char clientMessage[100];

    // Send some data
    sprintf(clientMessage, "GetSample %s %d %d", alg_name, best, clique_count);
    printf("%s \n", clientMessage);
    if (send(sock, clientMessage, strlen(clientMessage), 0) < 0) {
        puts("GetSample Send failed");
    } else {
        // puts("GetSample send succes");
    }

    shutdown(sock, 1); // outgoing

    // Receive a reply from the server

    printf("Best is %d \n", best);
    free(g);
    g = (int*)malloc(best * best * sizeof(int));
    recv_timeout(sock, 5);
    close(sock);
    return -1;
}

void load_counterexample(char* name, int m) {
    free(g);
    FILE* file = fopen(name, "r");

    // printf("%s\n", name);

    int count;
    fscanf(file, "%d", &m);
    fscanf(file, "%d", &count);
    // printf("M: %d\n", m);
    // printf("Count: %d\n", count);

    g = (int*)malloc(m * m * sizeof(int));

    int i;
    for (i = 0; i < m * m; i++) {
        fscanf(file, "%d", &g[i]);
    }

    fclose(file);
}

void increment_counter() {
    int* g2 = (int*)malloc((m + 1) * (m + 1) * sizeof(int));
    memset(g2, 0, sizeof(g2[0]) * (m + 1) * (m + 1));

    int i, j;
    for (i = 0; i < m; i++) {
        for (j = i; j < m; j++) {
            g2[i * (m + 1) + j] = g[i * m + j];
        }
    }
    m++;
    free(g);
    g = g2;
}

bool send_all(int socket, char* alg_name, int* buffer, size_t length) {
    char message_head[40];
    sprintf(message_head, "PostExample %s %d %d ", alg_name, m, clique_count);
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
    send_all(sock, alg_name, g, m * m);
    close(sock);
}

void flip_entry(int* matrix, int row, int column, int m) {
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

void random_flip() {
    char* alg_name = "RandomFlip";

    clique_count = INT_MAX;
    int previous = INT_MAX;
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    m = get_best_example(alg_name);
    while (1) {
        get_next_work(alg_name);
        int row = random_int(0, m);
        int column = random_int(row, m);

        flip_entry(g, row, column, m);
        clique_count = CliqueCount(g, m);
        printf("Number of cliques at %d: %d\n", m, clique_count);
        if (clique_count == 0) {
            send_counterexample(alg_name, g, m);
            // write_counterexample(g, m);
            increment_counter();
            clique_count = INT_MAX;
        }

        // Flip back if worse
        if (clique_count > previous) {
            flip_entry(g, row, column, m);
        }
        previous = clique_count;
        gettimeofday(&now, NULL);

        timediff =
            (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
        if (timediff > 20) {
            gettimeofday(&begin, NULL);
            if (clique_count < 10)
                send_counterexample(alg_name, g, m);
            m = get_best_example(alg_name);
        }
    }
}

void best_clique() {
    char* alg_name = "BestClique";

    clique_count = INT_MAX;
    int previous = INT_MAX;
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    // m = get_best_example(alg_name);
    get_next_work(alg_name);
    while (1) {
        int row = random_int(0, m);
        int column = random_int(row, m);

        flip_entry(g, row, column, m);
        clique_count = CliqueCount(g, m);
        printf("Number of cliques at %d: %d\n", m, clique_count);
        if (clique_count == 0) {
            send_counterexample(alg_name, g, m);
            get_next_work(alg_name);
            clique_count = INT_MAX;
        }

        // Flip back if worse
        if (clique_count > previous) {
            flip_entry(g, row, column, m);
            // clique_count = previous;
        }

        gettimeofday(&now, NULL);

        timediff =
            (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
        if (timediff > 100) {
            gettimeofday(&begin, NULL);
            send_counterexample(alg_name, g, m);
            // m = get_best_example(alg_name);
            get_next_work(alg_name);
        }
        previous = clique_count;
    }
}

int main(int argc, char** argv) {
    if (argc == 2) {
        FILE* file = fopen(argv[1], "r");
        int count;
        fscanf(file, "%d", &m);
        fscanf(file, "%d", &count);
        printf("M: %d\n", m);
        printf("Count: %d\n", count);

        g = (int*)malloc(m * m * sizeof(int));

        int i;
        for (i = 0; i < m * m; i++) {
            fscanf(file, "%d", &g[i]);
        }
        m = 20;
        fclose(file);
    }
    // random_flip();
    best_clique();
}

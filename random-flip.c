
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

int random_int(int min, int max);
int load_counterexample(FILE* file, int* g);
void print_counterexample(int* g, int m);

int CliqueCount(int* g, int gsize) {
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int p;
    int q;
    int r;
    int count = 0;
    int sgsize = 10;

    for (i = 0; i < gsize - sgsize + 1; i++) {
        for (j = i + 1; j < gsize - sgsize + 2; j++) {
            for (k = j + 1; k < gsize - sgsize + 3; k++) {
                if ((g[i * gsize + j] == g[i * gsize + k]) &&
                    (g[i * gsize + j] == g[j * gsize + k])) {
                    for (l = k + 1; l < gsize - sgsize + 4; l++) {
                        if ((g[i * gsize + j] == g[i * gsize + l]) &&
                            (g[i * gsize + j] == g[j * gsize + l]) &&
                            (g[i * gsize + j] == g[k * gsize + l])) {
                            for (m = l + 1; m < gsize - sgsize + 5; m++) {
                                if ((g[i * gsize + j] == g[i * gsize + m]) &&
                                    (g[i * gsize + j] == g[j * gsize + m]) &&
                                    (g[i * gsize + j] == g[k * gsize + m]) &&
                                    (g[i * gsize + j] == g[l * gsize + m])) {
                                    if (sgsize <= 5) {
                                        count++;
                                    } else {
                                        for (n = m + 1; n < gsize - sgsize + 6;
                                             n++) {
                                            if ((g[i * gsize + j] ==
                                                 g[i * gsize + n]) &&
                                                (g[i * gsize + j] ==
                                                 g[j * gsize + n]) &&
                                                (g[i * gsize + j] ==
                                                 g[k * gsize + n]) &&
                                                (g[i * gsize + j] ==
                                                 g[l * gsize + n]) &&
                                                (g[i * gsize + j] ==
                                                 g[m * gsize + n])) {
                                                if (sgsize <= 6) {
                                                    count++;
                                                } else {
                                                    for (o = n + 1;
                                                         o < gsize - sgsize + 7;
                                                         o++) {
                                                        if ((g[i * gsize + j] ==
                                                             g[i * gsize +
                                                               o]) &&
                                                            (g[i * gsize + j] ==
                                                             g[j * gsize +
                                                               o]) &&
                                                            (g[i * gsize + j] ==
                                                             g[k * gsize +
                                                               o]) &&
                                                            (g[i * gsize + j] ==
                                                             g[l * gsize +
                                                               o]) &&
                                                            (g[i * gsize + j] ==
                                                             g[m * gsize +
                                                               o]) &&
                                                            (g[i * gsize + j] ==
                                                             g[n * gsize +
                                                               o])) {
                                                            if (sgsize <= 7) {
                                                                count++;
                                                            } else {
                                                                for (
                                                                    p = o + 1;
                                                                    p <
                                                                    gsize -
                                                                        sgsize +
                                                                        8;
                                                                    p++) {
                                                                    if ((g[i * gsize +
                                                                           j] ==
                                                                         g[i * gsize +
                                                                           p]) &&
                                                                        (g[i * gsize +
                                                                           j] ==
                                                                         g[j * gsize +
                                                                           p]) &&
                                                                        (g[i * gsize +
                                                                           j] ==
                                                                         g[k * gsize +
                                                                           p]) &&
                                                                        (g[i * gsize +
                                                                           j] ==
                                                                         g[l * gsize +
                                                                           p]) &&
                                                                        (g[i * gsize +
                                                                           j] ==
                                                                         g[m * gsize +
                                                                           p]) &&
                                                                        (g[i * gsize +
                                                                           j] ==
                                                                         g[n * gsize +
                                                                           p]) &&
                                                                        (g[i * gsize +
                                                                           j] ==
                                                                         g[o * gsize +
                                                                           p])) {
                                                                        if (sgsize <=
                                                                            8) {
                                                                            count++;
                                                                        } else {
                                                                            for (
                                                                                q = p +
                                                                                    1;
                                                                                q <
                                                                                gsize -
                                                                                    sgsize +
                                                                                    9;
                                                                                q++) {
                                                                                if ((g[i * gsize +
                                                                                       j] ==
                                                                                     g[i * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[j * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[k * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[l * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[m * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[n * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[o * gsize +
                                                                                       q]) &&
                                                                                    (g[i * gsize +
                                                                                       j] ==
                                                                                     g[p * gsize +
                                                                                       q])) {
                                                                                    if (sgsize <=
                                                                                        9) {
                                                                                        count++;
                                                                                    } else {
                                                                                        for (
                                                                                            r = q +
                                                                                                1;
                                                                                            r <
                                                                                            gsize -
                                                                                                sgsize +
                                                                                                10;
                                                                                            r++) {
                                                                                            if ((g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[i * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[j * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[k * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[l * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[m * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[n * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[o * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[p * gsize +
                                                                                                   r]) &&
                                                                                                (g[i * gsize +
                                                                                                   j] ==
                                                                                                 g[q * gsize +
                                                                                                   r])) {
                                                                                                count++;
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return (count);
}

int random_int(int min, int max) { return min + rand() % (max - min); }

void print_counterexample(int* g, int m) {
    int i, j;
    for ( i = 0; i < (m - 11) / 2; i++) {
        printf("*-");
    }
    printf(" Counter example at %d ", m);
    for ( i = 0; i < (m - 11) / 2; i++) {
        printf("-*");
    }
    printf("\n");
    for ( i = 0; i < m; i++) {
        for ( j = 0; j < m; j++) {
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

int get_best_example2() {
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[2000];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

    // Connect to remote server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return -1;
    }

    puts("Connected\n");

    // keep communicating with server
    while (1) {
        // printf("Enter message : ");
        // scanf("%s", message);

        // Send some data
        // if (send(sock, message, strlen(message), 0) < 0) {
        //     puts("Send failed");
        //     return ;
        // }

        // Receive a reply from the server
        if (recv(sock, server_reply, 2000, 0) < 0) {
            puts("recv failed");
            break;
        }

        // puts("Server reply :");
        // puts(server_reply);
        return atoi(server_reply);
        break;
    }

    close(sock);
    return -1;
}

void get_best_example() {

    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // if (inet_pton(AF_INET, htonl(INADDR_ANY), &serv_addr.sin_addr) <= 0) {
    //     printf("\n inet_pton error occured\n");
    //     return;
    // }
    //
    // if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    // {
    //     printf("\n Error : Connect Failed \n");
    //     return;
    // }

    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
        recvBuff[n] = 0;
        if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error\n");
        }
    }

    if (n < 0) {
        printf("\n Read error \n");
    }

    return;
}

int main(int argc, char** argv) {
    int* g = 0;
    int m;

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
        fclose(file);
    }

    else {
        // Start from best at server
        m = 0;
        m = get_best_example2();
        char name[40];
        sprintf(name, "counterexamples/%d.txt", m);
        FILE* file = fopen(name, "r");

        printf("%s\n", name);

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
        fclose(file);
    }

    // print_counterexample(g, m);

    int c_count = INT_MAX;
    int previous = INT_MAX;
    while (1) {
        int row = random_int(0, m);
        int column = random_int(row, m);

        if (g[row * m + column] == 0)
            g[row * m + column] = 1;
        else
            g[row * m + column] = 0;

        c_count = CliqueCount(g, m);
        printf("Number of cliques at %d: %d\n", m, c_count);
        if (c_count == 0) {
            print_counterexample(g, m);
            write_counterexample(g, m);
            int* g2 = (int*)malloc((m + 1) * (m + 1) * sizeof(int));
            memset(g2, 0, sizeof(g2[0]) * (m + 1) * (m + 1));

            int i, j;
            for (i = 0; i < m; i++) {
                for (j = i; j < m; j++) {
                    g2[i * (m + 1) + j] = g[i * m + j];
                }
            }
            m++;
            g = g2;
        }

        // Flip back if worse
        if (c_count > previous) {
            if (g[row * m + column] == 0)
                g[row * m + column] = 1;
            else
                g[row * m + column] = 0;
        }
        previous = c_count;


        int m2 = get_best_example2();
        if(m2 > m){
            m = m2;
            previous = INT_MAX;
            char name[40];
            sprintf(name, "counterexamples/%d.txt", m);
            FILE* file = fopen(name, "r");

            printf("%s\n", name);

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
            fclose(file);
        }
    }
}

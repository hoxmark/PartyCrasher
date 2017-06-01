// Declarations to please the compiler
#include "globals.h"
char* server_ip;
char* load_balancer_ip;
int load_balancer_port;
int server_port;
int update_interval;
char uuid_str[37];
struct PartyState* currentState;
struct PartyState* bestState;

#include "algorithms/bestclique.c"
#include "algorithms/bestflip.c"
#include "algorithms/endflip.c"
#include "algorithms/random-flip.c"

#include "utils.c"
#include <stdio.h>
#include <stdlib.h>

#include <uuid/uuid.h>

int main(int argc, char** argv) {
    srand(getpid());
    uuid_t id;
    uuid_generate(id);
    uuid_unparse_lower(id, uuid_str);
    printf("generate uuid=%s\n", uuid_str);

    update_interval = 10;

    // Initialize structs
    struct PartyState standard, standard2;
    currentState = &standard;
    bestState = &standard2;

    // Allocate to one int to be able to free it the first time
    currentState->g = (int*)malloc(sizeof(int));
    bestState->g = (int*)malloc(sizeof(int));

    if (argc != 7)
        printf("Wrong number of arguments");
    else if (argc == 7) {
        load_balancer_ip = argv[1];
        load_balancer_port = atoi(argv[2]);
        server_ip = argv[3];
        server_port = atoi(argv[4]);
        int alg_type = atoi(argv[5]);
        int arg = atoi(argv[6]);

        getServerIp();

        // TODO make a case 1
        switch (alg_type) {
        case 1:
            best_flip();
        case 2:
            best_clique();
            break;
        case 3:
            end_flip(arg);
            break;
        }
    }
}

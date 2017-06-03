// Declarations to please the compiler
#include "globals.h"
#include "queue.c"
char* server_ip;
int server_port;
int update_interval;
char uuid_str[40];
struct PartyState* currentState;
struct PartyState* bestState;
struct PairTuple bestFlip;
char* alg_name;

#include "algorithms/bestclique.c"
#include "algorithms/bestflip.c"
#include "algorithms/endflip.c"
#include "algorithms/endflip_tabu.c"
#include "algorithms/random-flip.c"
#include "algorithms/tabusearch.c"

#include "utils.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    srand(getpid());
    generate_random_uuid();
    printf("uuid=%s\n", uuid_str);

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
        case 1:
            // best_flip();
            tabu_search();
            break;
        case 2:
            best_clique();
            break;
        case 3:
            end_flip(arg);
            break;
        case 4:
            endflip_tabu();
            break;
        }
    }
}

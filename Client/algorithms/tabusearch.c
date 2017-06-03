#include "../counters/clique-count.c"
#include "../globals.h"
#include "../queue.c"
#include "../utils.c"
#include <sys/time.h>

void tabu_search() {
    alg_name = "TabuSearch";
    get_next_work();
    struct PairTuple flip_to_do = get_tabu_flip_index();
    while (1) {
            flip_entry(currentState->g, flip_to_do.i, flip_to_do.j, currentState->width);
            currentState->clique_count = CliqueCount(currentState->g, currentState->width);
            currentState->num_calculations++;
            printf("Number of cliques at %d: %d - best is %d\n", currentState->width, currentState->clique_count, bestState->clique_count);
            send_counterexample_tabu(alg_name, flip_to_do, currentState->g, currentState->width, currentState->clique_count, currentState->num_calculations);
            get_next_work();
            flip_to_do = get_tabu_flip_index();
    }
}

#include "../utils.c"

#include "../counters/clique-count.c"
#include "../globals.h"
#include "../communication.c"
#include <sys/time.h>

// Algorithm named "RandomFlip in the presentation"
void best_clique() {
    alg_name = "BestClique";
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    reset_state();
    get_next_work();

    while (1) {
        int row = random_int(0, currentState->width);
        int column = random_int(row, currentState->width);

        flip_entry(currentState->g, row, column, currentState->width);
        currentState->clique_count = CliqueCount(currentState->g, currentState->width);
        currentState->num_calculations++;
        printf("Number of cliques at %d: %d - best is %d\n", currentState->width, currentState->clique_count, bestState->clique_count);

        if (currentState->clique_count <= bestState->clique_count) {
            update_best_clique();
        } else if (currentState->clique_count > bestState->clique_count) {
            update_current_clique();
        }

        if (currentState->clique_count == 0) {
            send_counterexample(alg_name, currentState->g, currentState->width, currentState->clique_count, currentState->num_calculations);
            get_next_work();
        } else {
            gettimeofday(&now, NULL);
            timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
            if (timediff > update_interval) {
                gettimeofday(&begin, NULL);
                send_counterexample(alg_name, currentState->g, currentState->width, currentState->clique_count, currentState->num_calculations);
                get_next_work();
            }
        }
    }
}

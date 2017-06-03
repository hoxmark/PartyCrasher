#include "../counters/clique-count-extend.c"
#include "../globals.h"
#include "../utils.c"
#include <sys/time.h>

void end_flip(int num_flips) {
    alg_name = "EndFlip";
    double timediff;
    struct timeval begin, now;
    gettimeofday(&begin, NULL);

    reset_state();
    get_next_work();

    while (1) {
        int i, row, column;

        for (i = 0; i < num_flips; i++) {
            row = 0;
            column = random_int(0, currentState->width);
            flip_entry(currentState->g, row, column, currentState->width);
        }

        currentState->clique_count = CliqueCountExtend(currentState->g, currentState->width);
        currentState->num_calculations++;

        printf("Number of cliques at %d: %d - best is %d\n", currentState->width, currentState->clique_count, bestState->clique_count);

        if (currentState->clique_count <= bestState->clique_count) {
            update_best_clique();
        } else {
            update_current_clique();
        }

        if (currentState->clique_count == 0) {
            // send_counterexample(char *alg_name, int *buffer, int m, int clique_count, int calculations)
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

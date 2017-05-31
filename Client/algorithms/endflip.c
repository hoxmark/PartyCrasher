#include "../utils.c"
#include "../globals.h"
#include "../counters/clique-count-extend.c"
#include <sys/time.h>

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

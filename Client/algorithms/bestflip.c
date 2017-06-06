// #include "../counters/clique-count-row.c"
// #include "../globals.h"
// #include "../utils.c"
// #include <sys/time.h>
//
// void best_flip() {
//     alg_name = "BestFlip";
//     double timediff;
//     struct timeval begin, now;
//     gettimeofday(&begin, NULL);
//
//     reset_state();
//     get_next_work();
//
//     int i;
//     while (1) {
//         for (i = 0; i < currentState->width; i++) {
//             flip_entry(currentState->g, 0, i, currentState->width);
//             currentState->clique_count = CliqueCountRow(currentState->g, currentState->width);
//             if (currentState->clique_count < bestState->clique_count) {
//                 update_best_clique();
//             }
//             // Flip it back, and check the next iteration
//             flip_entry(currentState->g, 0, i, currentState->width);
//
//             // If timeout,
//             gettimeofday(&now, NULL);
//             timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
//             if (timediff > update_interval) {
//                 gettimeofday(&begin, NULL);
//                 send_counterexample(alg_name, currentState->g, currentState->width, currentState->clique_count, currentState->num_calculations);
//
//                 int updated = get_next_work();
//
//                 // Restart loop if we got a new work item
//                 if (updated)
//                     i = 0;
//             }
//         }
//
//         update_current_clique();
//
//         if (currentState->clique_count == 0) {
//             send_counterexample(alg_name, currentState->g, currentState->width, currentState->clique_count, currentState->num_calculations);
//             get_next_work();
//         } else {
//             gettimeofday(&now, NULL);
//             timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
//             if (timediff > update_interval) {
//                 gettimeofday(&begin, NULL);
//                 send_counterexample(alg_name, currentState->g, currentState->width, currentState->clique_count, currentState->num_calculations);
//                 get_next_work();
//             }
//         }
//     }
// }

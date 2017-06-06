#ifndef UTILS_C
#define UTILS_C

#include <math.h>

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

// Globals
#include "globals.h"
#include "queue.c"

int random_int(int min, int max);
void print_counterexample(int* g, int m);
void update_best_clique();
void flip_entry(int* matrix, int row, int column, int m);
void update_current_clique();
void reset_state();
void generate_random_uuid();

int random_int(int min, int max) { return min + rand() % (max - min); }

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

void update_best_clique() {
    free(bestState->g);
    bestState->g = NULL;
    bestState->g = (int*)malloc(currentState->width * currentState->width * sizeof(int));
    int i;
    for (i = 0; i < currentState->width * currentState->width; i++) {
        bestState->g[i] = currentState->g[i];
    }
    bestState->width = currentState->width;
    bestState->clique_count = currentState->clique_count;
    printf("Best clique is now %d - %d \n", bestState->width, bestState->clique_count);
}

void flip_entry(int* matrix, int row, int column, int m) {
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

void update_current_clique() {
    free(currentState->g);
    currentState->g = NULL;
    currentState->g = (int*)malloc(bestState->width * bestState->width * sizeof(int));
    int i;
    for (i = 0; i < bestState->width * bestState->width; i++) {
        currentState->g[i] = bestState->g[i];
    }
    currentState->width = bestState->width;
    currentState->clique_count = bestState->clique_count;
    printf("Current clique is now %d - %d \n", currentState->width, currentState->clique_count);
}

void reset_state() {
    currentState->clique_count = INT_MAX;
    currentState->width = 0;
    currentState->num_calculations = 0;
    bestState->clique_count = INT_MAX;
    bestState->width = 0;
    bestState->num_calculations = 0;

    free(currentState->g);
    free(bestState->g);
    currentState->g = NULL;
    bestState->g = NULL;
}

void generate_random_uuid() {
    int t = 0;
    char* szTemp = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    char* szHex = "0123456789ABCDEF-";
    int nLen = strlen(szTemp);

    for (t = 0; t < nLen + 1; t++) {
        int r = rand() % 16;
        char c = ' ';

        switch (szTemp[t]) {
        case 'x': {
            c = szHex[r];
        } break;
        case 'y': {
            c = szHex[(r & 0x03) | 0x08];
        } break;
        case '-': {
            c = '-';
        } break;
        case '4': {
            c = '4';
        } break;
        }

        uuid_str[t] = (t < nLen) ? c : 0x00;
    }
}

#endif

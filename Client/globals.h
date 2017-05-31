#ifndef GLOBALS_H
#define GLOBALS_H

typedef struct PartyState {
    int width;
    int clique_count;
    int* g;
    int num_calculations;
} PartyState;

extern char* server_ip;
extern int server_port;
extern int update_interval;
extern char uuid_str[37];
extern struct PartyState* currentState;
extern struct PartyState* bestState;

#endif

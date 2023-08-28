#include "laws.h"

#define NUM_RULES 128
#define NUM_PLACES 4

static Law laws[NUM_PLACES][NUM_RULES];
static int next_law[NUM_PLACES];

void register_law(Law l) {
    laws[l.place][next_law[l.place]++] = l;
}
void apply_laws(int place, Ball *b) {
    for (int i = 0; i < next_law[place]; i++) {
        laws[place][i].f(b);
    }
}
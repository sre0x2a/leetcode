#include <stdio.h>

#define LEN 512

enum {
    SPLIT = 256,
    MATCH,
    DIGIT
};

typedef struct State {
    int c;
    struct State *out;
    struct State *out1;
    int id;
} State;

typedef struct Set {
    State *s[LEN];
    int n;
} Set;

Set l1;
Set l2;
int id;

// (-|\+)?(\d+|\d+\.|\d+\.\d+|\.\d+)((e|E)(-|\+)?\d+)?
State machine[] = {
    // (-|\+)?
    { .c = SPLIT, .out = machine + 1, .out1 = machine + 4, .id = -1 }, // 0
    { .c = SPLIT, .out = machine + 2, .out1 = machine + 3, .id = -1 }, // 1
    { .c = 0x2d, .out = 0, .out1 = machine + 4, .id = -1 }, // 2
    { .c = 0x2b, .out = 0, .out1 = machine + 4, .id = -1 }, // 3

    // \d+
    { .c = SPLIT, .out = machine + 5, .out1 = machine + 7, .id = -1 }, // 4
    { .c = DIGIT, .out = 0, .out1 = machine + 6, .id = -1 }, // 5
    { .c = SPLIT, .out = machine + 5, .out1 = machine + 20, .id = -1 }, // 6

    // \d+\.
    { .c = SPLIT, .out = machine + 8, .out1 = machine + 11, .id = -1 }, // 7
    { .c = DIGIT, .out = 0, .out1 = machine + 9, .id = -1 }, // 8
    { .c = SPLIT, .out = machine + 8, .out1 = machine + 10, .id = -1 }, // 9
    { .c = 0x2e, .out = 0, .out1 = machine + 20, .id = -1 }, // 10

    // \d+\.\d+
    { .c = SPLIT, .out = machine + 12, .out1 = machine + 17, .id = -1 }, // 11
    { .c = DIGIT, .out = 0, .out1 = machine + 13, .id = -1 }, // 12
    { .c = SPLIT, .out = machine + 12, .out1 = machine + 14, .id = -1 }, // 13
    { .c = 0x2e, .out = 0, .out1 = machine + 15, .id = -1 }, // 14
    { .c = DIGIT, .out = 0, .out1 = machine + 16, .id = -1 }, // 15
    { .c = SPLIT, .out = machine + 15, .out1 = machine + 20, .id = -1 }, // 16

    // \.\d+
    { .c = 0x2e, .out = 0, .out1 = machine + 18, .id = -1 }, // 17
    { .c = DIGIT, .out = 0, .out1 = machine + 19, .id = -1 }, // 18
    { .c = SPLIT, .out = machine + 18, .out1 = machine + 20, .id = -1 }, // 19

    // (e|E)
    { .c = SPLIT, .out = machine + 21, .out1 = machine + 30, .id = -1 }, // 20
    { .c = SPLIT, .out = machine + 22, .out1 = machine + 23, .id = -1 }, // 21
    { .c = 0x65, .out = 0, .out1 = machine + 24, .id = -1 }, // 22
    { .c = 0x45, .out = 0, .out1 = machine + 24, .id = -1 }, // 23

    // (-|\+)?
    { .c = SPLIT, .out = machine + 25, .out1 = machine + 28, .id = -1 }, // 24
    { .c = SPLIT, .out = machine + 26, .out1 = machine + 27, .id = -1 }, // 25
    { .c = 0x2d, .out = 0, .out1 = machine + 28, .id = -1 }, // 26
    { .c = 0x2b, .out = 0, .out1 = machine + 28, .id = -1 }, // 27

    // \d+
    { .c = DIGIT, .out = 0, .out1 = machine + 29, .id = -1 }, // 28
    { .c = SPLIT, .out = machine + 28, .out1 = machine + 30, .id = -1 }, // 29

    { .c = MATCH, .out = 0, .out1 = 0, .id = -1 }, // 30
};

State *matchState = machine + 30;

void readLine(char *);

int find(char *);
Set *startSet(State *, Set *);
void step(Set *, int, Set *);
int match(Set *);
void addState(Set *, State *);

int main(void) {
    char s[LEN];
    readLine(s);
    printf("%d\n", find(s));
    return 0;
}

void readLine(char *s) {
    while ((*s = getchar()) != EOF && *s != 0xa)
        ++s;
    *s = 0;
}

int find(char *s) {
    Set *clist, *nlist, *t;

    clist = startSet(machine, &l1);
    nlist = &l2;

    while (*s) {
        step(clist, *s, nlist);
        t = clist;
        clist = nlist;
        nlist = t;
        ++s;
    }

    return match(clist);
}

int match(Set *s) {
    int i;

    for (i = 0; i < s->n; ++i) {
        if (s->s[i] == matchState)
            return 1;
    }

    return 0;
}

void addState(Set *set, State *state) {
    if (!state || state->id == id)
        return;

    state->id = id;

    if (state->c == SPLIT) {
        addState(set, state->out);
        addState(set, state->out1);
        return;
    }

    set->s[set->n] = state;
    ++set->n;
}

Set *startSet(State *state, Set *set) {
    ++id;
    set->n = 0;
    addState(set, state);
    return set;
}

void step(Set *clist, int c, Set *nlist) {
    int i;
    State *state;

    ++id;
    nlist->n = 0;

    for (i = 0; i < clist->n; ++i) {
        state = clist->s[i];
        if (state->c == c || (state->c == DIGIT && c >= 0x30 && c < 0x3a))
            addState(nlist, state->out1);
    }
}


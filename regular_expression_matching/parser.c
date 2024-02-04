#include <stdio.h>
#include <stdlib.h>

#define LEN 20

enum {
    SPLIT = 256,
    MATCH,
    ANY
};

typedef struct State {
    int c;
    struct State *out;
    struct State *out1;
    int id;
} State;

typedef struct Set {
    State *s[2*LEN];
    int n;
} Set;

State matchState;
Set l1;
Set l2;
int id;

void readLine(char *);

State *newState(int);
State *compile(char *);
void release(State *);

int isMatch(char *, char *);
int find(State *, char *);
Set *startSet(State *, Set *);
void step(Set *, int, Set *);
int match(Set *);
void addState(Set *, State *);

int main(void) {
    char s[LEN+1], p[LEN+1];

    readLine(s);
    readLine(p);

    puts(isMatch(s, p) ? "true" : "false");

    return 0;
}

void readLine(char *s) {
    while ((*s = getchar()) != EOF && *s != 0xa)
        ++s;
    *s = 0;
}

State *newState(int c) {
    State *n;

    n = malloc(sizeof(State));

    n->c = c;
    n->out = 0;
    n->out1 = 0;
    n->id = -1;

    return n;
}

State *compile(char *p) {
    State *machine, *curr, *prev;

    machine = 0;
    prev = 0;

    while (*p) {
        if (*p != 0x2a) {
            curr = newState(*p == 0x2e ? ANY : *p);
            if (prev)
                prev->out1 = curr;
            prev = curr;
        }
        else {
            curr = newState(prev->c);
            curr->out1 = prev;
            prev->c = SPLIT;
            prev->out = curr;
        }

        if (!machine)
            machine = curr;

        ++p;
    }

    matchState.c = MATCH;
    matchState.out = 0;
    matchState.out1 = 0;
    matchState.id = -1;

    id = -1;

    if (curr == prev)
        curr->out1 = &matchState;
    else
        prev->out1 = &matchState;

    return machine;
}

void release(State *p) {
    State *curr, *prev;

    curr = p;

    while (curr && curr != &matchState) {
        prev = curr;
        curr = curr->out1;
        free(prev);
    }
}

int isMatch(char* s, char* p) {
    State *machine;
    int ret;

    machine = compile(p);
    ret = find(machine, s);
    release(machine);

    return ret;
}

int find(State *start, char *s) {
    Set *clist, *nlist, *t;

    clist = startSet(start, &l1);
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
        if (s->s[i] == &matchState)
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
        if (state->c == c || state->c == ANY)
            addState(nlist, state->out1);
    }
}


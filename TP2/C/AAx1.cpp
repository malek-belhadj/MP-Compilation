#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_RULES 10
#define MAX_SYMBOLS 40
#define MAX_STATES 20
#define MAX_ACTION 10

// Terminal and non-terminal symbols
char terminals[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g','h', 'i', 'j', 'k', 'l', 'm', 'n','o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z','+', '*', '(', ')', '$'};
char non_terminals[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z','S', 'E', 'T', 'F'};

#define NUM_TERMINALS (sizeof(terminals) / sizeof(terminals[0]))
#define NUM_NON_TERMINALS (sizeof(non_terminals) / sizeof(non_terminals[0]))

// Grammar rule
typedef struct {
    char lhs;
    char rhs[MAX_SYMBOLS];
    int length;
} Rule;

// LR(1) item
typedef struct {
    int rule_index;
    int dot_position;
    char lookahead;
} LR1Item;

// LR(1) state
typedef struct {
    LR1Item items[MAX_RULES * MAX_RULES];  // Allow for expanded closures
    int num_items;
} LR1State;

// LR(1) parsing table
typedef struct {
    char action[MAX_STATES][MAX_SYMBOLS][MAX_ACTION];
    int goto_table[MAX_STATES][MAX_SYMBOLS];
} LR1Table;

// Grammar rules
Rule grammar[MAX_RULES] = {
    {'S', "E", 1},
    {'E', "E+T", 3},
    {'E', "T", 1},
    {'T', "T*F", 3},
    {'T', "F", 1},
    {'F', "(E)", 3},
    {'F', "d", 1}
};
int num_rules = 7;

// Checks
bool is_non_terminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

int symbol_index(char symbol) {
    for (int i = 0; i < NUM_TERMINALS; i++) {
        if (terminals[i] == symbol) return i;
    }
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        if (non_terminals[i] == symbol) return NUM_TERMINALS + i;
    }
    return -1;
}

// Closure
void closure(LR1State *state) {
    bool added = true;
    while (added) {
        added = false;
        for (int i = 0; i < state->num_items; i++) {
            LR1Item item = state->items[i];
            if (item.dot_position >= grammar[item.rule_index].length)
                continue;

            char symbol = grammar[item.rule_index].rhs[item.dot_position];
            if (is_non_terminal(symbol)) {
                for (int j = 0; j < num_rules; j++) {
                    if (grammar[j].lhs == symbol) {
                        bool exists = false;
                        for (int k = 0; k < state->num_items; k++) {
                            LR1Item current = state->items[k];
                            if (current.rule_index == j &&
                                current.dot_position == 0 &&
                                current.lookahead == item.lookahead) {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists && state->num_items < MAX_RULES * MAX_RULES) {
                            state->items[state->num_items++] = (LR1Item){j, 0, item.lookahead};
                            added = true;
                        }
                    }
                }
            }
        }
    }
}

// Goto
LR1State goto_state(LR1State state, char symbol) {
    LR1State new_state = {0};
    for (int i = 0; i < state.num_items; i++) {
        LR1Item item = state.items[i];
        if (item.dot_position < grammar[item.rule_index].length &&
            grammar[item.rule_index].rhs[item.dot_position] == symbol) {
            new_state.items[new_state.num_items++] = (LR1Item){
                item.rule_index,
                item.dot_position + 1,
                item.lookahead
            };
        }
    }
    closure(&new_state);
    return new_state;
}

// Equality check
bool states_equal(LR1State *a, LR1State *b) {
    if (a->num_items != b->num_items) return false;
    bool found = false;
    for (int i = 0; i < a->num_items; i++) {
       found = false;
        for (int j = 0; j < b->num_items; j++) {
            if (a->items[i].rule_index == b->items[j].rule_index &&
                a->items[i].dot_position == b->items[j].dot_position &&
                a->items[i].lookahead == b->items[j].lookahead) {
                found = true;
                break;
            }
        }
        if (!found) return found;
    }
    return true;
}

// Build states
void build_lr1_states(LR1State *states, int *num_states) {
    LR1State initial = {0};
    initial.items[0] = (LR1Item){0, 0, '$'};
    initial.num_items = 1;
    closure(&initial);
    states[(*num_states)++] = initial;

    for (int i = 0; i < *num_states; i++) {
        for (int s = 0; s < NUM_TERMINALS + NUM_NON_TERMINALS; s++) {
            char symbol = (s < NUM_TERMINALS) ? terminals[s] : non_terminals[s - NUM_TERMINALS];
            LR1State next = goto_state(states[i], symbol);
            if (next.num_items > 0) {
                bool exists = false;
                for (int j = 0; j < *num_states; j++) {
                    if (states_equal(&next, &states[j])) {
                        exists = true;
                        break;
                    }
                }
                if (!exists && *num_states < MAX_STATES) {
                    states[(*num_states)++] = next;
                }
            }
        }
    }
}

// Build table
void build_lr1_table(LR1State *states, int num_states, LR1Table *table) {
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < states[i].num_items; j++) {
            LR1Item item = states[i].items[j];
            if (item.dot_position < grammar[item.rule_index].length) {
                char symbol = grammar[item.rule_index].rhs[item.dot_position];
                int index = symbol_index(symbol);
                if (index != -1 && index < MAX_SYMBOLS) {
                    strcpy(table->action[i][index], "shift");
                }
            } else if (item.rule_index == 0 && item.lookahead == '$') {
                int index = symbol_index('$');
                if (index != -1 && index < MAX_SYMBOLS) {
                    strcpy(table->action[i][index], "accept");
                }
            } else {
                int index = symbol_index(item.lookahead);
                if (index != -1 && index < MAX_SYMBOLS) {
                    strcpy(table->action[i][index], "reduce");
                }
            }
        }
    }
}

// Print table
void print_lr1_table(LR1Table *table, int num_states) {
    printf("\n=== LR(1) Table ===\n");
    for (int i = 0; i < num_states; i++) {
        printf("State %d:\n", i);
        for (int j = 0; j < NUM_TERMINALS; j++) {
            if (strlen(table->action[i][j]) > 0) {
                printf("  %c : %s\n", terminals[j], table->action[i][j]);
            }
        }
    }
}

// Main
int main() {
    LR1State states[MAX_STATES] = {0};
    int num_states = 0;

    build_lr1_states(states, &num_states);

    LR1Table table = {0};
    build_lr1_table(states, num_states, &table);

    print_lr1_table(&table, num_states);
    return 0;
}

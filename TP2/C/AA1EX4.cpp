#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_RULES 10
#define MAX_SYMBOLS 20
#define MAX_STATES 20

// Structure pour représenter une règle de grammaire
typedef struct {
    char lhs; // Non-terminal à gauche
    char rhs[MAX_SYMBOLS]; // Partie droite de la règle
    int length; // Longueur de la partie droite
} Rule;

// Structure pour représenter un item LR(1)
typedef struct {
    int rule_index; // Index de la règle
    int dot_position; // Position du point dans la règle
    char lookahead; // Symbole de lookahead
} LR1Item;

// Structure pour représenter un état LR(1)
typedef struct {
    LR1Item items[MAX_RULES];
    int num_items;
} LR1State;

// Structure pour représenter la table LR(1)
typedef struct {
    char action[MAX_STATES][MAX_SYMBOLS][10]; // Actions (shift, reduce, accept)
    int goto_table[MAX_STATES][MAX_SYMBOLS]; // Goto (états suivants)
} LR1Table;

int goto_map[MAX_STATES][MAX_SYMBOLS]; // Goto map for states

// Grammaire de départ
Rule grammar[MAX_RULES] = {
    {'S', "aAB", 3},
    {'S', "bSS", 3},
    {'A', "CC", 2},
    {'A', "f", 1},
    {'C', "cC", 2},    
    {'C', "d", 1},
    {'B', "bB", 2},
    {'B', "e" , 1}
};
int num_rules = 8;

// Terminals and Non-terminals
char terminals[] = {'a', 'b', 'c', 'd', 'e' , '$'};
char non_terminals[] = {'S', 'A', 'B', 'S', 'C'};

const int NUM_TERMINALS = sizeof(terminals)/sizeof(char);
const int NUM_NON_TERMINALS = sizeof(non_terminals)/sizeof(char);

// Check if character is a non-terminal
bool is_non_terminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

// Get index for symbol
int get_symbol_index(char symbol) {
    for (int i = 0; i < NUM_TERMINALS; i++) {
        if (terminals[i] == symbol) return i;
    }
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        if (non_terminals[i] == symbol) return NUM_TERMINALS + i;
    }
    return -1;
}

// Get symbol by index
char get_symbol_by_index(int index) {
    if (index < NUM_TERMINALS) return terminals[index];
    else return non_terminals[index - NUM_TERMINALS];
}

// Closure function
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
                        if (!exists && state->num_items < MAX_RULES) {
                            state->items[state->num_items++] = (LR1Item){j, 0, item.lookahead};
                            added = true;
                        }
                    }
                }
            }
        }
    }
}

// Goto function
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

// Compare states
bool states_equal(LR1State *a, LR1State *b) {
    if (a->num_items != b->num_items) return false;
    for (int i = 0; i < a->num_items; i++) {
        bool found = false;
        for (int j = 0; j < b->num_items; j++) {
            if (a->items[i].rule_index == b->items[j].rule_index &&
                a->items[i].dot_position == b->items[j].dot_position &&
                a->items[i].lookahead == b->items[j].lookahead) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

// Build LR(1) states
void build_lr1_states(LR1State *states, int *num_states) {
    LR1State initial_state = {0};
    initial_state.items[0] = (LR1Item){0, 0, '$'};
    initial_state.num_items = 1;
    closure(&initial_state);
    states[(*num_states)++] = initial_state;

    for (int i = 0; i < *num_states; i++) {
        for (int s = 0; s < NUM_TERMINALS + NUM_NON_TERMINALS; s++) {
            char symbol = get_symbol_by_index(s);
            LR1State next = goto_state(states[i], symbol);
            if (next.num_items > 0) {
                bool exists = false;
                for (int j = 0; j < *num_states; j++) {
                    if (states_equal(&next, &states[j])) {
                        exists = true;
                        goto_map[i][s] = j;
                        break;
                    }
                }
                if (!exists && *num_states < MAX_STATES) {
                    states[(*num_states)++] = next;
                    goto_map[i][s] = *num_states - 1;
                }
            }
        }
    }
}

// Build LR(1) table
void build_lr1_table(LR1State *states, int num_states, LR1Table *table) {
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < states[i].num_items; j++) {
            LR1Item item = states[i].items[j];
            if (item.dot_position < grammar[item.rule_index].length) {
                char next_symbol = grammar[item.rule_index].rhs[item.dot_position];
                int idx = get_symbol_index(next_symbol);
                if (idx != -1) {
                    if (idx < NUM_TERMINALS) {
                        sprintf(table->action[i][idx], "s%d", goto_map[i][idx]);
                    } else {
                        table->goto_table[i][idx - NUM_TERMINALS] = goto_map[i][idx];
                    }
                }
            } else if ((item.rule_index == 0) || (item.rule_index == 1) && item.lookahead == '$') {
                int idx = get_symbol_index('$');
                if (idx != -1) strcpy(table->action[i][idx], "acc");
            } else {
                int idx = get_symbol_index(item.lookahead);
                if (idx != -1) {
                    sprintf(table->action[i][idx], "r%d", item.rule_index);
                }
            }
        }
    }
}

// Main function
// Main function
// int main() {
//     LR1State states[MAX_STATES];
//     int num_states = 0;
//     build_lr1_states(states, &num_states);

//     LR1Table table = {0};
//     build_lr1_table(states, num_states, &table);

//     // Display LR(1) table
//     printf("LR(1) Table:\n");
//     for (int i = 0; i < num_states; i++) {
//         printf("State %d:\n", i);
//         for (int j = 0; j < NUM_TERMINALS; j++) {
           
//             if (strlen(table.action[i][j]) > 0) {
//                 printf("  %c: %s\n", get_symbol_by_index(j), table.action[i][j]);
            
//             }
//         }
//         for (int j = 0; j < NUM_NON_TERMINALS; j++) {
//             if ((table.goto_table[i][j]) != 0) {
//                 printf("  %c: %s\n", get_symbol_by_index(j + NUM_TERMINALS), table.goto_table[i][j]);
//             }
//         }
            
//     }
//         // for (int j = 0; j < NUM_NON_TERMINALS; j++) {
//         //     if ((table.goto_table[i][j]) != -1) {
//         //         printf("  %c: %s\n", get_symbol_by_index(j), table.goto_table[i][j]);
//         //     }
//         // }
        
//         printf("\n");

//     return 0;
// }


//pour une affichage conviviale
int main() {
    LR1State states[MAX_STATES];
    int num_states = 0;
    build_lr1_states(states, &num_states);
    LR1Table table = {0};
    build_lr1_table(states, num_states, &table);
    // Display LR(1) table
    printf("LR(1) Table:\n");
    printf("State\t");
    for (int i = 0; i < NUM_TERMINALS; i++) {
        printf("%c\t", terminals[i]);
    }
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        printf("%c\t", non_terminals[i]);
    }
    printf("\n");

    for (int i = 0; i < num_states; i++) {
        printf("%d\t", i);
        // Print action table
        for (int j = 0; j < NUM_TERMINALS; j++) {
            if (strlen(table.action[i][j]) > 0) {
                printf("%s\t", table.action[i][j]);
            } else {
                printf("\t");
            }
        }
        // Print goto table
        for (int j = 0; j < NUM_NON_TERMINALS; j++) {
            if (table.goto_table[i][j] != NULL) {
                printf("%d\t", table.goto_table[i][j]);
            } else {
                printf("\t");
            }
        }
        printf("\n");
    }

    return 0;
}
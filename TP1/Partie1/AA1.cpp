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

// Grammaire de départ
Rule grammar[MAX_RULES] = {
    {'S', "E", 1}, // S -> E (règle augmentée)
    {'E', "E+T", 3}, // E -> E + T
    {'E', "T", 1}, // E -> T
    {'T', "T*F", 3}, // T -> T * F
    {'T', "F", 1}, // T -> F
    {'F', "(E)", 3}, // F -> (E)
    {'F', "id", 2} // F -> id
};
int num_rules = 7;

// Fonction pour calculer la fermeture d'un ensemble d'items LR(1)
void closure(LR1State *state) {
    // Implémentation simplifiée (à compléter)
    // Pour chaque item dans l'état, ajouter les items dérivés
}

// Fonction pour calculer Goto pour un état et un symbole donné
LR1State goto_state(LR1State state, char symbol) {
    LR1State new_state = {0};
    // Implémentation simplifiée (à compléter)
    // Pour chaque item dans l'état, si le symbole après le point est `symbol`, ajouter un nouvel item
    return new_state;
}

// Fonction pour construire les états LR(1)
void build_lr1_states(LR1State *states, int *num_states) {
    // État initial
    LR1State initial_state = {0};
    initial_state.items[0] = (LR1Item){0, 0, '$'}; // S' -> .E, $
    initial_state.num_items = 1;
    closure(&initial_state);
    states[(*num_states)++] = initial_state;

    // Construire les états suivants
    for (int i = 0; i < *num_states; i++) {
        for (char symbol = 'a'; symbol <= 'z'; symbol++) {
            LR1State new_state = goto_state(states[i], symbol);
            if (new_state.num_items > 0) {
                states[(*num_states)++] = new_state;
            }
        }
    }
}

// Fonction pour construire la table LR(1)
void build_lr1_table(LR1State *states, int num_states, LR1Table *table) {
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < states[i].num_items; j++) {
            LR1Item item = states[i].items[j];
            if (item.dot_position < grammar[item.rule_index].length) {
                // Décalage
                char next_symbol = grammar[item.rule_index].rhs[item.dot_position];
                strcpy(table->action[i][next_symbol], "shift");
            } else if (item.rule_index == 0 && item.lookahead == '$') {
                // Acceptation
                strcpy(table->action[i]['$'], "accept");
            } else {
                // Réduction
                strcpy(table->action[i][item.lookahead], "reduce");
            }
        }
    }
}

// Fonction principale
int main() {
    LR1State states[MAX_STATES];
    int num_states = 0;
    build_lr1_states(states, &num_states);

    LR1Table table = {0};
    build_lr1_table(states, num_states, &table);

    // Afficher la table LR(1)
    printf("LR(1) Table:\n");
    for (int i = 0; i < num_states; i++) {
        printf("State %d:\n", i);
        for (char symbol = 'a'; symbol <= 'z'; symbol++) {
            if (strlen(table.action[i][symbol]) > 0) {
                printf("  %c: %s\n", symbol, table.action[i][symbol]);
            }
        }
    }

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STATES 10
#define MAX_SYMBOLS 10

// Définir les symboles terminaux et non-terminaux
enum Symbol {
    ID, PLUS, MULT, LPAREN, RPAREN, END, // Terminaux
    E, T, F, S_PRIME // Non-terminaux
};

// Définir les actions possibles dans la table LR(1)
enum ActionType {
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR
};

// Structure pour représenter une action dans la table LR(1)
typedef struct {
    enum ActionType type;
    int value; // État pour SHIFT, numéro de règle pour REDUCE
} Action;

// Table LR(1)
Action action_table[MAX_STATES][MAX_SYMBOLS];
int goto_table[MAX_STATES][MAX_SYMBOLS];

// Règles de la grammaire
typedef struct {
    int lhs; // Non-terminal à gauche
    int rhs_len; // Longueur de la partie droite
    int rhs[MAX_SYMBOLS]; // Symboles à droite
} Rule;

Rule rules[] = {
    {S_PRIME, 1, {E}}, // S' -> E
    {E, 3, {E, PLUS, T}}, // E -> E + T
    {E, 1, {T}}, // E -> T
    {T, 3, {T, MULT, F}}, // T -> T * F
    {T, 1, {F}}, // T -> F
    {F, 3, {LPAREN, E, RPAREN}}, // F -> ( E )
    {F, 1, {ID}} // F -> id
};

// Initialiser la table LR(1) (simplifiée pour cet exemple)
void initialize_tables() {
    // Exemple de table LR(1) pour la grammaire donnée
    action_table[0][ID] = (Action){SHIFT, 5};
    action_table[0][LPAREN] = (Action){SHIFT, 4};

    action_table[1][PLUS] = (Action){SHIFT, 6};
    action_table[1][END] = (Action){ACCEPT, 0};

    action_table[2][PLUS] = (Action){REDUCE, 2};
    action_table[2][MULT] = (Action){SHIFT, 7};
    action_table[2][RPAREN] = (Action){REDUCE, 2};
    action_table[2][END] = (Action){REDUCE, 2};

    action_table[3][PLUS] = (Action){REDUCE, 4};
    action_table[3][MULT] = (Action){REDUCE, 4};
    action_table[3][RPAREN] = (Action){REDUCE, 4};
    action_table[3][END] = (Action){REDUCE, 4};

    action_table[4][ID] = (Action){SHIFT, 5};
    action_table[4][LPAREN] = (Action){SHIFT, 4};

    action_table[5][PLUS] = (Action){REDUCE, 6};
    action_table[5][MULT] = (Action){REDUCE, 6};
    action_table[5][RPAREN] = (Action){REDUCE, 6};
    action_table[5][END] = (Action){REDUCE, 6};

    action_table[6][ID] = (Action){SHIFT, 5};
    action_table[6][LPAREN] = (Action){SHIFT, 4};

    action_table[7][ID] = (Action){SHIFT, 5};
    action_table[7][LPAREN] = (Action){SHIFT, 4};

    action_table[8][PLUS] = (Action){SHIFT, 6};
    action_table[8][RPAREN] = (Action){SHIFT, 11};

    action_table[9][PLUS] = (Action){REDUCE, 1};
    action_table[9][MULT] = (Action){SHIFT, 7};
    action_table[9][RPAREN] = (Action){REDUCE, 1};
    action_table[9][END] = (Action){REDUCE, 1};

    action_table[10][PLUS] = (Action){REDUCE, 3};
    action_table[10][MULT] = (Action){REDUCE, 3};
    action_table[10][RPAREN] = (Action){REDUCE, 3};
    action_table[10][END] = (Action){REDUCE, 3};

    action_table[11][PLUS] = (Action){REDUCE, 5};
    action_table[11][MULT] = (Action){REDUCE, 5};
    action_table[11][RPAREN] = (Action){REDUCE, 5};
    action_table[11][END] = (Action){REDUCE, 5};

    // Goto table
    goto_table[0][E] = 1;
    goto_table[0][T] = 2;
    goto_table[0][F] = 3;

    goto_table[4][E] = 8;
    goto_table[4][T] = 2;
    goto_table[4][F] = 3;

    goto_table[6][T] = 9;
    goto_table[6][F] = 3;

    goto_table[7][F] = 10;
}

// Fonction pour analyser un mot en utilisant la table LR(1)
bool parse_input(char *input) {
    int stack[MAX_STATES * 2]; // Pile pour les états et symboles
    int top = 0;
    stack[top++] = 0; // État initial

    int input_pos = 0;
    while (true) {
        int state = stack[top - 1];
        int symbol = input[input_pos];

        Action action = action_table[state][symbol];
        switch (action.type) {
            case SHIFT:
                stack[top++] = symbol;
                stack[top++] = action.value;
                input_pos++;
                break;
            case REDUCE: {
                Rule rule = rules[action.value];
                top -= 2 * rule.rhs_len;
                int new_state = stack[top - 1];
                stack[top++] = rule.lhs;
                stack[top++] = goto_table[new_state][rule.lhs];
                break;
            }
            case ACCEPT:
                return true;
            case ERROR:
                return false;
        }
    }
}

// Fonction principale
int main() {
    initialize_tables();

    char input[] = {ID, PLUS, ID, MULT, ID, END}; // Exemple : id + id * id
    if (parse_input(input)) {
        printf("Parsing successful! The input is valid.\n");
    } else {
        printf("Parsing failed! The input is invalid.\n");
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_RULES 10
#define MAX_SYMBOLS 20
#define MAX_NON_TERMINALS 10
#define MAX_TERMINALS 10

// Structure pour représenter une règle de grammaire
typedef struct {
    char non_terminal;
    char production[MAX_SYMBOLS];
} Rule;

// Structure pour représenter une grammaire
typedef struct {
    Rule rules[MAX_RULES];
    int num_rules;
    char non_terminals[MAX_NON_TERMINALS];
    char terminals[MAX_TERMINALS];
    int num_non_terminals;
    int num_terminals;
} Grammar;

// Structure pour stocker les ensembles "First" et "Follow"
typedef struct {
    char non_terminal;
    char first[MAX_SYMBOLS];
    char follow[MAX_SYMBOLS];
    int num_first;
    int num_follow;
} FirstFollow;

// Structure pour représenter la table LL(1)
typedef struct {
    char non_terminal;
    char terminal;
    char production[MAX_SYMBOLS];
} LL1TableEntry;

// Fonction pour calculer l'ensemble "First"
void calculate_first(Grammar g, FirstFollow *first_follow) {
    // Initialisation des ensembles "First"
    for (int i = 0; i < g.num_non_terminals; i++) {
        first_follow[i].non_terminal = g.non_terminals[i];
        first_follow[i].num_first = 0;
    }

    // Calcul des ensembles "First"
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < g.num_rules; i++) {
            char non_terminal = g.rules[i].non_terminal;
            char first_symbol = g.rules[i].production[0];

            // Si le premier symbole est un terminal, l'ajouter à "First"
            if (first_symbol >= 'a' && first_symbol <= 'z') {
                for (int j = 0; j < g.num_non_terminals; j++) {
                    if (first_follow[j].non_terminal == non_terminal) {
                        bool exists = false;
                        for (int k = 0; k < first_follow[j].num_first; k++) {
                            if (first_follow[j].first[k] == first_symbol) {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists) {
                            first_follow[j].first[first_follow[j].num_first++] = first_symbol;
                            changed = true;
                        }
                        break;
                    }
                }
            }
            // Si le premier symbole est un non-terminal, ajouter son "First"
            else if (first_symbol >= 'A' && first_symbol <= 'Z') {
                for (int j = 0; j < g.num_non_terminals; j++) {
                    if (first_follow[j].non_terminal == first_symbol) {
                        for (int k = 0; k < first_follow[j].num_first; k++) {
                            for (int l = 0; l < g.num_non_terminals; l++) {
                                if (first_follow[l].non_terminal == non_terminal) {
                                    bool exists = false;
                                    for (int m = 0; m < first_follow[l].num_first; m++) {
                                        if (first_follow[l].first[m] == first_follow[j].first[k]) {
                                            exists = true;
                                            break;
                                        }
                                    }
                                    if (!exists) {
                                        first_follow[l].first[first_follow[l].num_first++] = first_follow[j].first[k];
                                        changed = true;
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    } while (changed);
}

// Fonction pour calculer l'ensemble "Follow"
void calculate_follow(Grammar g, FirstFollow *first_follow) {
    // Initialisation des ensembles "Follow"
    for (int i = 0; i < g.num_non_terminals; i++) {
        first_follow[i].num_follow = 0;
        if (g.non_terminals[i] == g.rules[0].non_terminal) {
            first_follow[i].follow[first_follow[i].num_follow++] = '$'; // Symbole de fin de chaîne
        }
    }

    // Calcul des ensembles "Follow"
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < g.num_rules; i++) {
            char non_terminal = g.rules[i].non_terminal;
            char *production = g.rules[i].production;

            for (int j = 0; j < strlen(production); j++) {
                if (production[j] >= 'A' && production[j] <= 'Z') { // Si c'est un non-terminal
                    for (int k = 0; k < g.num_non_terminals; k++) {
                        if (first_follow[k].non_terminal == production[j]) {
                            if (j + 1 < strlen(production)) {
                                char next_symbol = production[j + 1];
                                if (next_symbol >= 'a' && next_symbol <= 'z') { // Terminal
                                    bool exists = false;
                                    for (int l = 0; l < first_follow[k].num_follow; l++) {
                                        if (first_follow[k].follow[l] == next_symbol) {
                                            exists = true;
                                            break;
                                        }
                                    }
                                    if (!exists) {
                                        first_follow[k].follow[first_follow[k].num_follow++] = next_symbol;
                                        changed = true;
                                    }
                                } else if (next_symbol >= 'A' && next_symbol <= 'Z') { // Non-terminal
                                    for (int l = 0; l < g.num_non_terminals; l++) {
                                        if (first_follow[l].non_terminal == next_symbol) {
                                            for (int m = 0; m < first_follow[l].num_first; m++) {
                                                bool exists = false;
                                                for (int n = 0; n < first_follow[k].num_follow; n++) {
                                                    if (first_follow[k].follow[n] == first_follow[l].first[m]) {
                                                        exists = true;
                                                        break;
                                                    }
                                                }
                                                if (!exists) {
                                                    first_follow[k].follow[first_follow[k].num_follow++] = first_follow[l].first[m];
                                                    changed = true;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                            } else { // Si c'est la fin de la production
                                for (int l = 0; l < g.num_non_terminals; l++) {
                                    if (first_follow[l].non_terminal == non_terminal) {
                                        for (int m = 0; m < first_follow[l].num_follow; m++) {
                                            bool exists = false;
                                            for (int n = 0; n < first_follow[k].num_follow; n++) {
                                                if (first_follow[k].follow[n] == first_follow[l].follow[m]) {
                                                    exists = true;
                                                    break;
                                                }
                                            }
                                            if (!exists) {
                                                first_follow[k].follow[first_follow[k].num_follow++] = first_follow[l].follow[m];
                                                changed = true;
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    } while (changed);
}

// Fonction pour construire la table LL(1)
void build_ll1_table(Grammar g, FirstFollow *first_follow, LL1TableEntry *ll1_table, int *num_entries) {
    *num_entries = 0;
    for (int i = 0; i < g.num_rules; i++) {
        char non_terminal = g.rules[i].non_terminal;
        char *production = g.rules[i].production;
        char first_symbol = production[0];

        if (first_symbol >= 'a' && first_symbol <= 'z') { // Terminal
            ll1_table[*num_entries].non_terminal = non_terminal;
            ll1_table[*num_entries].terminal = first_symbol;
            strcpy(ll1_table[*num_entries].production, production);
            (*num_entries)++;
        } else if (first_symbol >= 'A' && first_symbol <= 'Z') { // Non-terminal
            for (int j = 0; j < g.num_non_terminals; j++) {
                if (first_follow[j].non_terminal == first_symbol) {
                    for (int k = 0; k < first_follow[j].num_first; k++) {
                        ll1_table[*num_entries].non_terminal = non_terminal;
                        ll1_table[*num_entries].terminal = first_follow[j].first[k];
                        strcpy(ll1_table[*num_entries].production, production);
                        (*num_entries)++;
                    }
                    break;
                }
            }
        } else if (first_symbol == '\0') { // Production vide (epsilon)
            for (int j = 0; j < g.num_non_terminals; j++) {
                if (first_follow[j].non_terminal == non_terminal) {
                    for (int k = 0; k < first_follow[j].num_follow; k++) {
                        ll1_table[*num_entries].non_terminal = non_terminal;
                        ll1_table[*num_entries].terminal = first_follow[j].follow[k];
                        strcpy(ll1_table[*num_entries].production, production);
                        (*num_entries)++;
                    }
                    break;
                }
            }
        }
    }
}

// Fonction pour afficher la table LL(1)
void print_ll1_table(LL1TableEntry *ll1_table, int num_entries) {
    printf("LL(1) Parsing Table:\n");
    printf("Non-Terminal\tTerminal\tProduction\n");
    for (int i = 0; i < num_entries; i++) {
        printf("%c\t\t%c\t\t%s\n", ll1_table[i].non_terminal, ll1_table[i].terminal, ll1_table[i].production);
    }
}


// int main() {
//     // Exemple de grammaire
//     Grammar g;
//     g.num_rules = 3;
//     g.rules[0].non_terminal = 'S';
//     strcpy(g.rules[0].production, "aB");
//     g.rules[1].non_terminal = 'B';
//     strcpy(g.rules[1].production, "bC");
//     g.rules[2].non_terminal = 'C';
//     strcpy(g.rules[2].production, "c");

//     g.num_non_terminals = 3;
//     g.non_terminals[0] = 'S';
//     g.non_terminals[1] = 'B';
//     g.non_terminals[2] = 'C';

//     g.num_terminals = 3;
//     g.terminals[0] = 'a';
//     g.terminals[1] = 'b';
//     g.terminals[2] = 'c';

//   //  eliminate_left_recursion();
//    // factorize_grammar();

//     // Calcul des ensembles "First" et "Follow"
//     FirstFollow first_follow[MAX_NON_TERMINALS];
//     calculate_first(g, first_follow);
//     calculate_follow(g, first_follow);

//     // Construction de la table LL(1)
//     LL1TableEntry ll1_table[MAX_RULES * MAX_TERMINALS];
//     int num_entries;
//     build_ll1_table(g, first_follow, ll1_table, &num_entries);

//     // Affichage de la table LL(1)
//     print_ll1_table(ll1_table, num_entries);

//     return 0;
// }
bool ReadSyntaxe(char *input, Grammar g, LL1TableEntry *ll1_table, int num_entries) {
    char stack[100];
    int top = 0;

    stack[top++] = '$';        // Symbole de fin
    stack[top++] = g.rules[0].non_terminal; // Symbole de départ (premier non-terminal)

    int index = 0;
    char symbol = input[index];

    while (top > 0) {
        char top_symbol = stack[--top]; // Pop du sommet de pile

        if (top_symbol == '$' && symbol == '$') {
            printf("✓ La chaîne est acceptée.\n");
            return true;
        }

        if (top_symbol >= 'a' && top_symbol <= 'z') {  // Terminal
            if (top_symbol == symbol) {
                index++;
                symbol = input[index];
            } else {
                printf("✗ Erreur syntaxique : attendu '%c', trouvé '%c'\n", top_symbol, symbol);
                return false;
            }
        } else if (top_symbol >= 'A' && top_symbol <= 'Z') {  // Non-terminal
            // Chercher la règle dans la table LL(1)
            bool found = false;
            for (int i = 0; i < num_entries; i++) {
                if (ll1_table[i].non_terminal == top_symbol && ll1_table[i].terminal == symbol) {
                    // Empiler la production (à l'envers)
                    char *prod = ll1_table[i].production;
                    int len = strlen(prod);
                    for (int j = len - 1; j >= 0; j--) {
                        if (prod[j] != '\0') {
                            stack[top++] = prod[j];
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                printf("✗ Erreur syntaxique : aucun élément dans la table LL(1) pour [%c][%c]\n", top_symbol, symbol);
                return false;
            }
        } else {
            printf("✗ Erreur : symbole inconnu '%c'\n", top_symbol);
            return false;
        }
    }

    return false;
}



int main() {
    Grammar g;

    // Définition des règles de production
    g.num_rules = 10;
    strcpy(g.rules[0].production, "aAS");
    g.rules[0].non_terminal = 'S';
    strcpy(g.rules[1].production, "bB");
    g.rules[1].non_terminal = 'S';

    strcpy(g.rules[2].production, "cC");
    g.rules[2].non_terminal = 'A';
    strcpy(g.rules[3].production, "dD");
    g.rules[3].non_terminal = 'A';

    strcpy(g.rules[4].production, "b");
    g.rules[4].non_terminal = 'B';
    strcpy(g.rules[5].production, "");  // epsilon
    g.rules[5].non_terminal = 'B';

    strcpy(g.rules[6].production, "c");
    g.rules[6].non_terminal = 'C';
    strcpy(g.rules[7].production, "");  // epsilon
    g.rules[7].non_terminal = 'C';

    strcpy(g.rules[8].production, "d");
    g.rules[8].non_terminal = 'D';
    strcpy(g.rules[9].production, "");  // epsilon
    g.rules[9].non_terminal = 'D';

    // Définition des non-terminaux
    g.num_non_terminals = 5;
    g.non_terminals[0] = 'S';
    g.non_terminals[1] = 'A';
    g.non_terminals[2] = 'B';
    g.non_terminals[3] = 'C';
    g.non_terminals[4] = 'D';

    // Définition des terminaux
    g.num_terminals = 4;
    g.terminals[0] = 'a';
    g.terminals[1] = 'b';
    g.terminals[2] = 'c';
    g.terminals[3] = 'd';

    // Calcul des ensembles FIRST et FOLLOW
    FirstFollow first_follow[MAX_NON_TERMINALS];
    calculate_first(g, first_follow);
    calculate_follow(g, first_follow);

    // Construction de la table LL(1)
    LL1TableEntry ll1_table[MAX_RULES * MAX_TERMINALS];
    int num_entries;
    build_ll1_table(g, first_follow, ll1_table, &num_entries);

    // Affichage des ensembles FIRST
    printf("\n=== FIRST ===\n");
    for (int i = 0; i < g.num_non_terminals; i++) {
        printf("FIRST(%c) = { ", first_follow[i].non_terminal);
        for (int j = 0; j < first_follow[i].num_first; j++) {
            printf("%c ", first_follow[i].first[j]);
        }
        printf("}\n");
    }

    // Affichage des ensembles FOLLOW
    printf("\n=== FOLLOW ===\n");
    for (int i = 0; i < g.num_non_terminals; i++) {
        printf("FOLLOW(%c) = { ", first_follow[i].non_terminal);
        for (int j = 0; j < first_follow[i].num_follow; j++) {
            printf("%c ", first_follow[i].follow[j]);
        }
        printf("}\n");
    }

    // Affichage de la table LL(1)
    printf("\n=== TABLE LL(1) ===\n");
    for (int i = 0; i < num_entries; i++) {
        printf("M[%c][%c] = %s\n", ll1_table[i].non_terminal, ll1_table[i].terminal,
               strlen(ll1_table[i].production) > 0 ? ll1_table[i].production : "ε");
    }

    char input[100];
    while (true){
        printf("\nEntrez une chaîne à analyser (terminée par $) : ");
        scanf("%s", input);
    
        ReadSyntaxe(input, g, ll1_table, num_entries);
    }
    
    return 0;
}


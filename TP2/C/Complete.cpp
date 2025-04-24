#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_RULES 50
#define MAX_SYMBOLS 128
#define MAX_STATES 100
#define MAX_ITEMS 200
#define MAX_RHS 20
#define MAX_STACK 100
#define MAX_INPUT 1000
#define MAX_LINE 1000
#define EPSILON '%'  // ASCII character for epsilon

// Data structure definitions
typedef struct {
    char lhs;
    char rhs[MAX_RHS];
    int length;
} Rule;

typedef struct {
    int rule_index;
    int dot_position;
    char lookahead;
} LR1Item;

typedef struct {
    LR1Item items[MAX_ITEMS];
    int num_items;
} LR1State;

typedef struct {
    char action[MAX_STATES][MAX_SYMBOLS][10];
    int goto_table[MAX_STATES][MAX_SYMBOLS];
} LR1Table;

// Grammar storage
Rule grammar[MAX_RULES];
int num_rules = 0;
char terminals[MAX_SYMBOLS];
int num_terminals = 0;
char non_terminals[MAX_SYMBOLS];
int num_non_terminals = 0;

// Check if a symbol is a terminal
bool is_terminal(char symbol) {
    for (int i = 0; i < num_terminals; i++) {
        if (terminals[i] == symbol) return true;
    }
    return false;
}

// Check if a symbol is a non-terminal
bool is_non_terminal(char symbol) {
    for (int i = 0; i < num_non_terminals; i++) {
        if (non_terminals[i] == symbol) return true;
    }
    return false;
}

// Check if a rule can produce epsilon (empty string)
bool can_derive_epsilon(char symbol) {
    for (int r = 0; r < num_rules; r++) {
        if (grammar[r].lhs == symbol && 
           (grammar[r].length == 0 || 
            (grammar[r].length == 1 && grammar[r].rhs[0] == EPSILON))) {
            return true;
        }
    }
    return false;
}

// Calculate FIRST sets for a symbol
void compute_first_sets(bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS]) {
    bool changed;
    
    // Initialize: FIRST(a) = {a} for all terminals a
    for (int i = 0; i < num_terminals; i++) {
        char a = terminals[i];
        first_sets[a][a] = true;
    }
    
    // Iterative calculation until no more changes
    do {
        changed = false;
        
        for (int i = 0; i < num_rules; i++) {
            char A = grammar[i].lhs;
            char *beta = grammar[i].rhs;
            int len = grammar[i].length;
            
            if (len == 0) {
                // A -> ε, add ε to FIRST(A)
                if (!first_sets[A][EPSILON]) {
                    first_sets[A][EPSILON] = true;
                    changed = true;
                }
                continue;
            }
            
            // For each symbol Y of B, add FIRST(Y) to FIRST(A)
            for (int j = 0; j < len; j++) {
                char Yj = beta[j];
                
                // Add FIRST(Yj) to FIRST(A), except epsilon
                for (int k = 0; k < MAX_SYMBOLS; k++) {
                    if (k != EPSILON && first_sets[Yj][k] && !first_sets[A][k]) {
                        first_sets[A][k] = true;
                        changed = true;
                    }
                }
                
                // If Yj cannot derive epsilon, stop
                if (!can_derive_epsilon(Yj)) break;
                
                // If all symbols can derive epsilon, add epsilon to FIRST(A)
                if (j == len - 1 && can_derive_epsilon(Yj) && !first_sets[A][EPSILON]) {
                    first_sets[A][EPSILON] = true;
                    changed = true;
                }
            }
        }
    } while (changed);
}

// Calculate FIRST for a string
void first_of_string(char *str, int len, char lookahead, bool result[MAX_SYMBOLS], bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS]) {
    if (len == 0) {
        result[lookahead] = true;
        return;
    }
    
    char first_sym = str[0];
    
    // Add FIRST(first_sym) to result, except epsilon
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (i != EPSILON && first_sets[first_sym][i]) {
            result[i] = true;
        }
    }
    
    // If first_sym can derive epsilon and there are other symbols, continue with the rest
    if (can_derive_epsilon(first_sym) && len > 1) {
        first_of_string(str + 1, len - 1, lookahead, result, first_sets);
    }
    // If first_sym can derive epsilon and it's the last symbol, add lookahead
    else if (can_derive_epsilon(first_sym) && len == 1) {
        result[lookahead] = true;
    }
}

// Check if two LR(1) items are identical
bool items_equal(LR1Item item1, LR1Item item2) {
    return item1.rule_index == item2.rule_index && 
           item1.dot_position == item2.dot_position && 
           item1.lookahead == item2.lookahead;
}

// Check if an item already exists in a set
bool item_exists(LR1State *state, LR1Item item) {
    for (int i = 0; i < state->num_items; i++) {
        if (items_equal(state->items[i], item)) {
            return true;
        }
    }
    return false;
}

// Calculate the closure of a set of LR(1) items
void closure(LR1State *state, bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS]) {
    bool changed;
    
    do {
        changed = false;
        
        for (int i = 0; i < state->num_items; i++) {
            LR1Item item = state->items[i];
            Rule rule = grammar[item.rule_index];
            
            // Check if the dot is before a non-terminal
            if (item.dot_position < rule.length) {
                char B = rule.rhs[item.dot_position];
                
                if (is_non_terminal(B)) {
                    // Remove the dynamic non-terminal addition
                    // Calculate FIRST(βa)
                    bool first_beta_a[MAX_SYMBOLS] = {false};
                    
                    if (item.dot_position + 1 < rule.length) {
                        // Beta exists, calculate FIRST(beta a)
                        char beta[MAX_RHS];
                        int beta_len = 0;
                        for (int j = item.dot_position + 1; j < rule.length; j++) {
                            beta[beta_len++] = rule.rhs[j];
                        }
                        first_of_string(beta, beta_len, item.lookahead, first_beta_a, first_sets);
                    } else {
                        // No beta, lookahead is added directly
                        first_beta_a[item.lookahead] = true;
                    }
                    
                    // For each production B -> y, add [B -> .y, b] to the closure
                    for (int j = 0; j < num_rules; j++) {
                        if (grammar[j].lhs == B) {
                            for (int k = 0; k < MAX_SYMBOLS; k++) {
                                if (first_beta_a[k]) {
                                    LR1Item new_item = {j, 0, (char)k};
                                    if (!item_exists(state, new_item)) {
                                        if (state->num_items < MAX_ITEMS) {
                                            state->items[state->num_items++] = new_item;
                                            changed = true;
                                        } else {
                                            printf("Warning: MAX_ITEMS reached\n");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (changed);
}

// Calculate GOTO(I, X)
LR1State goto_state(LR1State state, char symbol, bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS]) {
    LR1State new_state = {0};
    
    for (int i = 0; i < state.num_items; i++) {
        LR1Item item = state.items[i];
        Rule rule = grammar[item.rule_index];
        
        if (item.dot_position < rule.length && rule.rhs[item.dot_position] == symbol) {
            LR1Item new_item = {
                item.rule_index,
                item.dot_position + 1,
                item.lookahead
            };
            new_state.items[new_state.num_items++] = new_item;
        }
    }
    
    if (new_state.num_items > 0) {
        closure(&new_state, first_sets);
    }
    
    return new_state;
}

// Check if two states are identical
bool states_equal(LR1State state1, LR1State state2) {
    if (state1.num_items != state2.num_items) return false;
    
    for (int i = 0; i < state1.num_items; i++) {
        if (!item_exists(&state2, state1.items[i])) {
            return false;
        }
    }
    
    return true;
}

// Find the index of a state in the collection, or -1 if it doesn't exist
int find_state(LR1State *states, int num_states, LR1State state) {
    for (int i = 0; i < num_states; i++) {
        if (states_equal(states[i], state)) {
            return i;
        }
    }
    return -1;
}

// Build the canonical collection of LR(1) states
void build_lr1_states(LR1State *states, int *num_states, bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS]) {
    // Initial state with [S' -> .S, $] (assuming grammar[0] is S' -> S)
    LR1State initial_state = {0};
    // Ensure grammar[0] is the augmented rule S' -> OriginalStartSymbol
    if (num_rules == 0) {
        printf("Error: Cannot build states, grammar is empty or not augmented.\n");
        return;
    }
    LR1Item initial_item = {0, 0, '$'}; // Rule 0, dot at start, lookahead $
    initial_state.items[0] = initial_item;
    initial_state.num_items = 1;
    
    // Calculate the closure of the initial state
    closure(&initial_state, first_sets);
    states[0] = initial_state;
    *num_states = 1;
    
    // Build other states
    int i = 0;
    while (i < *num_states) {
        // For each grammar symbol (terminals and non-terminals)
        for (int t = 0; t < num_terminals; t++) {
            char symbol = terminals[t];
            LR1State new_state = goto_state(states[i], symbol, first_sets);
            
            if (new_state.num_items > 0) {
                int state_idx = find_state(states, *num_states, new_state);
                if (state_idx == -1) {
                    if (*num_states < MAX_STATES) {
                        states[*num_states] = new_state;
                        (*num_states)++;
                    } else {
                        printf("Warning: MAX_STATES reached\n");
                    }
                }
            }
        }
        
        for (int n = 0; n < num_non_terminals; n++) {
            char symbol = non_terminals[n];
            LR1State new_state = goto_state(states[i], symbol, first_sets);
            
            if (new_state.num_items > 0) {
                int state_idx = find_state(states, *num_states, new_state);
                if (state_idx == -1) {
                    if (*num_states < MAX_STATES) {
                        states[*num_states] = new_state;
                        (*num_states)++;
                    } else {
                        printf("Warning: MAX_STATES reached\n");
                    }
                }
            }
        }
        
        i++;
    }
}

// Build the LR(1) parsing table
void build_lr1_table(LR1State *states, int num_states, LR1Table *table, bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS]) {
    // Initialize tables
    memset(table->action, 0, sizeof(table->action));
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            table->goto_table[i][j] = -1;
        }
    }
    
    // First, calculate all GOTO transitions for each state and non-terminal
    for (int i = 0; i < num_states; i++) {
        for (int n = 0; n < num_non_terminals; n++) {
            char symbol = non_terminals[n];
            LR1State new_state = goto_state(states[i], symbol, first_sets);
            
            if (new_state.num_items > 0) {
                int target = find_state(states, num_states, new_state);
                if (target != -1) {
                    table->goto_table[i][symbol] = target;
                }
            }
        }
    }
    
    // Then fill the ACTION table
    for (int i = 0; i < num_states; i++) {
        // For each item in state i
        for (int j = 0; j < states[i].num_items; j++) {
            LR1Item item = states[i].items[j];
            Rule rule = grammar[item.rule_index];
            
            if (item.dot_position < rule.length) {
                // [A -> α.Xb, a], X is the symbol after the dot
                char X = rule.rhs[item.dot_position];
                
                // If X is a terminal, add a shift action
                if (is_terminal(X)) {
                    LR1State new_state = goto_state(states[i], X, first_sets);
                    int target = find_state(states, num_states, new_state);
                    
                    if (target != -1) {
                        // Shift action:
                        if (table->action[i][X][0] == 0) {
                            sprintf(table->action[i][X], "s%d", target);
                        } else if (strncmp(table->action[i][X], "s", 1) != 0) {
                            printf("Conflict in state %d for symbol %c: %s vs s%d\n", 
                                   i, X, table->action[i][X], target);
                            // Conflict resolution: prefer shift action
                            sprintf(table->action[i][X], "s%d", target);
                        }
                    }
                }
                // We don't need to handle non-terminals here anymore since we did it above
            } else {
                // [A -> b., a], the dot is at the end, add a reduce action
                if (item.rule_index == 0 && item.lookahead == '$') {
                    // S' -> S., $ -> accept
                    if (table->action[i]['$'][0] == 0) {
                        strcpy(table->action[i]['$'], "acc");
                    } else {
                        printf("Conflict in state %d for symbol $: %s vs acc\n", 
                               i, table->action[i]['$']);
                        // Accept should always take priority
                        strcpy(table->action[i]['$'], "acc");
                    }
                } else {
                    // A -> b., a -> reduce by A -> b
                    char a = item.lookahead;
                    if (table->action[i][a][0] == 0) {
                        sprintf(table->action[i][a], "r%d", item.rule_index);
                    } else {
                        // Conflict detected
                        if (strncmp(table->action[i][a], "r", 1) == 0) {
                            // Reduce-Reduce conflict
                            int existing_rule;
                            sscanf(table->action[i][a], "r%d", &existing_rule);
                            printf("Reduce-Reduce conflict in state %d for symbol %c: r%d vs r%d\n", 
                                   i, a, existing_rule, item.rule_index);
                            
                            // Resolution by rule priority (choose rule with lower index)
                            if (item.rule_index < existing_rule) {
                                sprintf(table->action[i][a], "r%d", item.rule_index);
                            }
                        } else if (strncmp(table->action[i][a], "s", 1) == 0) {
                            // Shift-Reduce conflict
                            int shift_state;
                            sscanf(table->action[i][a], "s%d", &shift_state);
                            printf("Shift-Reduce conflict in state %d for symbol %c: s%d vs r%d\n", 
                                   i, a, shift_state, item.rule_index);
                            
                            // By default, prefer shift (do nothing)
                            // For operator precedence, additional logic could be added here
                        }
                    }
                }
            }
        }
    }
}

// Print an LR(1) item
void print_item(LR1Item item) {
    Rule rule = grammar[item.rule_index];
    printf("[%c -> ", rule.lhs);
    
    for (int i = 0; i < rule.length; i++) {
        if (i == item.dot_position) printf(".");
        printf("%c", rule.rhs[i]);
    }
    
    if (item.dot_position == rule.length) printf(".");
    printf(", %c]", item.lookahead);
}

// Print an LR(1) state
void print_state(LR1State state, int state_num) {
    printf("State %d:\n", state_num);
    for (int i = 0; i < state.num_items; i++) {
        printf("  ");
        print_item(state.items[i]);
        printf("\n");
    }
}

// Print the LR(1) table
void print_table(LR1Table *table, int num_states) {
    printf("\nACTION TABLE:\n");
    
    // Header with all terminals
    printf("State\t");
    for (int t = 0; t < num_terminals; t++) {
        printf("%c\t", terminals[t]);
    }
    printf("\n");
    
    // Separator line
    printf("-----");
    for (int t = 0; t < num_terminals; t++) {
        printf("--------");
    }
    printf("\n");
    
    // ACTION table content
    for (int i = 0; i < num_states; i++) {
        printf("%d\t", i);
        
        for (int t = 0; t < num_terminals; t++) {
            char term = terminals[t];
            if (table->action[i][term][0] != 0) {
                printf("%s\t", table->action[i][term]);
            } else {
                printf("\t");
            }
        }
        printf("\n");
    }
    
    printf("\nGOTO TABLE:\n");
    
    // Header with all non-terminals
    printf("State\t");
    for (int n = 0; n < num_non_terminals; n++) {
        printf("%c\t", non_terminals[n]);
    }
    printf("\n");
    
    // Separator line
    printf("-----");
    for (int n = 0; n < num_non_terminals; n++) {
        printf("--------");
    }
    printf("\n");
    
    // GOTO table content
    for (int i = 0; i < num_states; i++) {
        printf("%d\t", i);
        
        for (int n = 0; n < num_non_terminals; n++) {
            char non_term = non_terminals[n];
            if (table->goto_table[i][non_term] != -1) {
                printf("%d\t", table->goto_table[i][non_term]);
            } else {
                printf("\t");
            }
        }
        printf("\n");
    }
}

// Parse an input string with the LR(1) table
bool parse_input(char *input, LR1Table *table, int num_states) {
    int stack[MAX_STACK];  // State stack
    int top = 0;           // Stack top
    stack[top] = 0;        // Initial state
    
    int i = 0;
    char symbol = input[i++];
    
    printf("Parsing input: %s\n", input);
    printf("Step\tStack\tSymbol\tAction\n");
    printf("--------------------------------------\n");
    
    int step = 1;
    while (true) {
        int state = stack[top];
        
        printf("%d\t", step++);
        printf("[");
        for (int j = 0; j <= top; j++) {
            printf("%d", stack[j]);
            if (j < top) printf(" ");
        }
        printf("]\t%c\t", symbol);
        
        // Check action for current symbol
        if (table->action[state][symbol][0] == 0) {
            printf("Error: No action defined for state %d and symbol %c\n", state, symbol);
            return false;
        }
        
        if (strncmp(table->action[state][symbol], "s", 1) == 0) {
            // Shift action
            int next_state;
            sscanf(table->action[state][symbol], "s%d", &next_state);
            printf("Shift %d\n", next_state);
            
            stack[++top] = next_state;
            symbol = input[i++];
        }
        else if (strncmp(table->action[state][symbol], "r", 1) == 0) {
            // Reduce action
            int rule_index;
            sscanf(table->action[state][symbol], "r%d", &rule_index);
            Rule rule = grammar[rule_index];
            
            printf("Reduce by %c -> ", rule.lhs);
            for (int j = 0; j < rule.length; j++) {
                printf("%c", rule.rhs[j]);
            }
            printf("\n");
            
            // Pop states
            top -= rule.length;
            
            // Check GOTO[s', A] where s' is the top state
            int goto_state = table->goto_table[stack[top]][rule.lhs];
            if (goto_state == -1) {
                printf("Error: No GOTO defined for state %d and non-terminal %c\n", stack[top], rule.lhs);
                return false;
            }
            stack[++top] = goto_state;
        }
        else if (strcmp(table->action[state][symbol], "acc") == 0) {
            printf("Accept\n");
            return true;
        }
        else {
            printf("Error: Invalid action %s\n", table->action[state][symbol]);
            return false;
        }
    }
    
    return false;
}

// Read grammar from user input
void read_grammar() {
    printf("Enter grammar rules (one per line, format: 'X -> abc', use '%%' for epsilon, empty line to finish):\n");
    
    // Clear existing data
    num_rules = 0;
    num_terminals = 0;
    num_non_terminals = 0;
    memset(terminals, 0, sizeof(terminals));
    memset(non_terminals, 0, sizeof(non_terminals));
    memset(grammar, 0, sizeof(grammar)); // Clear grammar array too
    
    // Add $ as the first terminal
    terminals[num_terminals++] = '$';
    
    char line[MAX_LINE];
    bool first_rule = true;
    char original_start_symbol = 0;

    while (1) {
        if (fgets(line, MAX_LINE, stdin) == NULL || line[0] == '\n' || line[0] == '\r') {
            // Check if at least one rule was entered
            if (num_rules == 0) {
                 printf("Error: No grammar rules entered.\n");
                 exit(1); // Or handle error appropriately
            }
            break; // Finished reading
        }
        
        // Remove trailing newline
        line[strcspn(line, "\r\n")] = 0;

        // Parse the rule (Example: "E -> E+T")
        char lhs;
        char rhs_str[MAX_RHS] = {0};
        if (sscanf(line, " %c -> %s", &lhs, rhs_str) != 2) {
             printf("Error: Invalid rule format: %s\n", line);
             continue; // Skip invalid line
        }

        // --- Augmentation Logic ---
        if (first_rule) {
            original_start_symbol = lhs;
            // Add the augmented rule S' -> S (using a symbol not in grammar, e.g., ASCII 1)
            grammar[0].lhs = 1; // Use a special character for S'
            grammar[0].rhs[0] = original_start_symbol;
            grammar[0].length = 1;
            num_rules = 1; // Start counting rules from 1 for user rules

            // Add S' to non-terminals
            non_terminals[num_non_terminals++] = 1; 
            first_rule = false; 
        }
        // --- End Augmentation Logic ---


        // Add LHS to non-terminals if new
        bool found = false;
        for (int i = 0; i < num_non_terminals; i++) {
            if (non_terminals[i] == lhs) {
                found = true;
                break;
            }
        }
        if (!found && isupper(lhs)) { // Assuming non-terminals are uppercase
             if (num_non_terminals < MAX_SYMBOLS) {
                non_terminals[num_non_terminals++] = lhs;
             } else {
                 printf("Warning: MAX_SYMBOLS reached for non-terminals.\n");
             }
        } else if (!isupper(lhs)) {
             printf("Warning: LHS '%c' is not uppercase. Treating as non-terminal.\n", lhs);
             // Optionally add logic to handle non-uppercase non-terminals if needed
             if (!found) {
                 if (num_non_terminals < MAX_SYMBOLS) non_terminals[num_non_terminals++] = lhs;
             }
        }


        // Process RHS
        grammar[num_rules].lhs = lhs;
        int rhs_len = 0;
        for (int k = 0; rhs_str[k] != '\0' && k < MAX_RHS -1 ; k++) {
            char symbol = rhs_str[k];
            
            if (symbol == EPSILON) { // Handle epsilon
                 if (strlen(rhs_str) == 1) { // Only epsilon on RHS
                     grammar[num_rules].length = 0; // Represent as empty RHS
                     // Don't add epsilon itself to terminals/non-terminals
                     rhs_len = 0; // Ensure length is 0
                     break; 
                 } else {
                     printf("Error: Epsilon '%' must be the only symbol on RHS in rule: %s\n", line);
                     // Skip this rule or handle error
                     rhs_len = -1; // Mark as error
                     break;
                 }
            }

            grammar[num_rules].rhs[rhs_len++] = symbol;

            // Add symbol to terminals/non-terminals if new
            bool is_nt = false;
            for(int nt_idx = 0; nt_idx < num_non_terminals; ++nt_idx) {
                if (non_terminals[nt_idx] == symbol) {
                    is_nt = true;
                    break;
                }
            }

            if (!is_nt) { // If not already a non-terminal, check if it's a terminal
                bool is_term = false;
                 for(int t_idx = 0; t_idx < num_terminals; ++t_idx) {
                     if (terminals[t_idx] == symbol) {
                         is_term = true;
                         break;
                     }
                 }
                 if (!is_term) { // New terminal
                     if (num_terminals < MAX_SYMBOLS) {
                         terminals[num_terminals++] = symbol;
                     } else {
                          printf("Warning: MAX_SYMBOLS reached for terminals.\n");
                     }
                 }
            }
        }
        
        if (rhs_len != -1) { // Only increment rule count if rule was valid
            grammar[num_rules].length = rhs_len;
            num_rules++;
        }

        if (num_rules >= MAX_RULES) {
            printf("Warning: MAX_RULES reached.\n");
            break;
        }
    }

     // Ensure the original start symbol is marked as non-terminal if not already
     bool start_symbol_is_nt = false;
     for (int i = 0; i < num_non_terminals; i++) {
         if (non_terminals[i] == original_start_symbol) {
             start_symbol_is_nt = true;
             break;
         }
     }
     if (!start_symbol_is_nt && original_start_symbol != 0) {
          if (num_non_terminals < MAX_SYMBOLS) {
              non_terminals[num_non_terminals++] = original_start_symbol;
          } else {
               printf("Warning: MAX_SYMBOLS reached for non-terminals (adding start symbol).\n");
          }
     }
}

int main() {
    printf("LR(1) Parser Generator\n");
    printf("======================\n\n");
    
    // Read grammar from user
    read_grammar();
    
    // Calculate FIRST sets
    bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS] = {{false}};
    compute_first_sets(first_sets);
    
    // Display FIRST sets for verification
    printf("FIRST sets:\n");
    for (int n = 0; n < num_non_terminals; n++) {
        char nt = non_terminals[n];
        printf("FIRST(%c) = { ", nt);
        bool first = true;
        for (int t = 0; t < MAX_SYMBOLS; t++) {
            if (first_sets[nt][t]) {
                if (!first) printf(", ");
                if (t == EPSILON) {
                    printf("epsilon");
                } else {
                    printf("%c", t);
                }
                first = false;
            }
        }
        printf(" }\n");
    }
    printf("\n");
    
    // Build LR(1) states
    LR1State states[MAX_STATES];
    int num_states = 0;
    build_lr1_states(states, &num_states, first_sets);
    
    // Display states for verification
    printf("Number of states: %d\n", num_states);
    for (int i = 0; i < num_states; i++) {
        print_state(states[i], i);
        printf("\n");
    }
    
    // Build LR(1) table
    LR1Table table;
    build_lr1_table(states, num_states, &table, first_sets);
    
    // Display LR(1) table
    print_table(&table, num_states);
    
    // Parse input strings
    char input[MAX_INPUT];
    printf("\nEnter strings to parse (append $ at the end, empty line to quit):\n");
    
    while (1) {
        printf("\nInput string (with $ at end): ");
        if (fgets(input, MAX_INPUT, stdin) == NULL || input[0] == '\n') {
            break;
        }
        
        // Remove newline
        int len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        // Parse the input
        if (parse_input(input, &table, num_states)) {
            printf("\nResult: VALID - The input string is in the language!\n");
        } else {
            printf("\nResult: INVALID - The input string is not in the language.\n");
        }
    }
    
    printf("Thank you for using the LR(1) Parser Generator!\n");
    return 0;
}
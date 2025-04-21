#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STATES 20
#define MAX_SYMBOLS 20
#define MAX_STACK 100

enum Symbol {
    // Terminals
    A_SYM, B_SYM, C_SYM, D_SYM, E_SYM, F_SYM, DOLLAR,
    // Non-terminals
    S_NT, A_NT, B_NT, C_NT, S_PRIME,
    SYMBOL_COUNT
};

enum ActionType { SHIFT, REDUCE, ACCEPT, ERROR };

typedef struct {
    enum ActionType type;
    int value;
} Action;

typedef struct {
    int lhs;
    int rhs_len;
    int rhs[MAX_SYMBOLS];
} Rule;

Rule rules[] = {
    {S_PRIME, 1, {S_NT}},          // 0: S' → S
    {S_NT, 3, {A_SYM, A_NT, B_NT}}, // 1: S → aAB
    {S_NT, 3, {B_SYM, S_NT, S_NT}}, // 2: S → bSS
    {A_NT, 2, {C_NT, C_NT}},       // 3: A → CC
    {A_NT, 1, {F_SYM}},            // 4: A → f
    {C_NT, 2, {C_SYM, C_NT}},      // 5: C → cC
    {C_NT, 1, {D_SYM}},            // 6: C → d
    {B_NT, 2, {B_SYM, B_NT}},      // 7: B → bB
    {B_NT, 1, {E_SYM}}             // 8: B → e
};
const int num_rules = 9;

Action action_table[MAX_STATES][SYMBOL_COUNT];
int goto_table[MAX_STATES][SYMBOL_COUNT];

void initialize_tables() {
    // Initialize all to ERROR
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < SYMBOL_COUNT; j++) {
            action_table[i][j] = (Action){ERROR, -1};
            goto_table[i][j] = -1;
        }
    }

    // State 0
    action_table[0][A_SYM] = (Action){SHIFT, 2};
    action_table[0][B_SYM] = (Action){SHIFT, 3};
    goto_table[0][S_NT] = 1;

    // State 1
    action_table[1][DOLLAR] = (Action){ACCEPT, 0};

    // State 2
    action_table[2][C_SYM] = (Action){SHIFT, 7};
    action_table[2][D_SYM] = (Action){SHIFT, 8};
    action_table[2][F_SYM] = (Action){SHIFT, 6};
    goto_table[2][A_NT] = 4;
    goto_table[2][C_NT] = 5;

    // State 3
    action_table[3][A_SYM] = (Action){SHIFT, 2};
    action_table[3][B_SYM] = (Action){SHIFT, 3};
    goto_table[3][S_NT] = 9;

    // State 4
    action_table[4][B_SYM] = (Action){SHIFT, 11};
    action_table[4][E_SYM] = (Action){SHIFT, 12};
    goto_table[4][B_NT] = 10;

    // State 5
    action_table[5][C_SYM] = (Action){SHIFT, 7};
    action_table[5][D_SYM] = (Action){SHIFT, 8};
    goto_table[5][C_NT] = 13;

    // State 6
    action_table[6][B_SYM] = (Action){REDUCE, 4}; // A → f
    action_table[6][E_SYM] = (Action){REDUCE, 4}; // A → f

    // State 7
    action_table[7][C_SYM] = (Action){SHIFT, 7};
    action_table[7][D_SYM] = (Action){SHIFT, 8};
    goto_table[7][C_NT] = 14;

    // State 8
    action_table[8][C_SYM] = (Action){REDUCE, 6}; // C → d
    action_table[8][D_SYM] = (Action){REDUCE, 6}; // C → d
    action_table[8][B_SYM] = (Action){REDUCE, 6}; // C → d
    action_table[8][E_SYM] = (Action){REDUCE, 6}; // C → d

    // State 9
    action_table[9][A_SYM] = (Action){SHIFT, 2};
    action_table[9][B_SYM] = (Action){SHIFT, 3};
    goto_table[9][S_NT] = 15;

    // State 10
    action_table[10][DOLLAR] = (Action){REDUCE, 1}; // S → aAB

    // State 11
    action_table[11][B_SYM] = (Action){SHIFT, 11};
    action_table[11][E_SYM] = (Action){SHIFT, 12};
    goto_table[11][B_NT] = 16;

    // State 12
    action_table[12][DOLLAR] = (Action){REDUCE, 8}; // B → e
    action_table[12][B_SYM] = (Action){REDUCE, 8}; // B → e
    action_table[12][E_SYM] = (Action){REDUCE, 8}; // B → e

    // State 13
    action_table[13][B_SYM] = (Action){REDUCE, 3}; // A → CC
    action_table[13][E_SYM] = (Action){REDUCE, 3}; // A → CC

    // State 14
    action_table[14][B_SYM] = (Action){REDUCE, 5}; // C → cC
    action_table[14][C_SYM] = (Action){REDUCE, 5}; // C → cC
    action_table[14][D_SYM] = (Action){REDUCE, 5}; // C → cC
    action_table[14][E_SYM] = (Action){REDUCE, 5}; // C → cC

    // State 15
    action_table[15][DOLLAR] = (Action){REDUCE, 2}; // S → bSS

    // State 16
    action_table[16][DOLLAR] = (Action){REDUCE, 7}; // B → bB
}

bool parse_input(int *input, int length) {
    int stack[MAX_STACK];
    int top = 0;
    stack[top++] = 0; // Initial state

    int pos = 0;
    while (pos < length) {
        int state = stack[top-1];
        int symbol = input[pos];

        Action action = action_table[state][symbol];
        
        switch (action.type) {
            case SHIFT:
                if (top >= MAX_STACK-1) return false;
                stack[top++] = symbol;
                stack[top++] = action.value;
                pos++;
                break;
                
            case REDUCE: {
                Rule rule = rules[action.value];
                if (top < 2*rule.rhs_len) return false;
                top -= 2*rule.rhs_len;
                int new_state = stack[top-1];
                int lhs = rule.lhs;
                int next_state = goto_table[new_state][lhs];
                if (next_state == -1) return false;
                stack[top++] = lhs;
                stack[top++] = next_state;
                break;
            }
            
            case ACCEPT:
                return true;
                
            case ERROR:
            default:
                return false;
        }
    }
    return false;
}




int main() {
    initialize_tables();

    // Test cases
    printf("\nTest Cases:\n");
    
    // Valid: a c d b e $
    int input1[] = {A_SYM, D_SYM, D_SYM, E_SYM, DOLLAR};
    printf("Parsing 'a d d e $': ");
    printf(parse_input(input1, 6) ? "Valid\n" : "Invalid\n");

    // Valid: b a f b e $
    int input2[] = {B_SYM, A_SYM, F_SYM, B_SYM, E_SYM, DOLLAR};
    printf("Parsing 'b a f b e $': ");
    printf(parse_input(input2, 6) ? "Valid\n" : "Invalid\n");

    // Invalid: a b c $
    int input3[] = {A_SYM, B_SYM, C_SYM, DOLLAR};
    printf("Parsing 'a b c $': ");
    printf(parse_input(input3, 4) ? "Valid\n" : "Invalid\n");

    return 0;
}
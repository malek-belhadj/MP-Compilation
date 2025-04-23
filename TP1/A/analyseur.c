#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// Types de token
typedef enum {
    OPERATOR,
    IDENTIFIER,
    FLOATING_POINT,
    INTEGER,
    SEPARATOR,
    END_OF_FILE
} TokenType;

// Structure de token
typedef struct {
    TokenType type;
    char value[100];
} Token;

// Fonction pour verifier si un caractère est un separateur
bool isSeparator(char c) {
    char separators[] = "();,";
    return strchr(separators, c) != NULL;
}

// Fonction pour scanner le prochain token
Token scanToken(FILE* input) {
    Token token;
    int c = fgetc(input);

    // Ignorer les espaces
    while (isspace(c))
        c = fgetc(input);

    // Si c'est la fin du fichier
    if (c == EOF) {
        token.type = END_OF_FILE;
        strcpy(token.value, "EOF");
        return token;
    }

    // Identifier le type de token // isalpha predefined function pour detecter les caracteres
    if (isalpha(c) || c == '_') {
        // Identificateur
        ungetc(c, input);
        fscanf(input, "%s", token.value);
        token.type = IDENTIFIER;
    } else if (isdigit(c)) {
        // Nombre (entier ou reel)
        ungetc(c, input);
        fscanf(input, "%s", token.value);
        if (strchr(token.value, '.') != NULL)
            token.type = FLOATING_POINT;
        else
            token.type = INTEGER;
    } else if (isSeparator(c)) {
        // Separateur
        token.type = SEPARATOR;
        token.value[0] = c;
        token.value[1] = '\0';
    } else {
        // Operateur
        token.type = OPERATOR;
        token.value[0] = c;
        token.value[1] = '\0';
    }

    return token;
}

// Fonction principale
int main() {
    FILE* input = fopen("input.txt", "r");
    if (input == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }

    Token token;
    do {
        token = scanToken(input);
        switch (token.type) {
            case OPERATOR:
                printf("Operateur: %s\n", token.value);
                break;
            case IDENTIFIER:
                printf("Identificateur: %s\n", token.value);
                break;
            case FLOATING_POINT:
                printf("Reel: %s\n", token.value);
                break;
            case INTEGER:
                printf("Nombre: %s\n", token.value);
                break;
            case SEPARATOR:
                printf("Separateur: %s\n", token.value);
                break;
            case END_OF_FILE:
                printf("Fin du fichier.\n");
                break;
        }
    } while (token.type != END_OF_FILE);

    fclose(input);
    return 0;
}
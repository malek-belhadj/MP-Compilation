%{
#include<stdio.h>
int yylex(void);
int yyerror(char *s);
%}

%token FIN;
%token SOUS;  /* soustraction */
%token DIV;   /* division */
%token NB;

%%
liste:FIN {printf("correct\n");}
     |SOUS listesous'.'liste
     |DIV listediv'.'liste
     ;
listesous: NB {$$ = $1;}
         |listesous','NB {$$ = $1 - $3; printf("Soustraction = %d\n", $$);}
         ;
listediv: NB {$$ = $1;}
        |listediv','NB {
            if($3 == 0) {
                printf("Erreur: Division par zéro\n");
                $$ = $1;
            } else {
                $$ = $1 / $3; 
                printf("Division = %d\n", $$);
            }
        }
        ;
%%

#include "lex.yy.c"
int yyerror(char *s)
{
    printf ("%s", s);
    return (0);
}
int main()
{
    yyparse();
    getchar();
}
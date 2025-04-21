%{
#include<stdio.h>
int yylex(void);
int yyerror(char *s);
%}

%token FIN;
%token SOM;
%token PROD;
%token NB;

%%
liste:FIN {printf("correct\n");}
     |SOM listesom'.'liste
     |PROD listeprod'.'liste
     ;
listesom: NB {$$ = $1;}
        |listesom','NB {$$ = $1 + $3; printf("Somme = %d\n", $$);}
        ;
listeprod: NB {$$ = $1;}
         |listeprod','NB {$$ = $1 * $3; printf("Produit = %d\n", $$);}
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
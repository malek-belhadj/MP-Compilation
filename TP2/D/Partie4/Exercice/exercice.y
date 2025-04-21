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
liste:FIN {printf("correct");} 
|SOM listesom'.'liste 
|PROD listeprod'.'liste; 
listesom: NB|listesom','NB; 
listeprod: NB|listeprod','NB; 
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
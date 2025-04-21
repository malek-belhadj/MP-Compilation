%{
#include <stdio.h>
%}

%%
[0-9]+[a-zA-Z]+[0-9]*  { printf("%s : ni id ni nb \n", yytext);  }
[a-zA-Z][a-zA-Z0-9]*    { printf("%s : id valide\n", yytext); }
[0-9]+                  { printf("%s : nb valide\n", yytext); }
.|\n                    ; 

%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}
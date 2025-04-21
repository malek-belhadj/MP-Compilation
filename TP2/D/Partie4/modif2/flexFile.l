%{
#include<stdio.h>
#include<math.h>
#include"exercice.tab.h"
%}
%%
[0-9]+ {yylval=atoi(yytext); return NB;}
soustraction {return SOUS;}
division {return DIV;}
[,|.] {return yytext[0];}
[$] {return FIN;}
[ \t\n] {}
. {printf("Erreur");}
%%
int yywrap()
{return 1;}
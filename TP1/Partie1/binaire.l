
%%
(0|1)+ fprintf(stdout, "c'est un nombre binaire\n");
.* fprintf(stdout, "Ce n'est pas un nombre binaire\n");
%%
  
int yywrap(void){return 1;}
  
int main(int argc, char *argv[])
{   
    freopen("resultat.txt", "w", stdout);
    ++argv, --argc;   
if ( argc > 0 )  
yyin = fopen( argv[0], "r" );  
else  
yyin = stdin;  
yylex() ;  
if (argc > 1)  
    fclose(yyin);  
fclose(stdout); 
return 0;
}
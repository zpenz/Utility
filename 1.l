%{
    int WordCount = 0;
%}

WORD [A-Za-z\_\'\.\"]+
%%
{WORD} {printf("word : %s\n",yytext);}
([0-9])+ {printf("number : %s\n",yytext);}
[=] {printf("op : %s\n",yytext);}
[/t] ; 
. {}
%%

int yywrap(){return 1;}

int main(void){
    yylex();
}
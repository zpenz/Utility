%{
    int WordCount = 0;
%}

WORD [A-Za-z\_\'\.\"]+
STRING \"\((?!\").\)+\"
%%
{WORD} {printf("word : %s\n",yytext);}
([0-9])+ {printf("number : %s\n",yytext);}
[=] {printf("op : %s\n",yytext);}
[/t] ; 
{STRING} {printf("string : %s",yytext);}
. {}

%%

int yywrap(){return 1;}

int main(void){
    yylex();
}
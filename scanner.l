%{
#include <stdio.h>
#include "tree.h"
#include "y.tab.h"

void yyerror (char * s);
int line = 1;

%}
L       [A-Za-z]
D       [0-9]
%%
[ \t]+          ;
[\n]		{++line;}
program		{return Prog;}
var		{return Var;}
int		{return Int;}
real		{return Real;}
boolean		{return Boolean;}
record		{return Record;}
field		{return Field;}
end		{return End;}
bind		{return Bind;}
to		{return To;}
assert		{return Assert;}
begin		{return Begin;}
loop		{return Loop;}
exit		{return Exit;}
when		{return When;}
if		{return If;}
then		{return Then;}
elsif		{return Elsif;}
else		{return Else;}
put		{return Put;}
or		{return Or;}
and		{return And;}
not		{return Not;}
"not="		{return Ne;}
div		{return Div;}
mod		{return Mod;}

";"		{return Semi;}
"="             {return Eq;}
":"		{return Colon;}
":="		{return Assign;}
"<"		{return Lt;}
"<="		{return Le;}
">"		{return Gt;}
">="		{return Ge;}
"+"             {return Plus;}
"-"             {return Minus;}
"*"             {return Mult;}
"/"             {return Slash;}
"("             {return Lparen;}
")"             {return Rparen;}
","		{return Comma;}
"."		{return Dot;}

{L}({L}|{D}|"_")*       {
				yylval.i = lookup(yytext);
				return Ident;
			}
{D}+			{
				yylval.i = atoi(yytext);
				return IntConst;
			}
[0-9]+(\.[0-9]+)?(([Ee](\+|\-)[0-9])?)   {
						yylval.i = lookup(yytext);
						return RealConst;
					}

.		{yyerror ("Bad Character");}
%%

int yywrap (void) { return 1; }

void yyerror (char s[])
{
        printf ("Error: %s at token %s in line %d\n", s, yytext, line);
}

typedef char name[20];
static char Names[200][20] = {"<no name>"};
int top = 0;

int lookup (char s[])
{
        int i;

        for (i = 1; i <= top; i ++)
                if (strcmp (s, Names[i]) == 0) return i;
        strcpy (Names[++top], s);
        return top;
}

void printNames (void)
{
        int i = 0;

        for (; i <= top; i++)
                printf ("%d\t%s\n", i, Names[i]);
}

char *id_name (int i)
{
        return Names[i];
}

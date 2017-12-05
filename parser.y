%{
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"

extern tree root;
%}

%token <i> Ident 1 IntConst 2 RealConst 3
%token Var 4 Int 5 Real 6 Boolean 7 Record 8 End 9
%token Bind 10 To 11 Assert 12 Begin 13 Loop 14 Exit 15
%token When 16 If 17 Then 18 Elsif 19 Else 20 Put 21 Or 22
%token And 23 Not 24 Ne 25 Div 26 Mod 27 Semi 28 Eq 29
%token Lt 30 Le 31 Gt 32 Ge 33 Colon 34 Assign 35 Plus 36
%token Minus 37 Mult 38 Slash 39 Dot 40 Comma 41
%token Lparen 42 Rparen 43 Prog 44 Field 45 NoType 46

%start	program

%union { tree p; int i;}

%type <p>	pStateDeclSeq idlist type field_list state_decls declaration statement ref
%type <p>	end_if expr and_expr not_expr rel_expr sum prod factor basic

%%
program
	: pStateDeclSeq
		{ root = buildTree(Prog, $1, NULL, NULL); }
	;
pStateDeclSeq 
	: /* empty */
		{ $$ = NULL; }
	| statement Semi pStateDeclSeq
		{ $$ = $1; $$->next = $3; }
	| Var idlist Colon type Semi pStateDeclSeq
		{ $$ = buildTree(Colon, buildTree(Var, $2, NULL, NULL), $4, NULL); $$->next = $6; }
	;
idlist
	: Ident
		{ $$ = buildIntTree(Ident, $1); }
	| Ident Comma idlist
		{ $$ = buildIntTree(Ident, $1); $$->next = $3; }
	;
type
	: Int
		{ $$ = buildTree(Int, NULL, NULL, NULL); }
	| Real
		{ $$ = buildTree(Real, NULL, NULL, NULL); }
	| Boolean
		{ $$ = buildTree(Boolean, NULL, NULL, NULL); }
	| Record field_list End Record
		{ $$ = buildTree(Record, $2, NULL, NULL); }
	;
field_list
	: idlist Colon type
		{ $$ = buildTree(Colon, $1, $3, NULL); }
	| idlist Colon type Semi field_list
		{ $$ = buildTree(Colon, $1, $3, NULL); $$->next = $5; }
	;
state_decls
	: /* empty */
		{ $$ = NULL; }
	| statement Semi state_decls
		{ $$ = $1; $$->next = $3; }
	| declaration Semi state_decls
		{ $$ = $1; $$->next = $3; }
	;
declaration
	: Var idlist Colon type
		{ $$ = buildTree(Colon, buildTree(Var, $2, NULL, NULL), $4, NULL); }
	| Bind idlist To ref
		{ $$ = buildTree(Bind, $2, $4, NULL); }
	| Bind Var idlist To ref
		{ $$ = buildTree(Bind, buildTree(Var, $3, NULL, NULL), $5, NULL); }
	;
statement
	: ref Assign expr
		{ $$ = buildTree(Assign, $1, $3, NULL); }
	| Assert expr
		{ $$ = buildTree(Assert, $2, NULL, NULL); }
	| Begin state_decls End
		{ $$ = buildTree(Begin, $2, NULL, NULL); }
	| Loop state_decls End Loop
		{ $$ = buildTree(Loop, $2, NULL, NULL); }
	| Exit
		{ $$ = buildTree(Exit, NULL, NULL, NULL); }
	| Exit When expr
		{ $$ = buildTree(Exit, buildTree(When, $3, NULL, NULL), NULL, NULL); }
	| If expr Then state_decls end_if
		{ $$ = buildTree(If, $2, buildTree(Then, $4, NULL, NULL), $5); }
	;
ref
	: Ident
		{ $$ = buildIntTree(Ident, $1); }
	| Ident Dot Ident
		{ $$ = buildTree(Dot, buildIntTree(Ident, $1), buildIntTree(Ident, $3), NULL); }
	;
end_if
	: End If
		{ $$ = buildTree(End, NULL, NULL, NULL); }
	| Else state_decls end_if
		{ $$ = buildTree(Else, $2, NULL, NULL); }
	| Elsif expr Then state_decls end_if
		{ $$ = buildTree(Elsif, $2, buildTree(Then, $4, NULL, NULL), NULL); $$->next = $5; }
	;
expr
	: expr Or and_expr
		{ $$ = buildTree(Or, $1, $3, NULL); }
	| and_expr
		{ $$ = $1; }
	;
and_expr
	: and_expr And not_expr
		{ $$ = buildTree(And, $1, $3,  NULL); }
	| not_expr
		{ $$ = $1; }
	;
not_expr
	: Not not_expr
		{ $$ = buildTree(Not, $2, NULL, NULL); }
	| rel_expr
		{ $$ = $1; }
	;
rel_expr
	: sum
		{ $$ = $1; }
	| rel_expr Eq sum
		{ $$ = buildTree(Eq, $1, $3, NULL); }
	| rel_expr Ne sum
		{ $$ = buildTree(Ne, $1, $3, NULL); }
	| rel_expr Lt sum
		{ $$ = buildTree(Lt, $1, $3, NULL); }
	| rel_expr Le sum
		{ $$ = buildTree(Le, $1, $3, NULL); }
	| rel_expr Gt sum
		{ $$ = buildTree(Gt, $1, $3, NULL); }
	| rel_expr Ge sum
		{ $$ = buildTree(Ge, $1, $3, NULL); }
	;
sum
	: prod
		{ $$ = $1; }
	| sum Plus prod
		{ $$ = buildTree(Plus, $1, $3, NULL); }
	| sum Minus prod
		{ $$ = buildTree(Minus, $1, $3, NULL); }
	;
prod
	: factor
		{ $$ = $1; }
	| prod Mult factor
		{ $$ = buildTree(Mult, $1, $3, NULL); }
	| prod Slash factor
		{ $$ = buildTree(Slash, $1, $3, NULL); }
	| prod Div factor
		{ $$ = buildTree(Div, $1, $3, NULL); }
	| prod Mod factor
		{ $$ = buildTree(Mod, $1, $3, NULL); }
	;
factor
	: Plus basic
		{ $$ = buildTree(Plus, $2, NULL, NULL); }
	| Minus basic
		{ $$ = buildTree(Minus, $2, NULL, NULL); }
	| basic
		{ $$ = $1; }
	;
basic
	: ref
		{ $$ = $1; }
	| Lparen expr Rparen
		{ $$ = $2; }
	| IntConst
		{ $$ = buildIntTree(IntConst, $1); }
	| RealConst
		{ $$ = buildIntTree(RealConst, $1); }
	;
%%


%{
/*
 * ====================================================================
 * Practical No : 13
 * Title        : Introduction to YACC and generate a calculator
 *                program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. YACC (Yet Another Compiler Compiler; GNU version = Bison) is a
 *     parser generator: from a context-free grammar with C actions it
 *     produces the C function yyparse().
 *  2. The generated parser is a bottom-up LALR(1) shift-reduce parser
 *     (the same engine as Practical 12, with tables built by YACC).
 *  3. A YACC file has three sections separated by %% :
 *        declarations (%token, %left, %union, C code) %%
 *        grammar rules with actions { ... }            %%
 *        user C routines (yylex, yyerror, main)
 *  4. In an action, $$ is the value of the rule's left side and $1,
 *     $2, $3 ... are the values of the right-side symbols.
 *  5. yyparse() calls yylex() to get the next token (the scanner may
 *     be written by LEX, or by hand as done here) and yyerror() to
 *     report a syntax error.
 *  6. The grammar  expr : expr '+' expr | expr '*' expr | ...  is
 *     ambiguous; precedence declarations resolve it:
 *        %left '+' '-'   (lowest)   %left '*' '/'   %right '^'
 *        %nonassoc UMINUS (unary minus, highest)
 *  7. Later declarations have higher precedence; %left makes an
 *     operator left associative, %right makes it right associative.
 *  8. Each rule computes its value immediately, so the result is
 *     ready when the whole line has been parsed (syntax-directed
 *     translation).
 *  9. Supports + - * / ^ ( ) unary minus and decimal numbers.
 *
 * Build (no LEX needed - the scanner is inside this file):
 *     bison p13_calc.y
 *     gcc p13_calc.tab.c -o p13 -lm
 * Build with a LEX scanner (needs flex) :
 *     flex p13_calc.l
 *     bison -d p13_calc.y
 *     gcc -DUSE_LEX p13_calc.tab.c lex.yy.c -o p13 -lm
 * Run     : ./p13       (type  2+3*4  or  (2+3)*4  then Enter)
 * ====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

int  yylex(void);
void yyerror(const char *msg);
%}

%union { double dval; }

%token <dval> NUMBER
%type  <dval> expr

%left  '+' '-'
%left  '*' '/'
%right '^'
%nonassoc UMINUS

%%
input   : /* empty */
        | input line
        ;

line    : '\n'
        | expr '\n'            { printf("Result = %g\n", $1); }
        | error '\n'           { yyerrok; }
        ;

expr    : expr '+' expr        { $$ = $1 + $3; }
        | expr '-' expr        { $$ = $1 - $3; }
        | expr '*' expr        { $$ = $1 * $3; }
        | expr '/' expr        { if ($3 == 0) {
                                     yyerror("division by zero");
                                     $$ = 0;
                                 } else
                                     $$ = $1 / $3; }
        | expr '^' expr        { $$ = pow($1, $3); }
        | '-' expr %prec UMINUS { $$ = -$2; }
        | '(' expr ')'         { $$ = $2; }
        | NUMBER               { $$ = $1; }
        ;
%%

#ifndef USE_LEX
/* Hand-written scanner: skips blanks, reads numbers, returns other
   characters (operators, brackets, newline) as themselves. */
int yylex(void)
{
    int c;
    while ((c = getchar()) == ' ' || c == '\t')
        ;
    if (c == EOF)
        return 0;                               /* 0 = end of input      */
    if (isdigit(c) || c == '.') {
        ungetc(c, stdin);
        if (scanf("%lf", &yylval.dval) != 1)
            return 0;
        return NUMBER;
    }
    return c;
}
#endif

void yyerror(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
}

int main(void)
{
    printf("YACC calculator  (+ - * / ^ ( ) ), Ctrl+D to quit\n");
    return yyparse();
}

%{
/*
 * ====================================================================
 * Practical No : 13  (optional LEX scanner for the YACC calculator)
 * ====================================================================
 * Use this file ONLY if you want LEX to supply the tokens instead of
 * the hand-written yylex() in p13_calc.y.
 *   1. The scanner returns NUMBER (with its value stored in
 *      yylval.dval) for every number.
 *   2. Blanks and tabs are skipped.
 *   3. Newline is returned as '\n' so the parser knows a line ended.
 *   4. Any other character (+ - * / ^ ( ) ) is returned as itself.
 *   5. p13_calc.tab.h is created by  bison -d  and defines NUMBER
 *      and the YYSTYPE union.
 *
 * Build : flex p13_calc.l
 *         bison -d p13_calc.y
 *         gcc -DUSE_LEX p13_calc.tab.c lex.yy.c -o p13 -lm
 * ====================================================================
 */
#include <stdlib.h>
#include "p13_calc.tab.h"
%}

%option noyywrap

%%
[0-9]+(\.[0-9]+)?   { yylval.dval = atof(yytext); return NUMBER; }
[0-9]*\.[0-9]+      { yylval.dval = atof(yytext); return NUMBER; }
[ \t]+              { /* skip blanks */ }
\n                  { return '\n'; }
.                   { return yytext[0]; }
%%

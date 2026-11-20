/*
 * ====================================================================
 * Practical No : 9
 * Title        : Construct a recursive descent parser for a given
 *                grammar.
 * ====================================================================
 * THEORY & LOGIC
 *  1. A recursive descent parser is a top-down parser built from a set
 *     of mutually recursive functions - ONE function for every
 *     non-terminal of the grammar.
 *  2. It starts with the start symbol and tries to match the input
 *     from left to right, building a leftmost derivation.
 *  3. Left recursion (E -> E+T) would make the functions call
 *     themselves for ever, so the grammar is first rewritten:
 *        E  -> T E'      E' -> + T E' | epsilon
 *        T  -> F T'      T' -> * F T' | epsilon
 *        F  -> ( E ) | id
 *  4. Each function looks at the current input symbol (lookahead):
 *     terminals are matched and consumed, non-terminals are handled
 *     by calling their own functions.
 *  5. For E' and T' the epsilon alternative is chosen when the
 *     lookahead is not '+' / '*'.
 *  6. Every function returns 1 (success) or 0 (syntax error).
 *  7. The input is accepted only if E() succeeds AND the whole string
 *     has been consumed.
 *  8. The program prints the production used at each call, which
 *     shows the leftmost derivation. 'i' stands for id in the input.
 *  9. No backtracking is needed because this grammar is LL(1).
 *
 * Compile : gcc p09_recursive_descent.c -o p09
 * Run     : ./p09       (try:  i+i*i    (i+i)*i    i+*i)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char input[100];        /* cleaned input string               */
int  pos = 0;           /* index of the current lookahead     */

int E(void);  int Ep(void);  int T(void);  int Tp(void);  int F(void);

int E(void)
{
    printf("E  -> T E'\n");
    return T() && Ep();
}

int Ep(void)                                  /* E' -> + T E' | epsilon */
{
    if (input[pos] == '+') {
        printf("E' -> + T E'\n");
        pos++;                                /* match '+'              */
        return T() && Ep();
    }
    printf("E' -> epsilon\n");
    return 1;
}

int T(void)
{
    printf("T  -> F T'\n");
    return F() && Tp();
}

int Tp(void)                                  /* T' -> * F T' | epsilon */
{
    if (input[pos] == '*') {
        printf("T' -> * F T'\n");
        pos++;                                /* match '*'              */
        return F() && Tp();
    }
    printf("T' -> epsilon\n");
    return 1;
}

int F(void)                                   /* F -> ( E ) | id        */
{
    if (input[pos] == '(') {
        printf("F  -> ( E )\n");
        pos++;                                /* match '('              */
        if (!E()) return 0;
        if (input[pos] == ')') {              /* match ')'              */
            pos++;
            return 1;
        }
        return 0;                             /* missing ')'            */
    }
    if (input[pos] == 'i') {
        printf("F  -> id\n");
        pos++;                                /* match id               */
        return 1;
    }
    return 0;                                 /* no alternative fits    */
}

/* remove blanks and turn the word "id" into the single symbol 'i' */
void cleanInput(const char *raw)
{
    int i, j = 0;
    for (i = 0; raw[i] != '\0' && j < 98; i++) {
        if (isspace((unsigned char)raw[i])) continue;
        if (raw[i] == 'i' && raw[i + 1] == 'd') { input[j++] = 'i'; i++; continue; }
        input[j++] = raw[i];
    }
    input[j] = '\0';
}

int main(void)
{
    char line[200];

    printf("Grammar:  E->TE'  E'->+TE'|eps  T->FT'  T'->*FT'|eps  F->(E)|id\n");
    printf("Use 'i' (or the word id) for identifiers.\n");

    for (;;) {
        printf("\nEnter an expression (empty line to quit): ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        cleanInput(line);
        if (input[0] == '\0') break;

        pos = 0;
        printf("\nDerivation steps:\n");
        if (E() && input[pos] == '\0')
            printf("\nResult: \"%s\" is ACCEPTED (valid string)\n", input);
        else
            printf("\nResult: \"%s\" is REJECTED - syntax error at position %d"
                   " (near '%c')\n", input, pos + 1,
                   input[pos] ? input[pos] : '$');
    }
    return 0;
}

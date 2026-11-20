/*
 * ====================================================================
 * Practical No : 11
 * Title        : Implement a C program for operator precedence parsing.
 * ====================================================================
 * THEORY & LOGIC
 *  1. Operator precedence parsing is a bottom-up (shift-reduce) method
 *     for grammars in which no production has two adjacent
 *     non-terminals and no production is epsilon (operator grammars).
 *  2. Grammar used:   E -> E+E | E*E | (E) | id     ('i' = id)
 *  3. Only the terminals (operators) are compared. A precedence table
 *     gives, for the terminal on top of the stack (a) and the next
 *     input terminal (b), one of three relations:
 *        a < b  : b has higher precedence  -> SHIFT
 *        a = b  : same handle (e.g. '(' and ')') -> SHIFT
 *        a > b  : a has higher precedence  -> REDUCE the handle
 *  4. '*' has higher precedence than '+', and both are left
 *     associative, which is why  + > +  and  * > *  in the table.
 *  5. The stack holds terminals and the non-terminal E. To compare,
 *     the topmost TERMINAL on the stack is used, skipping any E.
 *  6. On a '>' relation the handle on top of the stack is reduced:
 *        i -> E,   E+E -> E,   E*E -> E,   (E) -> E
 *  7. '$' marks both the bottom of the stack and the end of the input;
 *     $ < every operator and every operator > $.
 *  8. The string is accepted when both the stack top and the input are
 *     $ and the stack contains just $E.
 *  9. An empty cell in the table means a syntax error.
 *
 * Compile : gcc p11_operator_precedence.c -o p11
 * Run     : ./p11      (try:  i+i*i    (i+i)*i    i+)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* terminal order used for rows and columns of the table */
const char *TERM = "+*i()$";

/*            +    *    i    (    )    $
   row +  :   >    <    <    <    >    >
   row *  :   >    >    <    <    >    >
   row i  :   >    >    x    x    >    >
   row (  :   <    <    <    <    =    x
   row )  :   >    >    x    x    >    >
   row $  :   <    <    <    <    x    A          (x = error, A = accept) */
const char *rel[6] = {
    "><<<>>",
    ">><<>>",
    ">>xx>>",
    "<<<<=x",
    ">>xx>>",
    "<<<<xA"
};

int termIndex(char c)
{
    const char *p = c ? strchr(TERM, c) : NULL;
    return p ? (int)(p - TERM) : -1;
}

/* the topmost terminal on the stack (skips the non-terminal E) */
char topTerminal(const char *st, int sp)
{
    int i;
    for (i = sp - 1; i >= 0; i--)
        if (st[i] != 'E') return st[i];
    return '$';
}

int main(void)
{
    char line[200], in[100], st[100];
    int i, j = 0, sp = 0, ip = 0, step = 1;

    printf("Grammar: E -> E+E | E*E | (E) | id     (write id as i)\n");
    printf("Enter an expression: ");
    if (fgets(line, sizeof(line), stdin) == NULL) return 1;

    for (i = 0; line[i] != '\0' && j < 97; i++) {
        if (isspace((unsigned char)line[i])) continue;
        if (line[i] == 'i' && line[i + 1] == 'd') { in[j++] = 'i'; i++; continue; }      /* id -> i */
        in[j++] = line[i];
    }
    in[j++] = '$';
    in[j] = '\0';

    st[sp++] = '$';
    printf("\n%-5s %-14s %-14s %s\n", "Step", "Stack", "Input", "Action");

    for (;;) {
        char a = topTerminal(st, sp), b = in[ip], r;
        int ra = termIndex(a), rb = termIndex(b);

        st[sp] = '\0';
        printf("%-5d %-14s %-14s ", step++, st, in + ip);

        if (ra < 0 || rb < 0) {
            printf("ERROR: invalid symbol '%c'\n", rb < 0 ? b : a);
            printf("\nResult: REJECTED\n");
            return 0;
        }
        r = rel[ra][rb];

        if (r == 'A') {                                  /* $ and $         */
            if (sp == 2 && st[1] == 'E') {
                printf("ACCEPT\n\nResult: ACCEPTED\n");
            } else {
                printf("ERROR: incomplete expression\n\nResult: REJECTED\n");
            }
            return 0;
        }
        if (r == 'x') {
            printf("ERROR: no relation between '%c' and '%c'\n", a, b);
            printf("\nResult: REJECTED\n");
            return 0;
        }
        if (r == '<' || r == '=') {                      /* SHIFT           */
            printf("%c %c %c  -> shift %c\n", a, r, b, b);
            st[sp++] = b;
            ip++;
        } else {                                         /* REDUCE  (a > b) */
            printf("%c > %c  -> ", a, b);
            if (st[sp - 1] == 'i') {
                printf("reduce i -> E\n");
                st[sp - 1] = 'E';
            } else if (sp >= 4 && st[sp - 1] == 'E' && st[sp - 3] == 'E' &&
                       (st[sp - 2] == '+' || st[sp - 2] == '*')) {
                printf("reduce E%cE -> E\n", st[sp - 2]);
                sp -= 2;                                 /* E op E -> E     */
                st[sp - 1] = 'E';
            } else if (sp >= 4 && st[sp - 1] == ')' && st[sp - 2] == 'E' &&
                       st[sp - 3] == '(') {
                printf("reduce (E) -> E\n");
                sp -= 2;                                 /* ( E ) -> E      */
                st[sp - 1] = 'E';
            } else {
                printf("ERROR: no valid handle on the stack\n");
                printf("\nResult: REJECTED\n");
                return 0;
            }
        }
    }
}

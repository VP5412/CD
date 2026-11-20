/*
 * ====================================================================
 * Practical No : 14
 * Title        : Generate 3-tuple intermediate code for a given infix
 *                expression.
 * ====================================================================
 * THEORY & LOGIC
 *  1. Intermediate code is a machine-independent form between the
 *     parser and the code generator. A "3-tuple" (triple) has three
 *     fields:  ( operator , argument1 , argument2 ).
 *  2. Triples have no separate result name; a result is referred to by
 *     the NUMBER of the triple that computed it, written (n).
 *  3. Step 1 - convert the infix expression into postfix using the
 *     shunting-yard algorithm and an operator stack. Precedence:
 *        ^ (right assoc.)  >  * /  >  + -  >  = (right assoc.)
 *  4. Operands go straight to the output; an operator first pops
 *     every stack operator of higher (or equal, for left associative)
 *     precedence; '(' and ')' control grouping.
 *  5. Step 2 - evaluate the postfix string with a stack: operands are
 *     pushed; for an operator pop two operands, emit one triple and
 *     push its reference (n) back on the stack.
 *  6. Example   a = b + c * d   gives:
 *        (0) ( * , c , d )    (1) ( + , b , (0) )    (2) ( = , a , (1) )
 *  7. Operands may be any letters/digits (a, b2, 10 ...).
 *  8. Operators handled: + - * / ^ = and ( ). Unary minus is not
 *     supported in this simple version.
 *  9. Triples are compact, but moving a triple changes the numbers
 *     others refer to; that is why quadruples are also used.
 *
 * Compile : gcc p14_three_tuple_code.c -o p14
 * Run     : ./p14      (try:  a=b+c*d    (a+b)*(c-d)    x=a*b+c/d-e)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXT 100
#define LEN  16

char postfix[MAXT][LEN];            /* postfix tokens           */
int  pn = 0;

typedef struct {
    char op;
    char a1[LEN], a2[LEN];
} Triple;

Triple tr[MAXT];                    /* the generated triples    */
int tn = 0;

int prec(char c)
{
    switch (c) {
        case '^': return 4;
        case '*':
        case '/': return 3;
        case '+':
        case '-': return 2;
        case '=': return 1;
    }
    return 0;
}

int rightAssoc(char c) { return c == '^' || c == '='; }

/* infix -> postfix; returns 0 on error */
int toPostfix(const char *e)
{
    char opst[MAXT];
    int sp = 0, i = 0;

    while (e[i] != '\0') {
        char c = e[i];
        if (isspace((unsigned char)c)) { i++; continue; }

        if (isalnum((unsigned char)c)) {                 /* operand       */
            int k = 0;
            while (isalnum((unsigned char)e[i]) && k < LEN - 1)
                postfix[pn][k++] = e[i++];
            postfix[pn][k] = '\0';
            pn++;
        } else if (c == '(') {
            opst[sp++] = c;
            i++;
        } else if (c == ')') {
            while (sp > 0 && opst[sp - 1] != '(') {
                postfix[pn][0] = opst[--sp];
                postfix[pn++][1] = '\0';
            }
            if (sp == 0) { printf("Error: unbalanced ')'\n"); return 0; }
            sp--;                                        /* drop '('      */
            i++;
        } else if (strchr("+-*/^=", c)) {                /* operator      */
            while (sp > 0 && opst[sp - 1] != '(' &&
                   (prec(opst[sp - 1]) > prec(c) ||
                    (prec(opst[sp - 1]) == prec(c) && !rightAssoc(c)))) {
                postfix[pn][0] = opst[--sp];
                postfix[pn++][1] = '\0';
            }
            opst[sp++] = c;
            i++;
        } else {
            printf("Error: invalid character '%c'\n", c);
            return 0;
        }
    }
    while (sp > 0) {
        if (opst[sp - 1] == '(') { printf("Error: unbalanced '('\n"); return 0; }
        postfix[pn][0] = opst[--sp];
        postfix[pn++][1] = '\0';
    }
    return 1;
}

/* postfix -> triples; returns 0 on error */
int genTriples(void)
{
    char st[MAXT][LEN];
    int sp = 0, i;

    for (i = 0; i < pn; i++) {
        if (isalnum((unsigned char)postfix[i][0])) {     /* operand       */
            strcpy(st[sp++], postfix[i]);
        } else {                                         /* operator      */
            if (sp < 2) { printf("Error: malformed expression\n"); return 0; }
            tr[tn].op = postfix[i][0];
            strcpy(tr[tn].a2, st[--sp]);                 /* right operand */
            strcpy(tr[tn].a1, st[--sp]);                 /* left operand  */
            snprintf(st[sp++], LEN, "(%d)", tn);         /* reference     */
            tn++;
        }
    }
    if (sp != 1) { printf("Error: malformed expression\n"); return 0; }
    return 1;
}

int main(void)
{
    char expr[200];
    int i;

    printf("Enter an infix expression (e.g. a=b+c*d): ");
    if (fgets(expr, sizeof(expr), stdin) == NULL) return 1;

    if (!toPostfix(expr) || !genTriples()) return 1;

    printf("\nPostfix : ");
    for (i = 0; i < pn; i++) printf("%s ", postfix[i]);

    printf("\n\n%-6s %-10s %-10s %-10s\n", "No.", "Operator", "Arg1", "Arg2");
    printf("--------------------------------------\n");
    for (i = 0; i < tn; i++)
        printf("(%d)    %-10c %-10s %-10s\n", i, tr[i].op, tr[i].a1, tr[i].a2);

    printf("\nTriples as tuples:\n");
    for (i = 0; i < tn; i++)
        printf("  (%d)  ( %c , %s , %s )\n", i, tr[i].op, tr[i].a1, tr[i].a2);
    return 0;
}

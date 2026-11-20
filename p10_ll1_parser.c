/*
 * ====================================================================
 * Practical No : 10
 * Title        : Write a C program for constructing an LL(1) parsing
 *                table and parsing a string with it.
 * ====================================================================
 * THEORY & LOGIC
 *  1. LL(1) means: scan the input Left to right, build a Leftmost
 *     derivation, using 1 symbol of lookahead.
 *  2. A table-driven predictive parser needs a parsing table M[A][a]
 *     (non-terminal x terminal) and a stack.
 *  3. The table is built from FIRST and FOLLOW sets: for every
 *     production A -> alpha
 *       (a) for each terminal a in FIRST(alpha): M[A][a] = A -> alpha
 *       (b) if epsilon is in FIRST(alpha): for each b in FOLLOW(A),
 *           M[A][b] = A -> alpha   (this includes $).
 *  4. If any cell receives two different productions the grammar is
 *     NOT LL(1); the program counts and reports such conflicts.
 *  5. Parsing starts with  $ S  on the stack. Let X be the stack top
 *     and a the current input symbol:
 *       X = a = $          -> accept
 *       X a terminal = a   -> pop X and advance the input
 *       X a non-terminal   -> pop X, push the right side of M[X][a]
 *                             in reverse order (nothing for epsilon)
 *       empty table cell   -> syntax error
 *  6. Pushing in reverse keeps the leftmost symbol on top, so the
 *     input is expanded exactly in leftmost-derivation order.
 *  7. Assumed grammar (R=E', Y=T', i=id, # = epsilon):
 *        E->TR  R->+TR|#  T->FY  Y->*FY|#  F->(E)|i
 *  8. The program prints the table and then every parsing step
 *     (stack, remaining input and action).
 *  9. Time taken is linear in the length of the input.
 *
 * Compile : gcc p10_ll1_parser.c -o p10
 * Run     : ./p10      (try:  i+i*i    (i+i)*i    i+*i)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXP 20
#define MAXS 32

char prod[MAXP][MAXS];
int  np = 0;
char firstSet[128][MAXS];
char followSet[128][MAXS];

const char *NTS = "ERTYF";          /* rows of the table    */
const char *TS  = "+*()i$";         /* columns of the table */
int table[5][6];                    /* production number, -1 = error */

int isNT(char c) { return isupper((unsigned char)c); }

int addSym(char *set, char c)
{
    int n;
    if (strchr(set, c)) return 0;
    n = (int)strlen(set);
    set[n] = c;
    set[n + 1] = '\0';
    return 1;
}

void firstOfString(const char *s, char *out)
{
    int i, allNullable = 1;
    out[0] = '\0';
    for (i = 0; s[i] != '\0' && allNullable; i++) {
        char X = s[i];
        if (X == '#') continue;
        if (!isNT(X)) {
            addSym(out, X);
            allNullable = 0;
        } else {
            int j, hasEps = 0;
            for (j = 0; firstSet[(int)X][j] != '\0'; j++) {
                char c = firstSet[(int)X][j];
                if (c == '#') hasEps = 1;
                else          addSym(out, c);
            }
            if (!hasEps) allNullable = 0;
        }
    }
    if (allNullable) addSym(out, '#');
}

void computeFirst(void)
{
    int p, k, changed;
    char tmp[MAXS];
    do {
        changed = 0;
        for (p = 0; p < np; p++) {
            firstOfString(prod[p] + 2, tmp);
            for (k = 0; tmp[k] != '\0'; k++)
                changed |= addSym(firstSet[(int)prod[p][0]], tmp[k]);
        }
    } while (changed);
}

void computeFollow(void)
{
    int p, i, k, changed;
    char tmp[MAXS];

    addSym(followSet[(int)prod[0][0]], '$');
    do {
        changed = 0;
        for (p = 0; p < np; p++) {
            char A = prod[p][0];
            const char *rhs = prod[p] + 2;
            for (i = 0; rhs[i] != '\0'; i++) {
                char B = rhs[i];
                if (!isNT(B)) continue;
                firstOfString(rhs + i + 1, tmp);
                for (k = 0; tmp[k] != '\0'; k++)
                    if (tmp[k] != '#')
                        changed |= addSym(followSet[(int)B], tmp[k]);
                if (strchr(tmp, '#'))
                    for (k = 0; followSet[(int)A][k] != '\0'; k++)
                        changed |= addSym(followSet[(int)B],
                                          followSet[(int)A][k]);
            }
        }
    } while (changed);
}

void initGrammar(void)
{
    strcpy(prod[np++], "E=TR");
    strcpy(prod[np++], "R=+TR");
    strcpy(prod[np++], "R=#");
    strcpy(prod[np++], "T=FY");
    strcpy(prod[np++], "Y=*FY");
    strcpy(prod[np++], "Y=#");
    strcpy(prod[np++], "F=(E)");
    strcpy(prod[np++], "F=i");
}

int ntIndex(char c)
{
    const char *p = c ? strchr(NTS, c) : NULL;
    return p ? (int)(p - NTS) : -1;
}

int tIndex(char c)
{
    const char *p = c ? strchr(TS, c) : NULL;
    return p ? (int)(p - TS) : -1;
}

/* put production p into cell [row][sym]; returns 1 on a conflict */
int place(int row, char sym, int p)
{
    int col = tIndex(sym);
    if (col < 0) return 0;
    if (table[row][col] != -1 && table[row][col] != p) return 1;
    table[row][col] = p;
    return 0;
}

/* build the LL(1) table, return the number of conflicts found */
int buildTable(void)
{
    int p, k, conflicts = 0;
    char f[MAXS];

    memset(table, -1, sizeof(table));
    for (p = 0; p < np; p++) {
        int row = ntIndex(prod[p][0]);
        firstOfString(prod[p] + 2, f);
        for (k = 0; f[k] != '\0'; k++) {
            if (f[k] != '#') {
                conflicts += place(row, f[k], p);
            } else {                                  /* epsilon: use FOLLOW */
                const char *fo = followSet[(int)prod[p][0]];
                int m;
                for (m = 0; fo[m] != '\0'; m++)
                    conflicts += place(row, fo[m], p);
            }
        }
    }
    return conflicts;
}

void printTable(void)
{
    int r, c;
    char cell[24];

    printf("\nLL(1) PARSING TABLE\n");
    printf("%-4s", "");
    for (c = 0; TS[c] != '\0'; c++) printf("%-10c", TS[c]);
    printf("\n");
    for (r = 0; NTS[r] != '\0'; r++) {
        printf("%-4c", NTS[r]);
        for (c = 0; TS[c] != '\0'; c++) {
            int p = table[r][c];
            if (p < 0) strcpy(cell, "-");
            else       snprintf(cell, sizeof(cell), "%c->%s", prod[p][0], prod[p] + 2);
            printf("%-10s", cell);
        }
        printf("\n");
    }
}

void parseString(const char *in)
{
    char stack[100];
    int sp = 0, step = 1;
    const char *ip = in;

    stack[sp++] = '$';
    stack[sp++] = prod[0][0];

    printf("\n%-5s %-16s %-14s %s\n", "Step", "Stack(top right)", "Input", "Action");
    for (;;) {
        char X = stack[sp - 1], a = *ip;
        stack[sp] = '\0';
        printf("%-5d %-16s %-14s ", step++, stack, ip);

        if (X == '$' && a == '$') {
            printf("ACCEPT\n");
            printf("\nResult: string is ACCEPTED by the LL(1) parser.\n");
            return;
        }
        if (!isNT(X)) {                               /* terminal on top    */
            if (X == a) {
                printf("match %c\n", a);
                sp--;
                ip++;
            } else {
                printf("ERROR: expected '%c' but found '%c'\n", X, a);
                printf("\nResult: string is REJECTED.\n");
                return;
            }
        } else {                                      /* non-terminal       */
            int r = ntIndex(X), c = tIndex(a), p, len, k;
            if (r < 0 || c < 0 || table[r][c] < 0) {
                printf("ERROR: no rule for [%c, %c]\n", X, a);
                printf("\nResult: string is REJECTED.\n");
                return;
            }
            p = table[r][c];
            len = (int)strlen(prod[p] + 2);
            if (sp - 1 + len >= 98) {
                printf("ERROR: stack overflow\n");
                return;
            }
            printf("output %c->%s\n", prod[p][0], prod[p] + 2);
            sp--;                                     /* pop the non-terminal */
            if (prod[p][2] != '#')
                for (k = len - 1; k >= 0; k--)        /* push in reverse      */
                    stack[sp++] = prod[p][2 + k];
        }
    }
}

int main(void)
{
    char line[200], in[100];
    int i, j = 0, conflicts;

    initGrammar();
    computeFirst();
    computeFollow();
    conflicts = buildTable();

    printf("Grammar (R = E', Y = T', i = id, # = epsilon):\n");
    for (i = 0; i < np; i++)
        printf("  %c -> %s\n", prod[i][0], prod[i] + 2);
    printTable();
    if (conflicts)
        printf("\nWarning: %d conflict(s) - grammar is NOT LL(1).\n", conflicts);
    else
        printf("\nNo conflicts - the grammar is LL(1).\n");

    printf("\nEnter an expression using i + * ( ) : ");
    if (fgets(line, sizeof(line), stdin) == NULL) return 1;
    for (i = 0; line[i] != '\0' && j < 97; i++) {
        if (isspace((unsigned char)line[i])) continue;
        if (line[i] == 'i' && line[i + 1] == 'd') { in[j++] = 'i'; i++; continue; }    /* id -> i */
        in[j++] = line[i];
    }
    in[j++] = '$';
    in[j] = '\0';

    parseString(in);
    return 0;
}

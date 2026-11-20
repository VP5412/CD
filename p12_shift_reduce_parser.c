/*
 * ====================================================================
 * Practical No : 12
 * Title        : Given a parsing table, parse the given input using a
 *                Shift-Reduce parser for an unambiguous grammar.
 * ====================================================================
 * THEORY & LOGIC
 *  1. A shift-reduce parser is a bottom-up parser: it builds the parse
 *     tree from the leaves (input) up to the root (start symbol),
 *     producing a rightmost derivation in reverse.
 *  2. It uses a stack and an input buffer and has four actions:
 *     SHIFT (push next input symbol and a new state), REDUCE (replace
 *     a handle on the stack by the left side of a production),
 *     ACCEPT and ERROR.
 *  3. The decisions come from an SLR(1) parsing table with an ACTION
 *     part (state x terminal) and a GOTO part (state x non-terminal).
 *  4. Grammar (unambiguous):  1) E->E+T  2) E->T  3) T->T*F  4) T->F
 *     5) F->(E)   6) F->id      ('i' stands for id).
 *  5. sN means "shift and go to state N"; rN means "reduce by
 *     production N"; acc means accept; a blank cell is an error.
 *  6. To reduce by A -> beta, pop 2*|beta| items (symbol + state
 *     pairs), look at the state now on top, push A and the state
 *     given by GOTO[state, A].
 *  7. The parser reads the state on top and the next input symbol,
 *     finds the action in the table and repeats until acc or error.
 *  8. Every step (stack, remaining input, action) is printed, so the
 *     reductions in reverse give the rightmost derivation.
 *  9. This is the engine used by YACC/Bison generated parsers.
 *
 * Compile : gcc p12_shift_reduce_parser.c -o p12
 * Run     : ./p12      (try:  i+i*i    (i+i)*i    i*+i)
 * ====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *TERMS  = "i+*()$";      /* ACTION columns */
const char *NTERMS = "ETF";         /* GOTO columns   */

/* ACTION table: 12 states x 6 terminals  ("" = error) */
const char *action[12][6] = {
/*          i      +      *      (      )      $    */
/* 0  */ { "s5",  "",    "",    "s4",  "",    ""    },
/* 1  */ { "",    "s6",  "",    "",    "",    "acc" },
/* 2  */ { "",    "r2",  "s7",  "",    "r2",  "r2"  },
/* 3  */ { "",    "r4",  "r4",  "",    "r4",  "r4"  },
/* 4  */ { "s5",  "",    "",    "s4",  "",    ""    },
/* 5  */ { "",    "r6",  "r6",  "",    "r6",  "r6"  },
/* 6  */ { "s5",  "",    "",    "s4",  "",    ""    },
/* 7  */ { "s5",  "",    "",    "s4",  "",    ""    },
/* 8  */ { "",    "s6",  "",    "",    "s11", ""    },
/* 9  */ { "",    "r1",  "s7",  "",    "r1",  "r1"  },
/* 10 */ { "",    "r3",  "r3",  "",    "r3",  "r3"  },
/* 11 */ { "",    "r5",  "r5",  "",    "r5",  "r5"  }
};

/* GOTO table: 12 states x 3 non-terminals (E, T, F), -1 = none */
int gotoTbl[12][3] = {
    { 1, 2, 3}, {-1,-1,-1}, {-1,-1,-1}, {-1,-1,-1},
    { 8, 2, 3}, {-1,-1,-1}, {-1, 9, 3}, {-1,-1,10},
    {-1,-1,-1}, {-1,-1,-1}, {-1,-1,-1}, {-1,-1,-1}
};

/* productions 1..6 : left side, length of right side, text */
const char  lhs[7]       = {0, 'E', 'E', 'T', 'T', 'F', 'F'};
const int   rlen[7]      = {0, 3, 1, 3, 1, 3, 1};
const char *prodText[7]  = {"", "E->E+T", "E->T", "T->T*F",
                            "T->F", "F->(E)", "F->id"};

int main(void)
{
    char line[200], in[100];
    int  st[100];               /* state stack  */
    char sy[100];               /* symbol stack (sy[k] was pushed with st[k]) */
    int  sp = 0, step = 1, i, j = 0;
    const char *ip;

    printf("Grammar: 1) E->E+T  2) E->T  3) T->T*F  4) T->F  5) F->(E)  6) F->id\n");
    printf("Enter an expression (write id as i): ");
    if (fgets(line, sizeof(line), stdin) == NULL) return 1;

    for (i = 0; line[i] != '\0' && j < 97; i++) {
        if (isspace((unsigned char)line[i])) continue;
        if (line[i] == 'i' && line[i + 1] == 'd') { in[j++] = 'i'; i++; continue; }       /* id -> i */
        in[j++] = line[i];
    }
    in[j++] = '$';
    in[j] = '\0';
    ip = in;

    st[0] = 0;
    sy[0] = ' ';
    printf("\n%-5s %-26s %-12s %s\n", "Step", "Stack", "Input", "Action");

    for (;;) {
        int  s = st[sp];
        const char *pos = strchr(TERMS, *ip);
        const char *act;
        char stackStr[160] = "";
        int  k;

        /* build the printable stack:  0 E 1 + 6 ... */
        for (k = 0; k <= sp; k++) {
            char piece[16];
            if (k > 0) {
                snprintf(piece, sizeof(piece), "%c ", sy[k]);
                strcat(stackStr, piece);
            }
            snprintf(piece, sizeof(piece), "%d ", st[k]);
            strcat(stackStr, piece);
        }
        printf("%-5d %-26s %-12s ", step++, stackStr, ip);

        if (pos == NULL || *ip == '\0') {
            printf("ERROR: invalid symbol '%c'\n\nResult: REJECTED\n", *ip);
            return 0;
        }
        act = action[s][pos - TERMS];

        if (act[0] == '\0') {                               /* error cell   */
            printf("ERROR: no action in state %d for '%c'\n", s, *ip);
            printf("\nResult: REJECTED\n");
            return 0;
        }
        if (strcmp(act, "acc") == 0) {                      /* accept       */
            printf("ACCEPT\n\nResult: ACCEPTED\n");
            return 0;
        }
        if (act[0] == 's') {                                /* shift        */
            int ns = atoi(act + 1);
            printf("Shift %c, go to state %d\n", *ip, ns);
            sp++;
            sy[sp] = *ip;
            st[sp] = ns;
            ip++;
        } else {                                            /* reduce       */
            int p = atoi(act + 1), t, col, ns;
            printf("Reduce by %s\n", prodText[p]);
            sp -= rlen[p];                                  /* pop handle   */
            t = st[sp];
            col = (int)(strchr(NTERMS, lhs[p]) - NTERMS);
            ns = gotoTbl[t][col];
            if (ns < 0) {
                printf("ERROR: GOTO[%d,%c] is empty\n\nResult: REJECTED\n",
                       t, lhs[p]);
                return 0;
            }
            sp++;
            sy[sp] = lhs[p];                                /* push A       */
            st[sp] = ns;                                    /* new state    */
        }
    }
}

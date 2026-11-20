/*
 * ====================================================================
 * Practical No : 1(a)
 * Title        : Write a C program to remove all the comments from a
 *                C program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. Comment removal is the first job of the compiler's lexical phase:
 *     comments are meant for humans, so the scanner discards them
 *     before tokens are produced.
 *  2. C has two comment forms: single-line  // ...  and multi-line
 *     / * ... * /  (the second form does not nest).
 *  3. The program is a small finite-state machine (DFA) with the
 *     states: NORMAL, SLASH, LINE_COMMENT, BLOCK_COMMENT, BLOCK_STAR,
 *     IN_STRING and IN_CHAR.
 *  4. In NORMAL state a '/' moves to SLASH; the next character decides
 *     whether it was a comment start ('/' or '*') or an ordinary
 *     division sign (which is then printed).
 *  5. In LINE_COMMENT everything up to the newline is skipped; the
 *     newline itself is kept so line numbers do not change.
 *  6. In BLOCK_COMMENT characters are skipped until the pair '*' '/'
 *     is seen (tracked with the BLOCK_STAR state).
 *  7. IN_STRING / IN_CHAR make sure that text such as "http://x" or
 *     '/' is NOT mistaken for a comment; escape characters (\") are
 *     handled so a quote inside a string does not end it.
 *  8. A block comment is replaced by one space so that  a/ * c * /b
 *     does not glue the two tokens into  ab.
 *  9. Time complexity is O(n): each input character is read once.
 *
 * Compile : gcc p01a_remove_comments.c -o p01a
 * Run     : ./p01a sample_inputs/with_comments.c  [output_file]
 *           (prints to the screen when no output file is given)
 * ====================================================================
 */
#include <stdio.h>
#include <stdlib.h>

/* The states of the DFA */
enum State { NORMAL, SLASH, LINE_COMMENT, BLOCK_COMMENT, BLOCK_STAR,
             IN_STRING, IN_CHAR };

int main(int argc, char *argv[])
{
    FILE *in, *out = stdout;
    int c;
    enum State state = NORMAL;

    if (argc < 2) {
        printf("Usage: %s <input.c> [output.c]\n", argv[0]);
        return 1;
    }
    in = fopen(argv[1], "r");
    if (in == NULL) {
        perror("Cannot open input file");
        return 1;
    }
    if (argc >= 3) {
        out = fopen(argv[2], "w");
        if (out == NULL) {
            perror("Cannot open output file");
            fclose(in);
            return 1;
        }
    }

    while ((c = fgetc(in)) != EOF) {
        switch (state) {

        case NORMAL:
            if (c == '/')       state = SLASH;          /* maybe a comment */
            else if (c == '"') { fputc(c, out); state = IN_STRING; }
            else if (c == '\''){ fputc(c, out); state = IN_CHAR;   }
            else                 fputc(c, out);
            break;

        case SLASH:
            if (c == '/')       state = LINE_COMMENT;   /* // comment      */
            else if (c == '*')  state = BLOCK_COMMENT;  /* start of / *    */
            else {                                      /* plain division  */
                fputc('/', out);
                if (c == '"')       { fputc(c, out); state = IN_STRING; }
                else if (c == '\'') { fputc(c, out); state = IN_CHAR;   }
                else if (c == '/')  state = SLASH;      /* not reachable   */
                else { fputc(c, out); state = NORMAL; }
            }
            break;

        case LINE_COMMENT:
            if (c == '\n') {                            /* keep the newline */
                fputc(c, out);
                state = NORMAL;
            }
            break;

        case BLOCK_COMMENT:
            if (c == '*') state = BLOCK_STAR;
            else if (c == '\n') fputc(c, out);          /* preserve lines   */
            break;

        case BLOCK_STAR:
            if (c == '/')       { fputc(' ', out); state = NORMAL; }
            else if (c == '*')  state = BLOCK_STAR;     /* ** stays here    */
            else {
                if (c == '\n') fputc(c, out);
                state = BLOCK_COMMENT;
            }
            break;

        case IN_STRING:
            fputc(c, out);
            if (c == '\\') {                            /* escaped char     */
                int next = fgetc(in);
                if (next != EOF) fputc(next, out);
            } else if (c == '"')
                state = NORMAL;
            break;

        case IN_CHAR:
            fputc(c, out);
            if (c == '\\') {
                int next = fgetc(in);
                if (next != EOF) fputc(next, out);
            } else if (c == '\'')
                state = NORMAL;
            break;
        }
    }

    if (state == SLASH) fputc('/', out);                /* file ended on '/' */
    if (state == BLOCK_COMMENT || state == BLOCK_STAR)
        fprintf(stderr, "Warning: unterminated comment in input.\n");

    fclose(in);
    if (out != stdout) fclose(out);
    return 0;
}

# Compiler Design (CD) — Practical Programs

Every program starts with a comment block that gives the **theory and logic in 8-10 lines**,
followed by line-by-line comments in the code. Sample input files are in `sample_inputs/`.

## Tools needed
| Tool | Used for | Check with |
|------|----------|------------|
| `gcc` | all C programs | `gcc --version` |
| `flex` (or `lex`) | practicals 3(b) to 6(d) (and the optional scanner in 13) | `flex --version` |
| `bison` (or `yacc`) | practical 13 | `bison --version` |

Ubuntu / WSL: `sudo apt install gcc flex bison`

## Index, build and run

| No. | Practical | File | Build and run |
|-----|-----------|------|---------------|
| 1a | Remove comments from a C program | `p01a_remove_comments.c` | `gcc p01a_remove_comments.c -o p01a` then `./p01a sample_inputs/with_comments.c` |
| 1b | Recognize identifiers and numbers | `p01b_identifiers_numbers.c` | `gcc p01b_identifiers_numbers.c -o p01b` then `./p01b` |
| 2 | Token generator for a C program | `p02_token_generator.c` | `gcc p02_token_generator.c -o p02` then `./p02 sample_inputs/sample.c` |
| 3a | Study of LEX | `p03a_lex_study.md` | (theory notes) |
| 3b | Count characters, words, lines of a file | `p03b_count_chars_words_lines.l` | `flex p03b_count_chars_words_lines.l` then `gcc lex.yy.c -o p03b` then `./p03b sample_inputs/sample_text.txt` |
| 4a | `yytext` | `p04a_yytext.l` | flex + gcc as above, run `./p04a` |
| 4b | `ECHO`, `REJECT` | `p04b_echo_reject.l` | flex + gcc, run `./p04b` (type `abcd`) |
| 4c | `BEGIN` directive | `p04c_begin_directive.l` | flex + gcc, run `./p04c` |
| 5a | Count vowels and consonants | `p05a_vowels_consonants.l` | flex + gcc, run `./p05a` |
| 5b | Print all numbers from a file | `p05b_print_numbers.l` | flex + gcc, run `./p05b sample_inputs/numbers.txt` |
| 5c | Count comment lines in a C program | `p05c_count_comment_lines.l` | flex + gcc, run `./p05c sample_inputs/with_comments.c` |
| 6a | `unput`, `input` | `p06a_unput_input.l` | flex + gcc, run `./p06a` |
| 6b | `yyterminate`, `yy_flush_buffer` | `p06b_yyterminate_flush.l` | flex + gcc, run `./p06b` |
| 6c | `yywrap` | `p06c_yywrap.l` | flex + gcc, run `./p06c sample_inputs/file1.txt sample_inputs/file2.txt` |
| 6d | `yymore`, `yyless` | `p06d_yymore_yyless.l` | flex + gcc, run `./p06d` (try `well-known Program`) |
| 7 | FIRST set | `p07_first_set.c` | `gcc p07_first_set.c -o p07` then `./p07` |
| 8 | FOLLOW set | `p08_follow_set.c` | `gcc p08_follow_set.c -o p08` then `./p08` |
| 9 | Recursive descent parser | `p09_recursive_descent.c` | `gcc p09_recursive_descent.c -o p09` then `./p09` |
| 10 | LL(1) parsing table and parser | `p10_ll1_parser.c` | `gcc p10_ll1_parser.c -o p10` then `./p10` |
| 11 | Operator precedence parser | `p11_operator_precedence.c` | `gcc p11_operator_precedence.c -o p11` then `./p11` |
| 12 | Shift-reduce parser using a parsing table | `p12_shift_reduce_parser.c` | `gcc p12_shift_reduce_parser.c -o p12` then `./p12` |
| 13 | YACC calculator | `p13_calc.y` (+ optional `p13_calc.l`) | `bison p13_calc.y` then `gcc p13_calc.tab.c -o p13 -lm` then `./p13` |
| 14 | 3-tuple (triple) intermediate code | `p14_three_tuple_code.c` | `gcc p14_three_tuple_code.c -o p14` then `./p14` |
| 15 | Predecessor and successor from a CFG | `p15_cfg_pred_succ.c` | `gcc p15_cfg_pred_succ.c -o p15` then `./p15 < sample_inputs/cfg_input.txt` |

For practical 13 with a LEX scanner:
`flex p13_calc.l` , `bison -d p13_calc.y` , `gcc -DUSE_LEX p13_calc.tab.c lex.yy.c -o p13 -lm`

## Grammar used in practicals 7-12
```
E -> T R        R -> + T R | #        (R = E',  # = epsilon)
T -> F Y        Y -> * F Y | #        (Y = T')
F -> ( E ) | i                        (i = id)
```
Practicals 11, 12 and 13 use their own expression grammars, written in the header comment of each file.

## Expected outputs (quick check)
* 7: `FIRST(E) = { (, i }`, `FIRST(R) = { +, eps }`
* 8: `FOLLOW(E) = { $, ) }`, `FOLLOW(F) = { *, +, $, ) }`
* 10, 11, 12: input `i+i*i` is ACCEPTED, `i+*i` is REJECTED
* 13: `2+3*4` gives `Result = 14`
* 14: `a=b+c*d` gives `(0) (*,c,d)  (1) (+,b,(0))  (2) (=,a,(1))`
* 15: with `cfg_input.txt`, B2 has predecessors B1, B3 and successors B3, B4

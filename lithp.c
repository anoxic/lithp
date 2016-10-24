#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

long eval_fn(long x, char* fn, long y) {
    if (strcmp(fn, "+") == 0) { return x + y; }
    if (strcmp(fn, "-") == 0) { return x - y; }
    if (strcmp(fn, "*") == 0) { return x * y; }
    if (strcmp(fn, "/") == 0) { return x / y; }
    if (strcmp(fn, "%") == 0) { return x % y; }
    if (strcmp(fn, "^") == 0) { return pow(x, y); }
    if (strcmp(fn, "min") == 0) { return fmin(x, y); }
    if (strcmp(fn, "max") == 0) { return fmax(x, y); }
    return 0;
}

long eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    char* op = t->children[1]->contents;
    long  x  = eval(t->children[2]);

    /* Run function for 2 or more arguments */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_fn(x, op, eval(t->children[i]));
        i++;
    }

    /* With only one argument, - negates */
    if (strstr(op, "-") && i == 3) {
        x = -x;
    }

    return x;
}

int main(int argc, char** argv) {
    /* Parsers */
    mpc_parser_t* Integer  = mpc_new("integer");
    mpc_parser_t* Decimal  = mpc_new("decimal");
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Fn       = mpc_new("fn");
    mpc_parser_t* Lithp    = mpc_new("lithp");

    /* Language (Parser Definition) */
    mpca_lang(MPCA_LANG_DEFAULT,
        "  integer  : /-?[0-9]+/ ;                            "
        "  decimal  : /-?[0-9]+\\.[0-9]+/ ;                   "
        "  number   : <decimal> | <integer>  ;                "
        "  operator : '+' | '-' | '*' | '/' | '%' | '^' ;     "
        "  fn       : <operator> | \"min\" | \"max\" ;        "
        "  expr     : <number> | '(' <fn> <expr>+ ')' ;       "
        "  lithp    : /^/ <fn> <expr>+ /$/ ;                  ",
        Integer, Decimal, Number, Operator, Fn, Expr, Lithp);

    puts("Lithp Version 0.0.0.0.0.0.0.2");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("lithp> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lithp, &r)) {
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    /* Undefine and Delete our Parsers */
    mpc_cleanup(7, Integer, Decimal, Number, Operator, Fn, Expr, Lithp);

    return 0;
}


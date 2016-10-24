#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

typedef struct {
    enum { LVAL_INTEGER, LVAL_ERR } type;
    union {
        long integer;
        int err;
    } v;
} lval;

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_INTEGER };

lval lval_integer(long x) {
    lval v;
    v.type = LVAL_INTEGER;
    v.v.integer = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.v.err = x;
    return v;
}

void lval_print(lval v) {
    switch (v.type) {
        case LVAL_INTEGER: printf("%li", v.v.integer); break;

        case LVAL_ERR: 
        switch (v.v.err) {
            case LERR_DIV_ZERO: printf("Error: division by zero"); break;
            case LERR_BAD_OP: printf("Error: invalid operator"); break;
            case LERR_BAD_INTEGER: printf("Error: invalid integer"); break;
        }
        break;

        default: puts("Error: unknown error type"); break;
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval eval_fn(lval x, char* fn, lval y) {

    /* If either input is an error, return it */
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    if (strcmp(fn, "min") == 0) { return lval_integer(fmin(x.v.integer, y.v.integer)); }
    if (strcmp(fn, "max") == 0) { return lval_integer(fmax(x.v.integer, y.v.integer)); }
    if (strcmp(fn, "+") == 0) { return lval_integer(x.v.integer + y.v.integer); }
    if (strcmp(fn, "-") == 0) { return lval_integer(x.v.integer - y.v.integer); }
    if (strcmp(fn, "*") == 0) { return lval_integer(x.v.integer * y.v.integer); }
    if (strcmp(fn, "%") == 0) { return lval_integer(x.v.integer % y.v.integer); }
    if (strcmp(fn, "^") == 0) { return lval_integer(pow(x.v.integer, y.v.integer)); }
    if (strcmp(fn, "/") == 0) {
        return y.v.integer == 0 
            ? lval_err(LERR_DIV_ZERO) 
            : lval_integer(x.v.integer / y.v.integer);
    }

    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
    if (strstr(t->tag, "integer")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_integer(x) : lval_err(LERR_BAD_INTEGER);
    }

    char* op = t->children[1]->contents;
    lval  x  = eval(t->children[2]);

    /* Run function for 2 or more arguments */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_fn(x, op, eval(t->children[i]));
        i++;
    }

    /* With only one argument, - negates */
    if (strstr(op, "-") && i == 3) {
        x = lval_integer(-x.v.integer);
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
            lval result = eval(r.output);
            lval_println(result);
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


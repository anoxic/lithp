#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

typedef struct {
    enum { LVAL_INTEGER, LVAL_DECIMAL, LVAL_ERR } type;
    union {
        long integer;
        long double decimal;
        int err;
    } v;
} lval;

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_INTEGER, LERR_BAD_DECIMAL };

lval lval_integer(long x) {
    lval v;
    v.type = LVAL_INTEGER;
    v.v.integer = x;
    return v;
}

lval lval_decimal(long double x) {
    lval v;
    v.type = LVAL_DECIMAL;
    v.v.decimal = x;
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

        case LVAL_DECIMAL: printf("%Lg", v.v.decimal); break;

        case LVAL_ERR: 
        switch (v.v.err) {
            case LERR_DIV_ZERO: printf("Error: division by zero"); break;
            case LERR_BAD_OP: printf("Error: invalid operator"); break;
            case LERR_BAD_INTEGER: printf("Error: invalid integer"); break;
            case LERR_BAD_DECIMAL: printf("Error: invalid decimal"); break;
        }
        break;

        default: puts("Error: unknown error type"); break;
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval lval_convertnumber(lval x, int type) {
    if (type == LVAL_DECIMAL) {
        long double v = x.type == LVAL_DECIMAL ? x.v.decimal : x.v.integer;
        return lval_decimal(v);
    }
    long v = x.type==LVAL_DECIMAL ? x.v.decimal : x.v.integer;
    return lval_integer(v);
}

lval lval_ldtonumber(long double v, int type) {
    return type == LVAL_DECIMAL ? lval_decimal(v) : lval_integer(v);
}

lval eval_fn(lval x, char* fn, lval y) {
    int type;
    long double xc,yc;

    /* If either input is an error, return it */
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    if (x.type == LVAL_DECIMAL || y.type == LVAL_DECIMAL) {
        type = LVAL_DECIMAL;
        xc = lval_convertnumber(x,LVAL_DECIMAL).v.decimal;
        yc = lval_convertnumber(y,LVAL_DECIMAL).v.decimal;
    } else {
        type = LVAL_INTEGER;;
        xc = lval_convertnumber(x,LVAL_INTEGER).v.integer;
        yc = lval_convertnumber(y,LVAL_INTEGER).v.integer;
    }

    if (strcmp(fn, "min") == 0) { return lval_ldtonumber(fmin(xc,yc),type); }
    if (strcmp(fn, "max") == 0) { return lval_ldtonumber(fmax(xc,yc),type); }
    if (strcmp(fn, "+") == 0) { return lval_ldtonumber(xc + yc,type); }
    if (strcmp(fn, "-") == 0) { return lval_ldtonumber(xc - yc,type); }
    if (strcmp(fn, "*") == 0) { return lval_ldtonumber(xc * yc,type); }
    if (strcmp(fn, "%") == 0) { return lval_ldtonumber((long)xc % (long)yc,type); }
    if (strcmp(fn, "^") == 0) { return lval_ldtonumber(pow(xc, yc),type); }
    if (strcmp(fn, "/") == 0) {
        return yc == 0 
            ? lval_err(LERR_DIV_ZERO) 
            : lval_ldtonumber(xc / yc, type);
    }

    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
    if (strstr(t->tag, "decimal")) {
        errno = 0;
        long double x = strtold(t->contents, NULL);
        return errno != ERANGE ? lval_decimal(x) : lval_err(LERR_BAD_DECIMAL);
    }

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


#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv) {
    /* Parsers */
    mpc_parser_t* Integer  = mpc_new("integer");
    mpc_parser_t* Decimal  = mpc_new("decimal");
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Lithp    = mpc_new("lithp");

    /* Language (Parser Definition) */
    mpca_lang(MPCA_LANG_DEFAULT,
        "   integer  : /-?[0-9]+/ ;                            "
        "   decimal  : /-?[0-9]+\\.[0-9]+/ ;                   "
        "   number   : <decimal> | <integer>  ;                "
        "   operator : '+' | '-' | '*' | '/' | '%' ;           "
        "   expr     : <number> | '(' <operator> <expr>+ ')' ; "
        "   lithp    : /^/ <operator> <expr>+ /$/ ;            ",
        Integer, Decimal, Number, Operator, Expr, Lithp);

    puts("Lithp Version 0.0.0.0.0.0.0.2");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("lithp> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lithp, &r)) {
            /* On Success Print the AST */
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            /* Otherwise Print the Error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    /* Undefine and Delete our Parsers */
    mpc_cleanup(4, Number, Operator, Expr, Lithp);

    return 0;
}


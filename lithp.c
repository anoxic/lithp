#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv) {
    puts("Lithp Version 0.0.0.0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("lithp> ");
        add_history(input);
        printf("What does %s mean?\n", input);
        free(input);
    }

    return 0;
}

// http://www.buildyourownlisp.com/chapter5_languages

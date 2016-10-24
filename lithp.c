#include <stdio.h>

static char input_buffer[2048];

int main(int argc, char** argv) {
    puts("Lithp Version 0.0.0.0.0.0.0.1");
    puts("Press Ctrl+c to Exit");

    while (1) {
        fputs("lispy> ", stdout);
        fgets(input_buffer, 2048, stdin);
        printf("No you're a %s", input_buffer);
    }

    return 0;
}

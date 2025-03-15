// cowsay_command
#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include "../include/lib/string.h"

char cowsays[400] = "\n\n"
                    "    ^__^\n"
                    "    (oo)_______\n"
                    "    (__)       )>\n"
                    "       II---w II\n"
                    "       II     II\n";


                  

void cowsay_command(const char *args) {
    int arg_len = 0;
    // Check if arguments are NULL or empty
    if (args == NULL || *args == '\0') {
        vga_puts("cowsay: missing argument\nUsage: cowsay <text>\n");
        return;
    }

    // Skip leading whitespace
    while (*args == ' ') args++;

    // Check if the entire input is whitespace
    if (*args == '\0') {
        vga_putchar('\n');
        return;
    }

    vga_puts("< ");
    // Handle quoted arguments (both single and double quotes)
    if (*args == '\'' || *args == '"') {
        char quote = *args;
        args++; // Move past the opening quote

        // Print characters until the closing quote is found
        while (*args != '\0' && *args != quote) {
            vga_putchar(*args); // Use vga_putchar instead of vga_putc
            arg_len++;
            args++;
        }

        // If the closing quote is missing, print an error
        if (*args != quote) {
            vga_puts("\necho: missing closing quote\n");
            return;
        }

        args++; // Move past the closing quote
    } else {
        // Print the rest of the arguments as-is
        while (*args != '\0') {
            vga_putchar(*args); // Use vga_putchar instead of vga_putc
            arg_len++;
            args++;
        }

        vga_puts(" >");
        
    }

    

    
    vga_puts("\n");

    for(int i = 0; i < arg_len + 4; i++) {
        vga_puts("-");
    }

    
    
    vga_puts(cowsays);



    vga_putchar('\n'); // Use vga_putchar for newline
}

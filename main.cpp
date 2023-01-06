#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "LabelScanner.h"
#include "interpreter.h"

using namespace std;

const int PROG_SIZE = 1024 * 1024 * 16;

char PROG_BUFFER[PROG_SIZE];

int load_program(char *p, char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("%s %s", "Failed to open file.", file_name);
        return -1;
    }

    int i = 0;
    while (i < PROG_SIZE)
    {
        *p = fgetc(fp);
        if (feof(fp))
        {
            break;
        }
        p++;
        i++;
    }
    *p = '\0';
    return 0;
}

int main(int argc, char *argv[])
{
    memset((void *)PROG_BUFFER, 0, PROG_SIZE);

    if (argc != 2)
    {
        printf("%s", "Please input filename.");
    }

    char *file_name = argv[1];
    char *prog = PROG_BUFFER;
    load_program(prog, file_name);
    char *pc = prog;
    while (*pc != '\0')
    {
        printf("%c", *pc);
        pc += 1;
    }
    LabelScanner *label_scanner = LabelScanner::GetInstance(prog);
    label_scanner->scan_labels();
    label_scanner->show_label();
    Interpreter(prog).run();
}

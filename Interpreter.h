#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "tokenizer.h"

class Interpreter
{
public:
    Interpreter(char *p);

    void run();
    void exec_assignment();
    void exec_keyword();
    void exec_print();
    void exec_for();
    void exec_next();
    void exec_gosub();
    void exec_return();
    void exec_goto();
    void exec_if();


private:
    Tokenizer tokenizer;
    Token token;
};


#endif
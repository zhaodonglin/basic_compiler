#ifndef FORSTACK_H
#define FORSTACK_H

#include "tokenizer.h"
#include <stack>
using namespace std;

struct for_stack_element
{
    Tokenizer tokenizer;
    char count_variable[TOKEN_SIZE];
    int target;

    for_stack_element(Tokenizer tokenizer, int target, char *variable_name) {
        this->target = target;
        this->tokenizer = tokenizer;
        strncpy(this->count_variable, variable_name, TOKEN_SIZE);
    }
} ;


class ForStack {
    public:
        void push(Tokenizer tokenizer, int target, char *variable_name);
        for_stack_element* top();
        void pop();
        static ForStack *GetInstance();

    private:
        stack<for_stack_element*> for_stack;
        static ForStack * instance;
};
#endif



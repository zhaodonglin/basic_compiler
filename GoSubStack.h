#ifndef GOSUBSTACK_H
#define GOSUBSTACK_H
#include <stack>
#include "Tokenizer.h"
using namespace std;

class GoSubStack{

private:
    stack<Tokenizer> _stack;
    static GoSubStack * instance;

public:
    void push(Tokenizer tokenizer) {
        _stack.push(tokenizer);
    }

    void pop() {
        _stack.pop();
    }

    Tokenizer top() {
        return _stack.top();
    }

    static GoSubStack *GetInstance();
};

#endif
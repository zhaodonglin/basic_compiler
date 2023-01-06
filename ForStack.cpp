#include "ForStack.h"

ForStack* ForStack::instance = nullptr;
ForStack* ForStack::GetInstance() {
    if (instance == nullptr) {
        instance = new ForStack();
    }
    return instance;
}

void ForStack::push(Tokenizer tokenizer, int target, char *variable_name) {
   for_stack_element* element = new for_stack_element(tokenizer, target, variable_name);

   for_stack.push(element);
}

for_stack_element* ForStack::top() {
    return for_stack.top();
}

void ForStack::pop() {
    delete for_stack.top();
    for_stack.pop();
}


#include <stdio.h>
#include <stdlib.h>
#include "Interpreter.h"
#include "Expression.h"
#include "VariableTable.h"
#include "ForStack.h"
#include "LabelScanner.h"
#include "GoSubStack.h"

Interpreter::Interpreter(char *p) : tokenizer(p)
{
    token = tokenizer.get_token();
}

void Interpreter::run()
{
    TOKEN_TYPE token_type = token.get_type();
    while (true)
    {
        // the first token should be NUMBER or KEYWORD or VARIABLE
        // NUMBER is the label, label is processed in the first traverse.
        if (token_type == IDENTIFIER)
        {
            exec_assignment();
        }
        else if (token_type == KEYWORD)
        {
            exec_keyword();
        }
        token = tokenizer.get_token();
        token_type = token.get_type();
        if (token_type == EOP) {
            exit(1);
        }
    };
}

void Interpreter::exec_assignment()
{
    char name[TOKEN_SIZE];
    strncpy(name, token.get_data(), TOKEN_SIZE);

    token = tokenizer.get_token();
    if ((token.get_type() != OPERATOR) || (strcmp(token.get_data(), "=") != 0))
    {
        printf("%s", "Invalid assignment, expect '=' ");
        exit(-1);
    }

    int value = Expression(&tokenizer).evaluate();
    VariableTable *variable_table = VariableTable::GetInstance();
    variable_table->save(name, value);

    return;
}

void Interpreter::exec_print()
{
    token = tokenizer.get_token();
    TOKEN_TYPE token_type = token.get_type();
    do
    {

        if (token_type == QUOTE)
        {
            printf("%s", token.get_data());
        }
        else if (token_type == DELIMETER)
        {
            char *data = token.get_data();
            if (strcmp(data, ",") == 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    printf(" ");
                }
            }
            else if (strcmp(data, ";") == 0)
            {
                printf(" ");
            }
        }
        else if (token_type == LINE_BREAK || token_type == END)
        {
            break;
        }
        else
        {
            tokenizer.rollback(&token);

            Expression expression = Expression(&tokenizer);
            int value = expression.evaluate();
            printf("%s", "-------------------");
            printf("\n%d\n", value);
        }
        token = tokenizer.get_token();
        token_type = token.get_type();

    } while (token_type != END && token_type != LINE_BREAK);

}

void Interpreter::exec_for()
{
    token = tokenizer.get_token();
    char name[TOKEN_SIZE];
    if (token.get_type() != IDENTIFIER)
    {
        printf("%s", "invalid for, expect variable.");
        exit(-1);
    }
    strncpy(name, token.get_data(), TOKEN_SIZE);

    token = tokenizer.get_token();
    if ((token.get_type() != OPERATOR) || (strcmp(token.get_data(), "=") != 0))
    {
        printf("%s", "invalid for, expect '='.");
        exit(-1);
    }

    char *next_p = NULL;
    int initial_val = (int)Expression(&tokenizer).evaluate();
    printf("%s\n", name);
    VariableTable::GetInstance()->save(name, initial_val);
    printf("%d\n", initial_val);

    token = tokenizer.get_token();
    if ((strcmp(token.get_data(), "TO") != 0))
    {
        printf("%s", "invalid for, expect 'To'.");
        exit(-1);
    }

    int target_val = (int)Expression(&tokenizer).evaluate();
    // char *p = read_to_end_of_line(next_p);
    token = tokenizer.get_token();
    if (token.get_type() != LINE_BREAK) {
        printf("%s", "expect line break ");
        exit(-1);
    }
    if (initial_val < target_val)
    {
        printf("%s", "Pushed to stack.\n");
        // push to for stack
        // tokenizer.get_pointer()
        // push_to_for_stack(tokenizer.get_pointer(), name, initial_val, target_val);
        VariableTable::GetInstance()->save(name, initial_val);
        ForStack::GetInstance()->push(tokenizer, target_val, name);
    }
    printf("%d\n", target_val);

    return;
}

void Interpreter::exec_next()
{
    for_stack_element *element = ForStack::GetInstance()->top();
    int value = VariableTable::GetInstance()->get(element->count_variable);
    if (value+1 <= element->target) {
        tokenizer = element->tokenizer;
        VariableTable::GetInstance()->save(element->count_variable, value + 1);
    } else {
        ForStack::GetInstance()->pop();
    }
}

void Interpreter::exec_gosub() {
    int label = Expression(&tokenizer).evaluate();

    GoSubStack::GetInstance()->push(tokenizer);

    char *loc = LabelScanner::GetInstance(nullptr)->get_label_loc(label);
    tokenizer = Tokenizer(loc);

}

void Interpreter::exec_return() {
    tokenizer = GoSubStack::GetInstance()->top();
    GoSubStack::GetInstance()->pop();
}

void Interpreter::exec_goto() {
    int label = Expression(&tokenizer).evaluate();
    char *loc = LabelScanner::GetInstance(nullptr)->get_label_loc(label);
    tokenizer = Tokenizer(loc);
}

void Interpreter::exec_if() {
    int x = Expression(&tokenizer).evaluate();
    Token token = tokenizer.get_token();
    if (token.get_type() != OPERATOR) {
        printf("In exec_if, expect operator.");
        exit(-1);
    }
    char oper = *token.get_data();
    int y = Expression(&tokenizer).evaluate();
    token = tokenizer.get_token();
    if (strcmp(token.get_data(), "THEN") != 0) {
        printf("In exec_if, expect then.");
        exit(-1);
    }
    bool matched = false;
    switch(oper) {
        case '<': matched = (x<y);
        case '=': matched = (x=y);
        case '>': matched = (x>y);
    }
    if (!matched) {
        Token token = tokenizer.get_token();
        do
        {
            token = tokenizer.get_token();
        } while (token.get_type() != LINE_BREAK);
    }
}

void Interpreter::exec_keyword()
{
    char *data = token.get_data();

    if (strcmp(data, "PRINT") == 0)
    {
        printf("%s", "enter into print.");
        exec_print();
    }
    else if (strcmp(data, "FOR") == 0)
    {
        exec_for();
    }
    else if (strcmp(data, "NEXT") == 0)
    {
        exec_next();
    }
    else if (strcmp(data, "GOSUB") == 0)
    {
        exec_gosub();
    }
    else if (strcmp(data, "RETURN") == 0)
    {
        exec_return();
    }
    else if (strcmp(data, "IF") == 0) {
        exec_if();
    }
    else if (strcmp(data, "GOTO") == 0) {
        exec_goto();
    }
    else if (strcmp(data, "END") == 0)
    {
        exit(1);
    }
}


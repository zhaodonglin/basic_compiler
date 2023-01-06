#include "Expression.h"
#include <math.h>
#include "VariableTable.h"

const int MAX_WEIGHT = 256;

Expression::Expression(Tokenizer *tokenizer) : tokenizer(tokenizer)
{
    token = tokenizer->get_token();
}

int Expression::evaluate()
{
    parse();
    TreeNode *root = create_tree(parsed_tokens);
    return calculator(root);
}

void Expression::parse() {
    int weight = 0;

    while (true)
    {
        TOKEN_TYPE token_type = token.get_type();
        if (token_type == NUMBER)
        {
            parsed_tokens.push_back(parsed_token(weight, token));
        }
        else if (token_type == IDENTIFIER)
        {
            parsed_tokens.push_back(parsed_token(weight, token));
        }
        else if (token_type == OPERATOR)
        {
            switch (*token.get_data())
            {
            case '(':
                weight += 4;
                break;
            case ')':
                weight -= 4;
                break;
            case '+':
                weight += 1;
                parsed_tokens.push_back(parsed_token(weight, token));
                break;
            case '-':
                weight += 1;
                parsed_tokens.push_back(parsed_token(weight, token));
                break;
            case '*':
                weight += 2;
                parsed_tokens.push_back(parsed_token(weight, token));
                break;
            case '/':
                weight += 2;
                parsed_tokens.push_back(parsed_token(weight, token));
                break;
            }
        }
        else
        {
            break;
        }
        token = tokenizer->get_token();
    }
}

int Expression::find_lowest_token(vector<parsed_token> parsed_tokens) {
    int cur_lowest = MAX_WEIGHT;
    int cur_index = -1;
    std::vector<parsed_token>::size_type sz = parsed_tokens.size();
    for (unsigned i=0; i<sz; i++) {
        if (parsed_tokens[i].weight > 0 && parsed_tokens[i].weight < cur_lowest) {
            cur_lowest = parsed_tokens[i].weight;
            cur_index = i;
        }
    }
    return cur_index;
}

TreeNode* Expression::create_tree(vector<parsed_token> tokens)
{
    if (tokens.size() == 0)
    {
        return NULL;
    }else if (tokens.size()==1) {
        return new TreeNode(tokens[0].token, nullptr, nullptr);
    } else {
        int lowest = find_lowest_token(tokens);
        std::vector<parsed_token> left(v.begin(), v.begin()+lowest-1);
        std::vector<parsed_token> right(v.begin()+lowest+1, v.ned());

        return new TreeNode(tokens[lowest].token,
        create_tree(left),
        create_tree(right)
        );
    }
}

int calculator(struct TreeNode *root)
{
    int result = 0;
    TOKEN_TYPE token_type = root->token.get_type();

    if (token_type == NUMBER)
    {
        result = atoi(root->token.get_data());
    }
    else if (token_type == IDENTIFIER)
    {
        result = VariableTable::GetInstance()->get(root->token.get_data());
    }
    else
    {
        switch (*(char *)(root->token.get_data()))
        {
        case '+':
            result = calculator(root->left) + calculator(root->right);
            break;
        case '-':
            result = calculator(root->left) - calculator(root->right);
            break;
        case '*':
            result = calculator(root->left) * calculator(root->right);
            break;
        case '/':
            result = calculator(root->left) / calculator(root->right);
            break;
        }
    }
    return result;
}

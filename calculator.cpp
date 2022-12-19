#include <iostream>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

#define DATA_SIZE 256
#define TOKEN_NUMBER 256
#define MAX_WEIGHT 16

typedef enum token_type
{
    NUMBER,
    OPERATOR,
} TOKEN_TYPE;

typedef struct parsed_expression
{
    int index;
    int weight;
    char data[DATA_SIZE];
    TOKEN_TYPE type;
} PARSED_EXPRESSION;

typedef struct token
{
    TOKEN_TYPE type;
    char *p;
    char data[DATA_SIZE];
} TOKEN;

struct TreeNode
{
    struct TreeNode *left;
    struct TreeNode *right;
    char data[DATA_SIZE];
    TOKEN_TYPE type;
};

bool isoperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')';
}

TOKEN
get_token(char *p)
{
    TOKEN token;
    char *pointer = p;
    if (isdigit(*pointer))
    {
        while (isdigit(*pointer))
        {
            pointer += 1;
        }
        token.type = NUMBER;
        token.p = pointer;
        strncpy(token.data, p, pointer - p);
    }
    else if (isoperator(*pointer))
    {
        token.type = OPERATOR;
        token.p = pointer + 1;
        strncpy(token.data, p, 1);
    }
    return token;
}

void set_parsed_exp(PARSED_EXPRESSION *exp, int index, int weight, TOKEN *token)
{
    exp[index].index = index;
    exp[index].weight = weight;
    strncpy(exp[index].data, token->data, DATA_SIZE);
    exp[index].type = token->type;
}

int parse(char *expression, PARSED_EXPRESSION *parsed_exp)
{
    char *p = expression;
    int weight = 0;
    int index = 0;
    while (*p != '\0')
    {
        TOKEN token = get_token(p);
        cout << token.type << endl;

        if (token.type == NUMBER)
        {
            set_parsed_exp(parsed_exp, index, 0, &token);
            index++;
            // p = token.p + 1;
        }
        else if (token.type == OPERATOR)
        {
            switch (*token.data)
            {
            case '(':
                weight += 4;
                // index++;
                // p = token.p + 1;
                break;
            case ')':
                weight -= 4;
                // index++;
                // p = token.p + 1;
                break;
            case '+':
                weight += 1;
                set_parsed_exp(parsed_exp, index, weight, &token);
                index++;
                // p = token.p + 1;
                break;
            case '-':
                weight += 1;
                set_parsed_exp(parsed_exp, index, weight, &token);
                index++;
                // p = token.p + 1;
                break;
            case '*':
                weight += 2;
                set_parsed_exp(parsed_exp, index, weight, &token);
                index++;
                // p = token.p + 1;
                break;
            case '/':
                weight += 2;
                set_parsed_exp(parsed_exp, index, weight, &token);
                index++;
                // p = token.p + 1;
                break;
            }
        }
        p = token.p;
    }
    return index;
}

int find_lowest_weight(PARSED_EXPRESSION *exp, int begin, int end)
{
    int cur_lowest = MAX_WEIGHT;
    int cur_index = -1;
    for (int i = begin; i <= end; i++)
    {
        if (exp[i].weight > 0 && exp[i].weight < cur_lowest)
        {
            cur_lowest = exp[i].weight;
            cur_index = i;
        }
    }
    return cur_index;
}

struct TreeNode *
create_tree(PARSED_EXPRESSION *exp, int begin, int end)
{
    cout << "begin: " << begin << endl;
    cout << "end: " << end << endl;
    if (begin > end)
    {
        return NULL;
    }
    struct TreeNode *node =
        (struct TreeNode *)malloc(sizeof(struct TreeNode));

    if (begin == end)
    {
        strncpy(node->data, exp[begin].data, DATA_SIZE);
        node->left = NULL;
        node->right = NULL;
        node->type = exp[begin].type;
        return node;
    }

    int lowest = find_lowest_weight(exp, begin, end);
    cout << "lowest: " << lowest << endl;
    // cout << "end: " << end << endl;
    strncpy(node->data, exp[lowest].data, DATA_SIZE);
    node->left = create_tree(exp, begin, lowest - 1);
    node->right = create_tree(exp, lowest + 1, end);
    node->type = exp[lowest].type;
    return node;
}

void traverse_tree(struct TreeNode *root, int tab_num)
{
    if (root == NULL)
    {
        return;
    }
    for (int i = 0; i < tab_num; i++)
    {
        cout << "\t";
    }
    cout << "Node: ";
    cout << "data: " << root->data << " "
         << "type: " << root->type << endl;

    if (root->left != NULL)
    {
        for (int i = 0; i < tab_num + 1; i++)
        {
            cout << "\t";
        }
        cout << "left:" << endl;
        traverse_tree(root->left, tab_num + 1);
    }
    if (root->right != NULL)
    {
        for (int i = 0; i < tab_num + 1; i++)
        {
            cout << "\t";
        }
        cout << "right:" << endl;
        traverse_tree(root->right, tab_num + 1);
    }
}

float calculator(struct TreeNode *root)
{
    float result = 0;

    if (root->type == NUMBER)
    {
        cout << "original data: " << root->data << endl;
        result = atof(root->data);
        cout << "data: " << result << endl;
    }
    else
    {
        switch (*(char *)(root->data))
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

int main()
{
    PARSED_EXPRESSION parsed_expression[TOKEN_NUMBER];
    char tested_expression[] = "(2+3)*4";
    int length = parse(tested_expression, parsed_expression);
    cout << length << endl;
    for (int i = 0; i < length; i++)
    {
        cout << " index: " << parsed_expression[i].index;
        cout << " weight: " << parsed_expression[i].weight;
        cout << " data: " << parsed_expression[i].data;
        cout << " type: " << parsed_expression[i].type << endl;
    }
    struct TreeNode *root = create_tree(parsed_expression, 0, length - 1);
    traverse_tree(root, 0);
    float res = calculator(root);
    // int res = 0;
    cout << "result is: " << res << endl;
    return 0;
}

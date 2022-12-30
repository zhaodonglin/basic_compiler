#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace std;

#define PROG_SIZE (1024 * 1024 * 16)
#define TOKEN_SIZE (1024)
float eval_expression(char *p, char **next_p);
char PROG_BUFFER[PROG_SIZE];

typedef enum token_type
{
    OPERATOR,
    DELIMETER,
    VARIANT,
    QUOTE,
    NUMBER,
    KEYWORD,
    STRING,
    LINE_BREAK,
    END,
    EOP,
} TOKEN_TYPE;

typedef struct token
{
    char data[TOKEN_SIZE];
    char *next;
    TOKEN_TYPE type;
} TOKEN;

const char *KEYWORDS[] = {"PRINT", "FOR", "NEXT", "END", "TO"};

bool is_white(char p)
{
    return '\0' != p && NULL != strchr(" \t", p);
}

bool is_line_break(char p)
{
    return '\0' != p && NULL != strchr("\r\n", p);
}

bool is_operator(char p)
{
    return '\0' != p && NULL != strchr("+-=*/%()", p);
}

bool is_end(char p)
{
    return '\0' == p;
}

bool is_keyword(char *p)
{
    for (int i = 0; i < sizeof(KEYWORDS) / sizeof(char *); i++)
    {
        if (strcmp(KEYWORDS[i], p) == 0)
        {
            return true;
        }
    }
    return false;
}

TOKEN get_token(char *p)
{
    TOKEN token;
    char *next = p;
    // skip whitespace and tab
    while (is_white(*next))
    {
        next++;
    }
    if (is_end(*next))
    {
        token.type = EOP;
        return token;
    }
    char *start = next;
    if (isdigit(*next))
    {
        while (isdigit(*next))
        {
            next++;
        }
        memset(token.data, 0, TOKEN_SIZE);
        strncpy(token.data, start, next - start);

        token.next = next;
        token.type = NUMBER;
    }
    else if (is_line_break(*next))
    {
        token.next = next + 1;
        token.type = LINE_BREAK;
    }
    else if (isalpha(*next))
    {
        while (isalpha(*next))
        {
            next++;
        }
        memset(token.data, '\0', TOKEN_SIZE);
        strncpy(token.data, start, next - start);
        token.next = next;
        token.type = STRING;
        if (is_keyword(token.data))
        {
            token.type = KEYWORD;
        }
        else
        {
            token.type = VARIANT;
        }
    }
    else if (is_operator(*next))
    {
        next++;
        memset(token.data, '\0', TOKEN_SIZE);
        strncpy(token.data, start, next - start);

        token.next = next;
        token.type = OPERATOR;
    }

    return token;
}

typedef struct label
{
    int label;
    char *p;
} LABEL;

#define MAX_LABEL_NUMBER 256
LABEL labels[MAX_LABEL_NUMBER];
int cursor = 0;

void add_to_label(TOKEN *token)
{
    // static int cursor;
    int label = atoi(token->data);
    char *p = token->next;

    labels[cursor].p = p;
    labels[cursor].label = label;
    cursor++;
}

void show_label()
{
    for (int i = 0; i < cursor; i++)
    {
        cout << labels[i].label << labels[i].p << endl;
    }
}

void scan_labels(char *p)
{
    // scan from start to end
    // the first of line
    // the previous token is line_break and current token is digit,
    // then this is a label
    TOKEN token = get_token(p);
    if (token.type == EOP)
    {
        return;
    }
    if (token.type == NUMBER)
    {
        add_to_label(&token);
        p = token.next;
    }
    while (*p != '\0')
    {
        token = get_token(p);
        p = token.next;
        if (token.type == EOP)
        {
            return;
        }
        if (token.type == LINE_BREAK)
        {
            TOKEN next_token = get_token(p);
            if (next_token.type == EOP)
            {
                return;
            }
            if (next_token.type == NUMBER)
            {
                add_to_label(&next_token);
            }
            p = next_token.next;
        }
    }
}

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
    *(p - 1) = '\0';
    return 0;
}

#define MAX_VARIABLE_NUMBER 256
typedef struct variable
{
    char name[TOKEN_SIZE];
    int value;
} VARIABLE;

VARIABLE variable_table[MAX_VARIABLE_NUMBER];
int variable_cursor = 0;
void save_to_variable_table(char *name, int value)
{
    // static int cursor;
    VARIABLE *variable = &variable_table[variable_cursor];
    strncpy(variable->name, name, TOKEN_SIZE);
    variable->value = value;
    variable_cursor++;
}

int get_variable_value(char *name)
{
    for (int i = 0; i < variable_cursor; i++)
    {
        if (strcmp(variable_table[i].name, name) == 0)
        {
            return variable_table[i].value;
        }
    }
    printf("%s", "Failed to find value.\n");
    exit(-1);
}

void update_variable_value(char *name, int value)
{
    for (int i = 0; i < variable_cursor; i++)
    {
        if (strcmp(variable_table[i].name, name) == 0)
        {
            variable_table[i].value = value;
            return;
        }
    }
    printf("%s\n", name);
    printf("%s", "Failed to find value for update.\n");
    exit(-1);
}

// int eval_expression(char *p)
// {
//     return atoi(p);
// }

char *assignment(TOKEN *token)
{
    char name[TOKEN_SIZE];
    strncpy(name, token->data, TOKEN_SIZE);

    TOKEN next_token = get_token(token->next);
    if ((next_token.type != OPERATOR) || (strcmp(next_token.data, "=") != 0))
    {
        printf("%s", "Invalid assignment, expect '=' ");
        exit(-1);
    }
    char *next_p = NULL;
    int value = eval_expression(next_token.next, &next_p);
    save_to_variable_table(name, value);
    return next_token.next;
}

typedef struct for_stack
{
    char *loc;
    char count_variable[TOKEN_SIZE];
    int target;
} FOR_STACK;

#define STACK_SIZE 256
FOR_STACK for_stack[STACK_SIZE];
int stack_top = 0;
void push_to_for_stack(char *loc, char *count_variable, int initial_value, int target)
{
    for_stack[stack_top].loc = loc;
    strncpy(for_stack[stack_top].count_variable, count_variable, TOKEN_SIZE);
    for_stack[stack_top].target = target;
    stack_top++;

    save_to_variable_table(count_variable, initial_value);
}

FOR_STACK *pop_for_stack()
{
    char *loc = NULL;
    stack_top -= 1;
    loc = for_stack[stack_top].loc;
    char *variable_name = for_stack[stack_top].count_variable;

    int value = get_variable_value(variable_name);
    // save_to_variable_table(variable_name, value + 1);
    update_variable_value(variable_name, value + 1);
    return &for_stack[stack_top];
}

char *read_to_end_of_line(char *p)
{
    while ('\n' != *p && '\0' != *p)
    {
        p++;
    }

    return p;
}

char *exec_next(TOKEN *token)
{
    FOR_STACK *for_stack = pop_for_stack();
    int value = get_variable_value(for_stack->count_variable);
    if (value > for_stack->target)
    {
        char *p = read_to_end_of_line(token->next);
        return p + 1;
    }
    else
    {
        stack_top++;
        return for_stack->loc;
    }
}

char *exec_for(TOKEN *for_token)
{
    TOKEN token = get_token(for_token->next);
    char name[TOKEN_SIZE];
    if (token.type != VARIANT)
    {
        printf("%s", "invalid for, expect variable.");
        // return token.next;
        exit(-1);
    }
    // variable name is token.data
    strncpy(name, token.data, TOKEN_SIZE);

    token = get_token(token.next);
    if ((token.type != OPERATOR) || (strcmp(token.data, "=") != 0))
    {
        printf("%s", "invalid for, expect '='.");
        // return token.next;
        exit(-1);
    }

    // // variable initial value, should be eval_expression
    // token = get_token(token.next);
    // // int initial_val = eval_expression(token.next);
    // if (token.type != NUMBER)
    // {
    //     printf("%s", "initival value expect a NUMBER.");
    //     exit(-1);
    // }
    // // set variable

    // int initial_val = atoi(token.data);
    char *next_p = NULL;
    int initial_val = (int)eval_expression(token.next, &next_p);
    printf("%s\n", name);
    save_to_variable_table(name, initial_val);
    printf("%d\n", initial_val);
    token = get_token(next_p);
    if ((strcmp(token.data, "TO") != 0))
    {
        printf("%s", "invalid for, expect 'To'.");
        // return token.next;
        exit(-1);
    }
    // int end_val = eval_expression(token.next);
    // variable target value, should be eval_expression
    // token = get_token(token.next);
    // // int initial_val = eval_expression(token.next);
    // if (token.type != NUMBER)
    // {
    //     printf("%s", "initival value expect a NUMBER.");
    //     exit(-1);
    // }
    // int target_val = atoi(token.data);
    int target_val = (int)eval_expression(token.next, &next_p);
    char *p = read_to_end_of_line(next_p);
    if (initial_val < target_val)
    {
        printf("%s", "Pushed to stack.\n");
        push_to_for_stack(p + 1, name, initial_val, target_val);
    }
    printf("%d\n", target_val);

    return p + 1;
}

char *roll_back(TOKEN *token)
{
    char *p = token->next;
    char *cursor = token->data;

    while ('\0' != *cursor)
    {
        p--;
        cursor++;
    }
    return p;
}

char *exec_print(TOKEN *print_token)
{
    TOKEN token = *print_token;
    char *next_p = token.next;

    do
    {
        token = get_token(next_p);

        if (token.type == QUOTE)
        {
            printf("%s", token.data);
            next_p = token.next;
        }
        else if (token.type == DELIMETER)
        {
            if (strcmp(token.data, ",") == 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    printf(" ");
                }
            }
            else if (strcmp(token.data, ";") == 0)
            {
                printf(" ");
            }
            next_p = token.next;
        }
        else if (token.type == LINE_BREAK)
        {
            break;
        }
        else
        {
            char *p = roll_back(&token);

            float value = eval_expression(p, &next_p);
            // int value = get_variable_value(token.data);
            printf("%s", "-------------------");
            printf("\n%f\n", value);
            // token = get_token(next_p);
        }

    } while (token.type != END && token.type != LINE_BREAK);

    return token.next;
}

char *exec_keyword(TOKEN *token)
{
    char *p;
    if (strcmp(token->data, "PRINT") == 0)
    {
        printf("%s", "enter into print.");
        p = exec_print(token);
    }
    else if (strcmp(token->data, "FOR") == 0)
    {
        p = exec_for(token);
    }
    else if (strcmp(token->data, "NEXT") == 0)
    {
        p = exec_next(token);
    }
    else if (strcmp(token->data, "END") == 0)
    {
        exit(1);
    }

    return p;
}

void interprete(char *prog)
{
    // interprete tokens one by one
    TOKEN token;
    char *p = prog;
    do
    {
        token = get_token(p);
        // the first token should be NUMBER or KEYWORD or VARIABLE
        // NUMBER is the label, label is processed in the first traverse.
        if (token.type == VARIANT)
        {
            p = assignment(&token);
        }
        else if (token.type == KEYWORD)
        {
            p = exec_keyword(&token);
        }
        else if (token.type == NUMBER)
        {
            p = token.next;
        }
        printf("%s", p);
    } while (token.type != END);
}

#define DATA_SIZE 256
#define TOKEN_NUMBER 256
#define MAX_WEIGHT 16

typedef struct parsed_expression
{
    int index;
    int weight;
    char data[DATA_SIZE];
    TOKEN_TYPE type;
} PARSED_EXPRESSION;

struct TreeNode
{
    struct TreeNode *left;
    struct TreeNode *right;
    char data[DATA_SIZE];
    TOKEN_TYPE type;
};

void set_parsed_exp(PARSED_EXPRESSION *exp, int index, int weight, TOKEN *token)
{
    exp[index].index = index;
    exp[index].weight = weight;

    strncpy(exp[index].data, token->data, DATA_SIZE);

    exp[index].type = token->type;
}

int parse(char *expression, PARSED_EXPRESSION *parsed_exp, char **next_p)
{
    char *p = expression;
    int weight = 0;
    int index = 0;
    char *cur_end = p;
    while (true)
    {
        TOKEN token = get_token(p);
        cout << token.type << endl;

        if (token.type == NUMBER)
        {
            set_parsed_exp(parsed_exp, index, 0, &token);
            index++;
            // p = token.p + 1;
        }
        else if (token.type == VARIANT)
        {
            // get variable value
            // index++;
            set_parsed_exp(parsed_exp, index, 0, &token);
            index++;
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
        else
        {
            *next_p = cur_end;
            break;
        }
        p = token.next;
        cur_end = p;
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
    else if (root->type == VARIANT)
    {
        cout << "original data: " << root->data << endl;
        result = get_variable_value(root->data);
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

PARSED_EXPRESSION parsed_expression[TOKEN_NUMBER];

float eval_expression(char *p, char **next_p)
{
    int length = parse(p, parsed_expression, next_p);
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
    return res;
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
    scan_labels(prog);
    show_label();
    interprete(prog);
}

#include <iostream>
using namespace std;

#include "Tokenizer.h"

const char *KEYWORDS[] = {"PRINT", "FOR", "NEXT", "END", "TO"};

bool Token::iskeyword(char *p)
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

void Token::show()
{
    cout << "data:" << data << endl;
    cout << "type:" << type << endl;
}

Token::Token()
{
    memset(data, 0, TOKEN_SIZE);
    type = UNKNOWN;
}

Token::Token(TOKEN_TYPE src_type, char *src, int size)
{
    memset(data, 0, TOKEN_SIZE);
    type = src_type;
    if (src != NULL)
    {
        strncpy(data, src, size);
    }

    if (type == STRING)
    {
        if (iskeyword(data))
        {
            type = KEYWORD;
        }
        else
        {
            type = IDENTIFIER;
        }
    }
}

char *Token::get_data()
{
    return data;
}

TOKEN_TYPE Token::get_type()
{
    return type;
}

bool Tokenizer::iswhite(char p)
{
    return '\0' != p && NULL != strchr(" \t", p);
}

bool Tokenizer::islinebreak(char p)
{
    return '\0' != p && NULL != strchr("\r\n", p);
}

bool Tokenizer::isoperator(char p)
{
    return '\0' != p && NULL != strchr("+-=*/%()", p);
}

bool Tokenizer::isend(char p)
{
    return '\0' == p;
}

Token Tokenizer::get_token()
{
    Token token = Token();
    char *next = cur_pointer;

    while (iswhite(*next))
    {
        next++;
    }
    if (isend(*next))
    {
        return Token(EOP, NULL, 0);
    }
    char *start = next;
    if (isdigit(*next))
    {
        while (isdigit(*next))
        {
            next++;
        }
        cur_pointer = next;
        return Token(NUMBER, start, next - start);
    }
    else if (islinebreak(*next))
    {
        cur_pointer = next + 1;
        return Token(LINE_BREAK, NULL, 0);
    }
    else if (isalpha(*next))
    {
        while (isalpha(*next))
        {
            next++;
        }

        cur_pointer = next;
        return Token(STRING, start, next - start);
    }
    else if (isoperator(*next))
    {
        cur_pointer = next + 1;
        return Token(OPERATOR, start, next - start);
    }

    return token;
}

char *Tokenizer::next()
{
    return cur_pointer;
}

void Tokenizer::rollback(Token *token) {
    char *data = token->get_data();
    while ('\0' != *data) {
        data++;
        cur_pointer--;
    }
}



#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <cstring>
#include <cstddef>
#include <string.h>
#include <ctype.h>

const int TOKEN_SIZE = 1024;

typedef enum token_type
{
    OPERATOR,
    DELIMETER,
    IDENTIFIER,
    QUOTE,
    NUMBER,
    KEYWORD,
    STRING,
    LINE_BREAK,
    END,
    EOP,
    UNKNOWN,
} TOKEN_TYPE;

class Token
{
private:
    char data[TOKEN_SIZE];
    TOKEN_TYPE type;

public:
    Token();
    bool iskeyword(char *p);
    Token(TOKEN_TYPE src_type, char *src, int size);
    void show();
    char *get_data();
    TOKEN_TYPE get_type();
};

class Tokenizer
{
private:
    char *cur_pointer;
    bool iswhite(char p);
    bool islinebreak(char p);
    bool isoperator(char p);
    bool isend(char p);

public:
    Tokenizer(){
        cur_pointer =nullptr;
    }
    Tokenizer(char *p)
    {
        cur_pointer = p;
    }
    Tokenizer(const Tokenizer& tokenizer)
    {
        cur_pointer=tokenizer.cur_pointer;
    }
    Token get_token();
    char *next();
    void rollback(Token *token);

};
#endif

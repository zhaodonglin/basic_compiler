#include <iostream>
#include "LabelScanner.h"

LabelScanner::LabelScanner(char *p) : tokenizer(p)
{
    // tokenizer = Tokenizer(p);
    token = tokenizer.get_token();
}

void LabelScanner::add_to_label()
{
    int label = atoi(token.get_data());
    char *p = tokenizer.next();
    table[label] = p;
    return;
}

void LabelScanner::show_label()
{
    for (std::map<int, char *>::iterator it = table.begin(); it != table.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
}

void LabelScanner::scan_labels()
{
    if (token.get_type() == EOP)
    {
        return;
    }
    if (token.get_type() == NUMBER)
    {
        add_to_label();
    }

    while (token.get_type() != EOP)
    {
        token = tokenizer.get_token();
        if (token.get_type() == EOP)
        {
            return;
        }
        if (token.get_type() == LINE_BREAK)
        {
            token = tokenizer.get_token();
            if (token.get_type() == NUMBER)
            {
                add_to_label();
            }
        }
    }
}

int main(int argc, char *argv[])
{
    char buff[100] = "100 FOR";
    LabelScanner label_scanner = LabelScanner(buff);
    label_scanner.scan_labels();
    label_scanner.show_label();
    // Tokenizer tokenizer = Tokenizer(buff);
    // Token token = tokenizer.get_token();
    // printf("%s", token.data);
    // token.show();
}
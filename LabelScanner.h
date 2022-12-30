#ifndef LABEL_SCANNER_H
#define LABEL_SCANNER_H

#include <map>
#include "Tokenizer.h"

using namespace std;

class LabelScanner
{
public:
    LabelScanner(char *p);
    void add_to_label();
    void show_label();
    void scan_labels();

private:
    std::map<int, char *> table;
    Tokenizer tokenizer;
    Token token;
};
#endif
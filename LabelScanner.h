#ifndef LABEL_SCANNER_H
#define LABEL_SCANNER_H

#include <map>
#include "Tokenizer.h"

using namespace std;

class LabelScanner
{
public:
    void add_to_label();
    void show_label();
    void scan_labels();
    char* get_label_loc(int label);
    static LabelScanner *GetInstance(char *p);

private:
    LabelScanner(char *p);
    std::map<int, char *> table;
    Tokenizer tokenizer;
    Token token;
    static LabelScanner *label_scanner;
};
#endif
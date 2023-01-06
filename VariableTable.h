#ifndef VARIABLE_TABLE_H
#define VARIABLE_TABLE_H

#include <map>
#include <string>
using namespace std;

class VariableTable
{
private:
    map<string, float> table;
    static VariableTable *variable_table;

    VariableTable()=default;

public:
    void save(char *name, float value);
    float get(char *name);
    static VariableTable *GetInstance();
};
#endif
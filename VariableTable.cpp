#include "VariableTable.h"

void VariableTable::save(char *name, float value)
{
    string s = name;
    table[s] = value;
}

float VariableTable::get(char *name)
{
    string s = name;
    return table[s];
}

VariableTable *VariableTable::GetInstance()
{
    if (variable_table == nullptr)
    {
        variable_table = new VariableTable();
    }
    return variable_table;
}
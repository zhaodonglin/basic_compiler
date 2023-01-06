#include <map>
#include <string>
using namespace std;

class VariableTable
{
private:
    map<string, float> table;
    static VariableTable *variable_table;

    VariableTable();

public:
    void save(char *name, float value);
    float get(char *name);
    static VariableTable *GetInstance();
};
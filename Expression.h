#include "Tokenizer.h"
#include <vector>
using namespace std;

struct parsed_token
{
    int weight;
    Token token;

    parsed_token(int weight, Token token)
    {
        this->weight = weight;
        this->token = token;
    }
};

struct TreeNode
{
    struct TreeNode *left;
    struct TreeNode *right;
    Token token;
    TreeNode(Token token, TreeNode* left, TreeNode* right) {
        this->token = token;
        this->left = left;
        this->right = right;
    }
};

class Expression
{
public:
    Expression(Tokenizer *tokenizer);
    int evaluate();

private:
    Tokenizer *tokenizer;
    Token token;
    vector<parsed_token> parsed_tokens;
    void parse();

    int find_lowest_token(vector<parsed_token> parsed_tokens);
    TreeNode* Expression::create_tree(vector<parsed_token> tokens);
};

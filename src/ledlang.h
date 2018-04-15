#pragma once
#include <vector>

struct Editor;

struct AST
{
    
};

struct LedLang
{
    // tokens in -> AST out
    AST ParseTokens(std::vector<std::string> tokens)
    {
        (void) tokens;
        AST topNode;
        return topNode;
    }

    void RunCommand(AST ast, Editor* ed)
    {
        (void) ast;
        (void) ed;
    }
};

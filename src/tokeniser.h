#pragma once
#include <algorithm>

enum TokenType
{
    Identifier,
    Keyword,
    Literal,
    Operator,
    Punctuator,
    Comment,
    Other
};

struct Token
{
    std::string text = "";
    TokenType type = Other;
};

const std::vector<std::string> operators =
{
    "::", "++", "--", "(", ")", "[", "]", ".", "->",
    "~", "!", "+", "-", "&", "*", "new", "delete", "sizeof",
    "*", "%", "/", ">>", "<<", ">", "<", "<=", ">=", "==", "=",
    "^", "|", "||", "&&", "=", "*=", "/=", "%=", "+=", "-=", ">>=",
    "<<=", "&=", "^=", "|=", "?", ":", ","
};

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

bool IsOperator(std::string token)
{
    return (std::find(operators.begin(), operators.end(), rtrim(token)) != operators.end());
}

Token TextToToken(std::string text)
{
    Token t;
    t.text = text;

    if(IsOperator(text))
    {
        t.type = Comment;
    }
    else
    {
        t.type = Other;
    }
            
    return t;
}

// returns the number of chars of an operator we read
// 0 -> not an operator
int TryReadOperator(std::string text)
{
    const int lengthOfLongestOperator = 6;
    
    for(int i = lengthOfLongestOperator; i > 0; i--)
    {
        if(IsOperator(text.substr(0, i)))
        {
            return i;
        }
    }
    return 0;
}

const std::vector<std::string> keywords =
{
"__abstract", "__alignof", "__asm", "__assume", "__based", "__box", "__cdecl",
"__declspec", "__delegate", "__event", "__except", "__fastcall", "__finally", "__forceinline",
"__gc", "__hook", "__identifier", "__if_exists", "__if_not_exists", "__inline", "__int16",
"__int32", "__int64", "__int8", "__interface", "__leave", "__m128", "__m128d", "__m128i",
"__m64", "__multiple_inheritance", "__nogc", "__noop", "__pin", "__property", "__raise",
"__sealed", "__single_inheritance", "__stdcall", "__super", "__thiscall", "__try", "__except,",
"__finally", "__try_cast", "__unaligned", "__unhook", "__uuidof", "__value", "__virtual_inheritance",
"__w64", "__wchar_t", "wchar_t", "abstract", "array", "auto", "bool", "break", "case",
"catch", "char", "class", "const", "const_cast", "continue", "decltype", "default", "delegate",
"delete", "deprecated", "dllexport", "dllimport", "do", "double", "dynamic_cast", "else",
"enum", "class", "struct", "event", "explicit", "extern", "false", "finally", "float",
"for", "each", "in", "friend", "friend_as", "gcnew", "generic", "goto", "if", "initonly",
"inline", "int", "interface", "class", "interface", "struct", "interior_ptr", "literal", "long",
"mutable", "naked", "namespace", "new", "new", "noinline", "noreturn", "nothrow", "novtable",
"nullptr", "operator", "private", "property", "property", "protected", "public", "ref",
"class", "ref", "struct", "register", "reinterpret_cast", "return", "safecast", "sealed",
"selectany", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct",
"switch", "template", "this", "thread", "throw", "true", "try", "typedef", "typeid", "typeid",
"typename", "union", "unsigned", "using", "declaration", "using", "directive", "uuid", "value",
"class", "value", "struct", "virtual", "void", "volatile", "while"
};

bool IsKeyword(std::string token)
{
    return (std::find(keywords.begin(), keywords.end(), rtrim(token)) != keywords.end());
}

int TryReadKeyword(std::string text)
{
    const int lengthOfLongestKeyword = 21;
    
    for(unsigned int i = lengthOfLongestKeyword; i > 0; i--)
    {
        if(IsKeyword(text.substr(0, i)) && (i+1>text.length() || !std::isalpha(text[i])))
        {
            return i;
        }
    }
    return 0;
}


int TryReadNumericalLiteral(std::string text)
{
    int charsRead = 0;
    for(unsigned int i = 0; i<text.length(); i++)
    {
        if(!(std::isdigit(text[i]) || text[i] == '.'))
        {
            break;
        }
        else
        {
            charsRead++;
        }
    }

    return charsRead;
}

int TryReadStringLiteral(std::string text)
{
    int charsRead = 0;
    if(text[0] == '"' || text[0] == '\'')
    {
        charsRead++;
        for(unsigned int i = 1; i<text.length(); i++)
        {
            charsRead++;            
            if(text[i] == text[0])
            {
                break;
            }
            if(text[i] == '\\')
            {
                i++;
                charsRead++;
            }
        }
    }
    
    return charsRead;
}


int TryReadLiteral(std::string text)
{
    int charsRead;
    if((charsRead = TryReadNumericalLiteral(text)) > 0)
    {
        return charsRead;
    }

    if((charsRead = TryReadStringLiteral(text)) > 0)
    {
        return charsRead;
    }

    return 0;
}

int TryReadComment(std::string text)
{
    // TODO handle /* */ comments - multiline case is tricky
    int charsRead = 0;
    if(text.substr(0, 2) == "//")
    {
        // Assumption: we are tokenising line by line, not the whole program at once.
        charsRead = text.length();
    }
    return charsRead;
}

int TryReadIdentifier(std::string text)
{
    int charsRead = 0;
    for(unsigned int i = 0; i<text.length(); i++)
    {
        if(!std::isalpha(text[i]) || text[i] == '_')
        {
            break;
        }
        charsRead++;
    }
    return charsRead;
}

std::vector<Token> Tokenise(std::string text, std::string filename)
{
    // just do everything as c++ for now
    (void) filename;
    
    std::vector<Token> tokens;

    std::string currToken = "";
    for(unsigned int i = 0; i<text.length();)
    {
        Token t;
        if(std::isspace(text[i]))
        {
            currToken += text[i];           
            i++;
        }
        else
        {
            TokenType type = Other;
            int charsRead = 0;


            if((charsRead = TryReadComment(text.substr(i))) > 0)
            {
                type = Comment;
            }
            else if((charsRead = TryReadOperator(text.substr(i))) > 0)
            {
                type = Operator;
            }
            else if((charsRead = TryReadLiteral(text.substr(i))) > 0)
            {
                type = Literal;
            }
            else if((charsRead = TryReadKeyword(text.substr(i))) > 0)
            {
                type = Keyword;
            }
            else if((charsRead = TryReadIdentifier(text.substr(i))) > 0)
            {
                type = Identifier;
            }
            else
            {
                type = Other;
                charsRead = text.length() - i;
            }

            t.text = currToken + text.substr(i, charsRead);
            t.type = type;
            i += charsRead;
            currToken = "";
            tokens.push_back(t);
        }

        
    }

    return tokens;
}

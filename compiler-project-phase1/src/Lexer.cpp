#include "Lexer.h"

// classifying characters
namespace charinfo
{
    // ignore whitespaces
    LLVM_READNONE inline bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
               c == '\r' || c == '\n';
    }

    LLVM_READNONE inline bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool isLetter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    LLVM_READNONE inline bool secCharIsEqual(char c)
    {
        return (c == '=');
    }
}

void Lexer::next(Token &token)
{
    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
    {
        ++BufferPtr;
    }
    // make sure we didn't reach the end of input
    if (!*BufferPtr)
    {
        token.Kind = Token::eoi;
        return;
    }
    // collect characters and check for keywords or ident
    if (charinfo::isLetter(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isLetter(*end))
            ++end;
        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        Token::TokenKind kind;
        if (Name == "if")
            kind = Token::KW_if;
        else if (Name == "int")
            kind = Token::KW_int;
        else if (Name == "and")
            kind = Token::KW_and; // added
        else if (Name == "or")
            kind = Token::KW_or; // added
        else if (Name == "else")
            kind = Token::KW_else; // added
        else if (Name == "elif")
            kind = Token::elif; // added
        else if (Name == "begin")
            kind = Token::begin; // added
        else if (Name == "end")
            kind = Token::end; // added
        else if (Name == "loop")
            kind = Token::loop; // added
        else
            kind = Token::ident;
        // generate the token
        formToken(token, end, kind);
        return;
    }
    // check for numbers
    else if (charinfo::isDigit(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isDigit(*end))
            ++end;
        formToken(token, end, Token::number);
        return;
    }
    // check for double op
    else if (charinfo::secCharIsEqual(*(BufferPtr + 1)))
    {
        const char *end = BufferPtr + 2;

        switch (*BufferPtr)
        {

#define CASE(ch, tok)               \
    case ch:                        \
        formToken(token, end, tok); \
        break
            CASE('+', Token::plus_equal);   // added
            CASE('-', Token::minus_equal);  // added
            CASE('*', Token::mult_equal);   // added
            CASE('/', Token::div_equal);    // added
            CASE('%', Token::remain_equal); // added
            CASE('>', Token::more_equal);   // added
            CASE('<', Token::less_equal);   // added
            CASE('=', Token::equal_equal);  // added
            CASE('!', Token::not_equal);    // added

#undef CASE
        default:
            formToken(token, end, Token::unknown);
        }
        return;
    }
    else
    {

        switch (*BufferPtr)
        {

#define CASE(ch, tok)                         \
    case ch:                                  \
        formToken(token, BufferPtr + 1, tok); \
        break
            CASE('+', Token::plus);
            CASE('-', Token::minus);
            CASE('*', Token::star);
            CASE('/', Token::slash);
            CASE('%', Token::slash); // new 
            CASE('(', Token::l_paren);
            CASE(')', Token::r_paren);
            CASE(';', Token::semicolon);
            CASE(',', Token::comma);
            CASE('=', Token::equal);
            // CASE('+=', Token::plus_equal);   // added
            // CASE('-=', Token::minus_equal);  // added
            // CASE('*=', Token::mult_equal);   // added
            // CASE('/=', Token::div_equal);    // added
            // CASE('%=', Token::remain_equal); // added
            CASE('<', Token::less); // added
            CASE('>', Token::more); // added
            // CASE('>=', Token::more_equal);   // added
            // CASE('<=', Token::less_equal);   // added
            // CASE('==', Token::equal_equal);  // added
            // CASE('!=', Token::not_equal);    // added
            CASE(':', Token::colon); // added

#undef CASE
        default:
            formToken(token, BufferPtr + 1, Token::unknown);
        }
        return;
    }
}

void Lexer::formToken(Token &Tok, const char *TokEnd,
                      Token::TokenKind Kind)
{
    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
    BufferPtr = TokEnd;
}

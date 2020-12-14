#ifndef PARSER_H_
#define PARSER_H_
#include "parsingtable.h"
#include "rule.h"
typedef struct {
    Symbol *Symbols[32]; // 栈空间
    int nSymbolCount;    // 栈内元素数量
} ParsingStack;

void PushSymbol(ParsingStack *pStack, const Symbol *pSymbol);
void PushProduction(ParsingStack *pStack, const Production *pProduction);
const Symbol *PopSymbol(ParsingStack *pStack);
void Parse(const Rule *pRuleHead, const ParsingTable *pParsingTable,
           const char *string);
void PrintParsingStack(const ParsingStack *pStack);
#endif

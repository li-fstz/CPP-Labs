#ifndef PARSER_H_
#define PARSER_H_
#include "parsingtable.h"
#include "rule.h"
typedef struct {
    Symbol *Symbols[32];
    int nSymbolCount; // 数组元素个数
} ParsingStack;

void PushSymbol(ParsingStack *pStack, Symbol *pSymbol);
void PushProduction(ParsingStack *pStack, Production *pProduction);
Symbol *PopSymbol(ParsingStack *pStack);
void Parse(Rule *pRuleHead, ParsingTable *pParsingTable, const char *string);
void PrintParsingStack(ParsingStack *pStack);
#endif PARSER_H_

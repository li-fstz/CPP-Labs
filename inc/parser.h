#ifndef PARSER_H_
#define PARSER_H_

#include "parsingtable.h"
#include "rule.h"

typedef struct {
    const Symbol **symbols; // 栈空间
    int symbolCount;  // 栈内元素数量
} ParsingStack;

void PushSymbol(ParsingStack *stack, const Symbol *symbol);
void PushProduction(ParsingStack *stack, const Production *production);
const Symbol *PopSymbol(ParsingStack *stack);
void Parse(const Rule *ruleHead, const ParsingTable *parsingTable,
           const char *string);
void PrintParsingStack(const ParsingStack *stack);

#endif

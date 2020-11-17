#include "rule.h"
#include "parsingtable.h"
typedef struct _ParsingStack {
    RuleSymbol *Symbol[32];
    int nSymbolCount;  // 数组元素个数
} ParsingStack;

void PushSymbol(ParsingStack *pStack, RuleSymbol *pSymbol);
void PushSelect(ParsingStack *pStack, RuleSymbol *pSelect);
RuleSymbol *PopSymbol(ParsingStack *pStack);
void Parse (Rule *pRule, ParsingTable *pParsingTable, const char *string);
void PrintParsingStack(ParsingStack *pStack);

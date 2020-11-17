#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"
void PushSymbol(ParsingStack *pStack, RuleSymbol *pSymbol) {
    pStack->Symbol[pStack->nSymbolCount++] = pSymbol;
}
void PushSelect(ParsingStack *pStack, RuleSymbol *pSelect) {
    if (pSelect == NULL) {
        return;
    }
    PushSelect(pStack, pSelect->pNextSymbol);
    if (strcmp(pSelect->SymbolName, VoidSymbol)) {
        PushSymbol(pStack, pSelect);
    }
}
RuleSymbol *PopSymbol(ParsingStack *pStack) {
    if (pStack->nSymbolCount) {
        return pStack->Symbol[--pStack->nSymbolCount];
    } else {
        return NULL;
    }
}

void PrintParsingStack(ParsingStack *pStack) {
    for (int i = 0; i < pStack->nSymbolCount; i++) {
        printf(pStack->Symbol[i]->SymbolName);
    }
}

void Parse(Rule *pRule, ParsingTable *pParsingTable, const char *string) {
    ParsingStack Stack;
    Stack.nSymbolCount = 0;
    RuleSymbol *End = CreateSymbol(), *Start = CreateSymbol();
    End->isToken = 1;
    strcpy(End->SymbolName, EndSymbol);
    PushSymbol(&Stack, End);
    Start->isToken = 0;
    strcpy(Start->SymbolName, pRule->RuleName);
    Start->pRule = pRule;
    PushSymbol(&Stack, Start);
    int i = 0;
    while (Stack.nSymbolCount) {
        printf("%d\t", ++i);
        PrintParsingStack(&Stack);
        printf("\t%s\t", string);
        RuleSymbol *pTopSymbol = PopSymbol(&Stack);
        if (pTopSymbol->isToken) {
            if (Stack.nSymbolCount) {
                if (*pTopSymbol->SymbolName == *string) {
                    printf("“%c”匹配", *string);
                } else {
                    printf("“%c”不匹配", *string);
                    return;
                }
                string++;
            } else {
                if (strcmp(string, EndSymbol) == 0) {
                    printf("接受");
                } else {
                    printf("不接受");
                }
            }
        } else {
            char ch[] = {*string, '\0'};
            RuleSymbol **pSelect =
                FindSelect(pParsingTable, pTopSymbol->pRule, ch);
            printf("%s", pTopSymbol->SymbolName);
            PrintSelect(*pSelect);
            PushSelect(&Stack, *pSelect);
        }
        putchar('\n');
    }
}

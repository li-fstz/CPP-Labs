#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"
void PushSymbol(ParsingStack *pStack, Symbol *pSymbol) {
    pStack->Symbols[pStack->nSymbolCount++] = pSymbol;
}
void PushProduction(ParsingStack *pStack, Production *pProduction) {
    if (pProduction == NULL) {
        return;
    }
    PushProduction(pStack, pProduction->pNextSymbol);
    if (strcmp(pProduction->SymbolName, VOID_SYMBOL)) {
        PushSymbol(pStack, pProduction);
    }
}
Symbol *PopSymbol(ParsingStack *pStack) {
    if (pStack->nSymbolCount) {
        return pStack->Symbols[--pStack->nSymbolCount];
    } else {
        return NULL;
    }
}

void PrintParsingStack(ParsingStack *pStack) {
    for (int i = 0; i < pStack->nSymbolCount; i++) {
        printf(pStack->Symbols[i]->SymbolName);
    }
}

void Parse(Rule *pRule, ParsingTable *pParsingTable, const char *string) {
    ParsingStack Stack;
    Stack.nSymbolCount = 0;
    Symbol *End = CreateSymbol(), *Start = CreateSymbol();
    End->isToken = 1;
    strcpy(End->SymbolName, END_SYMBOL);
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
        Symbol *pTopSymbol = PopSymbol(&Stack);
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
                if (strcmp(string, END_SYMBOL) == 0) {
                    printf("接受");
                } else {
                    printf("不接受");
                }
            }
        } else {
            char ch[] = {*string, '\0'};
            Symbol **pProduction =
                FindProduction(pParsingTable, pTopSymbol->pRule, ch);
            printf("%s->", pTopSymbol->SymbolName);
            PrintProduction(*pProduction);
            PushProduction(&Stack, *pProduction);
        }
        putchar('\n');
    }
}

#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief 将符号压入栈中
 * 
 * @param pStack 栈指针
 * @param pSymbol 符号指针
 */
void PushSymbol(ParsingStack *pStack, Symbol *pSymbol) {
    pStack->Symbols[pStack->nSymbolCount++] = pSymbol;
}

/**
 * @brief 将产生式逆向压入栈中
 * 
 * @param pStack 栈指针
 * @param pProduction 产生式指针
 */
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

/**
 * @brief 输出栈
 * 
 * @param pStack 栈指针
 */
void PrintParsingStack(ParsingStack *pStack) {
    for (int i = 0; i < pStack->nSymbolCount; i++) {
        printf(pStack->Symbols[i]->SymbolName);
    }
}

/**
 * @brief 执行自顶向下语法分析
 * 
 * @param pRuleHead 文法链表的头指针
 * @param pParsingTable 预测分析表的指针
 * @param string 待解析文本
 */
void Parse(Rule *pRuleHead, ParsingTable *pParsingTable, const char *string) {
    ParsingStack Stack;
    Stack.nSymbolCount = 0;
    Symbol *End = CreateSymbol(), *Start = CreateSymbol();
    End->isToken = 1;
    strcpy(End->SymbolName, END_SYMBOL);
    PushSymbol(&Stack, End);
    Start->isToken = 0;
    strcpy(Start->SymbolName, pRuleHead->RuleName);
    Start->pRule = pRuleHead;
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
            Production **pProduction =
                FindProduction(pParsingTable, pTopSymbol->pRule, ch);
            printf("%s->", pTopSymbol->SymbolName);
            PrintProduction(*pProduction);
            PushProduction(&Stack, *pProduction);
        }
        putchar('\n');
    }
}

#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief 将符号压入栈中
 *
 * @param pStack 栈的指针
 * @param pSymbol 符号指针
 */
void PushSymbol(ParsingStack *pStack, const Symbol *pSymbol) {
    pStack->Symbols[pStack->nSymbolCount++] = pSymbol;
}

/**
 * @brief 将产生式逆向压入栈中，在压栈时忽略 ε
 *
 * @param pStack 栈的指针
 * @param pProduction 产生式指针
 */
void PushProduction(ParsingStack *pStack, const Production *pProduction) {
    if (pProduction == NULL) {
        return;
    }
    PushProduction(pStack, pProduction->pNextSymbol);
    if (strcmp(pProduction->SymbolName, VOID_SYMBOL)) {
        PushSymbol(pStack, pProduction);
    }
}

/**
 * @brief 将符号从栈中弹出
 *
 * @param pStack 栈的指针
 * @return Symbol* 符号的指针
 */
const Symbol *PopSymbol(ParsingStack *pStack) {
    if (pStack->nSymbolCount) {
        return pStack->Symbols[--pStack->nSymbolCount];
    } else {
        return NULL;
    }
}

/**
 * @brief 输出栈
 *
 * @param pStack 栈的指针
 */
void PrintParsingStack(const ParsingStack *pStack) {
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
void Parse(const Rule *pRuleHead, const ParsingTable *pParsingTable,
           const char *string) {
    /**
     * @brief 初始化分析栈
     */
    ParsingStack Stack;
    Stack.nSymbolCount = 0;

    /**
     * @brief 初始化开始符号和结束符号，
     * 并压入栈中
     */
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

        /**
         * @brief 从分析栈中弹出符号
         * 如果是终结符则进行匹配，
         * 如果是非终结符则从预测分析表中
         * 找到产生式逆向压入栈中。
         */
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
            Production *pProduction =
                *FindProduction(pParsingTable, pTopSymbol->pRule, ch);
            printf("%s->", pTopSymbol->SymbolName);
            PrintProduction(pProduction);
            PushProduction(&Stack, pProduction);
        }
        putchar('\n');
    }
}

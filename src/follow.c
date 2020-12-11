#include "follow.h"

#include <stdio.h>
#include <string.h>

#include "first.h"
#include "rule.h"

/**
 * @brief 从子集中删去 ε
 *
 * @param pSet 子集指针
 */
void RemoveVoidFromSet(Set *pSet) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminals[i], VOID_SYMBOL) == 0) {
            if (i != pSet->nTerminalCount - 1) {
                memmove(pSet->Terminals + i, pSet->Terminals + i + 1,
                        sizeof(pSet->Terminals[0]) *
                            (pSet->nTerminalCount - i - 1));
            }
            pSet->nTerminalCount--;
            return;
        }
    }
}

/**
 * @brief 生成文法的 Follow 集
 *
 * @param pRuleHead 文法的头指针
 * @param pVoidTable 空表的指针
 * @param pFollowSetList First 集的指针
 * @param pFirstSetList Follow 集的指针
 */
void GenFollowSet(const Rule *pRuleHead, VoidTable *pVoidTable,
                  SetList *pFollowSetList, SetList *pFirstSetList) {
    const Rule *pRule; // Rule 指针
    int isChange;      // 集合是否被修改的标志
    Symbol *pSymbol;   // Symbol 游标
    // 使用文法链表初始化 Follow 集合
    for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
        AddOneSet(pFollowSetList, pRule->RuleName);
    }
    // 在开始符号的 Follow 集合中加入结束符 #
    AddTerminalToSet(&pFollowSetList->Sets[0], END_SYMBOL);
    do {
        isChange = 0; // 设置集合修改标志
        for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
            for (Production *pProduction = pRule->pFirstProduction;
                 pProduction != NULL;
                 pProduction = pProduction->pNextProduction) {
                for (pSymbol = pProduction; pSymbol != NULL;
                     pSymbol = pSymbol->pNextSymbol) {
                    // 忽略终结符
                    if (pSymbol->isToken)
                        continue;
                    // 初始化一个 First 集合 TempFirstSet，作为临时变量
                    Set TempFirstSet;
                    TempFirstSet.nTerminalCount = 0;
                    Symbol *pTmpSymbol;
                    for (pTmpSymbol = pSymbol->pNextSymbol; pTmpSymbol != NULL;
                         pTmpSymbol = pTmpSymbol->pNextSymbol) {
                        if (pTmpSymbol->isToken) {
                            if (strcmp(pTmpSymbol, VOID_SYMBOL) != 0) {
                                AddTerminalToSet(&TempFirstSet,
                                                 pTmpSymbol->SymbolName);
                                break;
                            }
                        } else {
                            AddSetToSet(
                                &TempFirstSet,
                                GetSet(pFirstSetList, pTmpSymbol->SymbolName));
                            if (!*FindHasVoid(pVoidTable,
                                              pTmpSymbol->SymbolName)) {
                                break;
                            }
                        }
                    }
                    RemoveVoidFromSet(&TempFirstSet);
                    isChange =
                        AddSetToSet(GetSet(pFollowSetList, pSymbol->SymbolName),
                                    &TempFirstSet);
                    if (pTmpSymbol == NULL) {
                        isChange =
                            isChange ||
                            AddSetToSet(
                                GetSet(pFollowSetList, pSymbol->SymbolName),
                                GetSet(pFollowSetList, pRule->RuleName));
                    }
                }
            }
        }
    } while (isChange);
}

/**
 * @brief 输出 Follow 集
 *
 * @param pFollowSetList Follow 集指针
 */
void PrintFollowSet(SetList *pFollowSetList) {
    printf("\nThe Follow Set:\n");
    for (int i = 0; i < pFollowSetList->nSetCount; i++) {
        printf("Follow(%s) = { ", pFollowSetList->Sets[i].Name);
        for (int j = 0; j < pFollowSetList->Sets[i].nTerminalCount; j++) {
            if (j == pFollowSetList->Sets[i].nTerminalCount - 1) {
                printf("%s ", pFollowSetList->Sets[i].Terminals[j]);
            } else {
                printf("%s, ", pFollowSetList->Sets[i].Terminals[j]);
            }
        }
        printf("}\n");
    }
}

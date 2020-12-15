#include "follow.h"

#include <stdio.h>
#include <stdlib.h>
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
 * @param pRuleHead 文法链表的头指针
 * @param pVoidTable 空表的指针
 * @param pFirstSetList Follow 集的指针
 * @return SetList* 生成 First 集的指针
 */
SetList *GenFollowSetList(const Rule *pRuleHead, const VoidTable *pVoidTable,
                          const SetList *pFirstSetList) {
    SetList *pFollowSetList = malloc(sizeof(SetList));
    pFollowSetList->nSetCount = 0;

    const Rule *pRule;
    const Production *pProduction;
    const Symbol *pSymbol;

    /**
     * @brief Follow 集是否被修改的标志
     */
    int isChange;

    /**
     * @brief 初始化 Follow 集
     * 在开始符号的 Follow 子集中加入结束符
     */
    for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
        AddOneSet(pFollowSetList, pRule->RuleName);
    }
    AddTerminalToSet(&pFollowSetList->Sets[0], END_SYMBOL);

    do {
        isChange = 0;
        for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
            for (pProduction = pRule->pFirstProduction; pProduction != NULL;
                 pProduction = pProduction->pNextProduction) {
                for (pSymbol = pProduction; pSymbol != NULL;
                     pSymbol = pSymbol->pNextSymbol) {
                    if (pSymbol->isToken)
                        continue; // 忽略终结符

                    /**
                     * @brief 初始化一个临时子集
                     */
                    Set TmpSet;
                    TmpSet.nTerminalCount = 0;

                    Symbol *pTmpSymbol;

                    /**
                     * @brief
                     * 从当前符号向后查找，
                     * 如果为非 ε 的终结符，则将其加入临时子集，
                     * 并结束向后查找；
                     * 如果为非终结符，
                     * 则将其对应的 First 子集中的符号加入临时子集，
                     * 并且，当其不能推出 ε 时结束向后查找；
                     */
                    for (pTmpSymbol = pSymbol->pNextSymbol; pTmpSymbol != NULL;
                         pTmpSymbol = pTmpSymbol->pNextSymbol) {
                        if (pTmpSymbol->isToken) {
                            if (strcmp(pTmpSymbol->SymbolName, VOID_SYMBOL) !=
                                0) {
                                AddTerminalToSet(&TmpSet,
                                                 pTmpSymbol->SymbolName);
                                break;
                            }
                        } else {
                            AddSetToSet(
                                &TmpSet,
                                GetSet(pFirstSetList, pTmpSymbol->SymbolName));
                            if (!*FindHasVoid(pVoidTable,
                                              pTmpSymbol->SymbolName)) {
                                break;
                            }
                        }
                    }

                    /**
                     * @brief Follow 集中不包含 ε
                     * 所以要在临时子集中删去 ε 之后再合并到目标 Follow 子集。
                     */
                    RemoveVoidFromSet(&TmpSet);

                    /**
                     * @brief 目标 Follow 子集
                     */
                    Set *pDesSet = GetSet(pFollowSetList, pSymbol->SymbolName);
                    isChange = AddSetToSet(pDesSet, &TmpSet) || isChange;

                    /**
                     * @brief
                     * 如果当前符号是最后一个符号或后续符号可以推出空
                     * 则将该产生式对应文法的 Follow 子集中的符号
                     * 加入目标 Follow子集。
                     */
                    if (pTmpSymbol == NULL) {
                        isChange =
                            AddSetToSet(pDesSet, GetSet(pFollowSetList,
                                                        pRule->RuleName)) ||
                            isChange;
                    }
                }
            }
        }
    } while (isChange);
    return pFollowSetList;
}

/**
 * @brief 输出 Follow 集
 *
 * @param pFollowSetList Follow 集指针
 */
void PrintFollowSetList(const SetList *pFollowSetList) {
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

#include "follow.h"

#include <stdio.h>
#include <string.h>

#include "first.h"

/*
功能：
        将源 Set 添加到目标 Set 中，忽略ε。

参数：
        pDesSet -- 目标 Set 指针。
        pSrcSet -- 源 Set 指针。

返回值：
        添加成功返回 1，否则返回 0。
*/
void RemoveVoidFromSet(Set *pSet) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminal[i], VoidSymbol) == 0) {
            if (i != pSet->nTerminalCount - 1) {
                memmove(
                    pSet->Terminal + i, pSet->Terminal + i + 1,
                    sizeof(pSet->Terminal[0]) * (pSet->nTerminalCount - i - 1));
            }
            pSet->nTerminalCount--;
            return;
        }
    }
}
/*
功能：
        求文法的 Follow 集合。

参数：
        pHead -- 文法的头指针。
        pFollowSetList -- Follow 集合指针。
        pFirstSetList -- First 集合指针。
*/
void Follow(const Rule *pHead, VoidTable *pVoidTable, SetList *pFollowSetList,
            SetList *pFirstSetList) {
    const Rule *pRule;    // Rule 指针
    int isChange;         // 集合是否被修改的标志
    RuleSymbol *pSymbol;  // Symbol 游标

    // 使用文法链表初始化 Follow 集合
    for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
        AddOneSet(pFollowSetList, pRule->RuleName);
    }

    // 在开始符号的 Follow 集合中加入结束符 #
    AddTerminalToSet(&pFollowSetList->Sets[0], EndSymbol);

    do {
        isChange = 0;  // 设置集合修改标志

        for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
            for (RuleSymbol *pSelect = pRule->pFirstSymbol; pSelect != NULL;
                 pSelect = pSelect->pOther) {
                for (pSymbol = pSelect; pSymbol != NULL;
                     pSymbol = pSymbol->pNextSymbol) {
                    // 忽略终结符
                    if (pSymbol->isToken) continue;

                    // 初始化一个 First 集合 TempFirstSet，作为临时变量
                    Set TempFirstSet;
                    TempFirstSet.nTerminalCount = 0;
                    RuleSymbol *pTmpSymbol;
                    for (pTmpSymbol = pSymbol->pNextSymbol; pTmpSymbol != NULL;
                         pTmpSymbol = pTmpSymbol->pNextSymbol) {
                        if (pTmpSymbol->isToken) {
                            if (strcmp(pTmpSymbol, VoidSymbol) != 0) {
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

/*
功能：
        输出 First 集合。

参数：
        pFirstSetList -- First 集合指针。
*/
void PrintFollowSet(SetList *pFollowSetList) {
    printf("\nThe Follow Set:\n");
    for (int i = 0; i < pFollowSetList->nSetCount; i++) {
        printf("Follow(%s) = { ", pFollowSetList->Sets[i].Name);
        for (int j = 0; j < pFollowSetList->Sets[i].nTerminalCount; j++) {
            if (j == pFollowSetList->Sets[i].nTerminalCount - 1) {
                printf("%s ", pFollowSetList->Sets[i].Terminal[j]);
            } else {
                printf("%s, ", pFollowSetList->Sets[i].Terminal[j]);
            }
        }
        printf("}\n");
    }
}

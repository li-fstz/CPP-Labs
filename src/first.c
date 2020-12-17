#include "first.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rule.h"
#include "voidtable.h"

/**
 * @brief 输出 First 集
 *
 * @param pFirstSetList First 集指针
 */
void PrintFirstSetList(const SetList *pFirstSetList) {
    printf("\nThe First Set:\n");
    for (int i = 0; i < pFirstSetList->nSetCount; i++) {
        printf("First(%s) = { ", pFirstSetList->Sets[i].Name);
        for (int j = 0; j < pFirstSetList->Sets[i].nTerminalCount; j++) {
            if (j == pFirstSetList->Sets[i].nTerminalCount - 1) {
                printf("%s ", pFirstSetList->Sets[i].Terminals[j]);
            } else {
                printf("%s, ", pFirstSetList->Sets[i].Terminals[j]);
            }
        }
        printf("}\n");
    }
}

/**
 * @brief 生成文法的 First 集
 *
 * @param pRuleHead 文法链表的头指针
 * @param pVoidTable 空表的指针
 * @return SetList* 生成的 First 集的指针
 */
SetList *GenFirstSetList(const Rule *pRuleHead, const VoidTable *pVoidTable) {
    SetList *pFirstSetList = malloc(sizeof(SetList));
    pFirstSetList->nSetCount = 0;

    const Rule *pRule;
    const Production *pProduction;
    const Symbol *pSymbol;

    /**
     * @brief First 集是否被修改的标志
     */
    int isChange;

    /**
     * @brief 初始化 First 集
     * 如果该文法可以推出 ε，则向 First 集中加 ε
     */
    for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
        AddOneSet(pFirstSetList, pRule->RuleName);
        if (*FindHasVoid(pVoidTable, pRule->RuleName)) {
            AddTerminalToSet(pFirstSetList->Sets + pFirstSetList->nSetCount - 1,
                             VOID_SYMBOL);
        }
    }

    do {
        isChange = 0;
        for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {

            /**
             * @brief 该文法对应的 First 子集
             */
            Set *pDesSet = GetSet(pFirstSetList, pRule->RuleName);
            for (pProduction = pRule->pFirstProduction; pProduction != NULL;
                 pProduction = pProduction->pNextProduction) {
                for (pSymbol = pProduction; pSymbol != NULL;
                     pSymbol = pSymbol->pNextSymbol) {

                    /**
                     * @brief
                     *
                     * 如果是终结符，
                     * 则将该终结符加入到该文法 First 子集中，
                     * 并结束对该产生式的查找；
                     *
                     * 如果是非终结符，
                     * 则将非终结符对应的文法 First 子集中的终结符添加到
                     * 该文法 First 子集中，
                     * 如果非终结符对应的文法不能推出空,则结束对该产生式的查找。
                     */
                    if (pSymbol->isToken) {
                        isChange =
                            AddTerminalToSet(pDesSet, pSymbol->SymbolName) ||
                            isChange;
                        break;
                    } else {

                        /**
                         * @brief 非终结符对应的文法 First 子集
                         */
                        Set *pSrcSet =
                            GetSet(pFirstSetList, pSymbol->SymbolName);
                        isChange = AddSetToSet(pDesSet, pSrcSet) || isChange;
                        if (!*FindHasVoid(pVoidTable, pSrcSet->Name)) {
                            break;
                        }
                    }
                }
            }
        }
    } while (isChange);
    return pFirstSetList;
}

/**
 * @brief 在 First 集或 Follow 集中添加一个非终结符子集
 *
 * @param pSetList First 集或 Follow 集的指针
 * @param pName 非终结符子集名字
 */
void AddOneSet(SetList *pSetList, const char *pName) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (strcmp(pSetList->Sets[i].Name, pName) == 0) {
            return;
        }
    }
    pSetList->Sets[pSetList->nSetCount].nTerminalCount = 0;
    strcpy(pSetList->Sets[pSetList->nSetCount++].Name, pName);
}

/**
 * @brief 在 First 集或 Follow 集中查找一个非终结符子集
 *
 * @param pSetList First 集或 Follow 集的指针
 * @param pName 非终结符子集名字
 * @return Set* 如果找到则返回子集的指针，否则返回 NULL
 */
Set *GetSet(const SetList *pSetList, const char *pName) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (strcmp(pSetList->Sets[i].Name, pName) == 0) {
            return pSetList->Sets + i;
        }
    }
    return NULL;
}

/**
 * @brief 添加一个终结符到子集
 *
 * @param pSet 子集指针
 * @param Terminal 终结符
 * @return int 对子集是否有修改
 */
int AddTerminalToSet(Set *pSet, const char *Terminal) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminals[i], Terminal) == 0) {
            return 0;
        }
    }
    strcpy(pSet->Terminals[pSet->nTerminalCount++], Terminal);
    return 1;
}

/**
 * @brief 将源子集中的所有终结符添加至目标子集中
 *
 * @param pDesSet 目标子集指针
 * @param pSrcSet 源子集指针
 * @return int 对目标子集是否有修改
 */
int AddSetToSet(Set *pDesSet, const Set *pSrcSet) {
    int flag = 0;
    for (int i = 0; i < pSrcSet->nTerminalCount; i++) {
        flag = AddTerminalToSet(pDesSet, pSrcSet->Terminals[i]) || flag;
    }
    return flag;
}

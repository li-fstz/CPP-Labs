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
void PrintFirstSet(SetList *pFirstSetList) {
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
 * @param pRuleHead 文法的头指针
 * @param pVoidTable 空表的指针
 * @param pFirstSetList First 集的指针
 */
void GenFirstSet(const Rule *pRuleHead, VoidTable *pVoidTable,
                 SetList *pFirstSetList) {
    const Rule *pRule; // Rule 指针
    int isChange;      // 集合是否被修改的标志
    Symbol *pSymbol;   // Symbol 游标
    // 使用文法链表初始化 First 集合
    for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
        AddOneSet(pFirstSetList, pRule->RuleName);
        if (*FindHasVoid(pVoidTable, pRule->RuleName)) {
            AddTerminalToSet(pFirstSetList->Sets + pFirstSetList->nSetCount - 1,
                             VOID_SYMBOL);
        }
    }

    do {
        isChange = 0; // 设置修改标志
        for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
            // 根据文法名称在 pFirstSetList 中查找 Set
            Set *pDesSet = GetSet(pFirstSetList, pRule->RuleName);
            for (Production *pProduction = pRule->pFirstProduction;
                 pProduction != NULL;
                 pProduction = pProduction->pNextProduction) {
                int hasVoid = 1; // First 集合中是否含有ε的标志
                for (pSymbol = pProduction; pSymbol != NULL && hasVoid;
                     pSymbol = pSymbol->pNextSymbol) {
                    if (pSymbol->isToken) // 终结符
                    {
                        // 调用 AddTerminalToSet 函数将终结符添加到
                        // pDesSet，并设置修改标志
                        if (AddTerminalToSet(pDesSet, pSymbol->SymbolName))
                            isChange = 1;
                        hasVoid = 0; // 设置 First 集合中是否含有ε的标志
                    } else           // 非终结符
                    {
                        // 根据非终结符名称在 pFirstSetList 中查找 Set
                        Set *pSrcSet =
                            GetSet(pFirstSetList, pSymbol->SymbolName);

                        // 调用 AddSetToSet 函数，将源 Set
                        // 中的所有终结符添加到目标 Set 中，并设置修改标志
                        if (AddSetToSet(pDesSet, pSrcSet))
                            isChange = 1;
                        hasVoid = *FindHasVoid(pVoidTable, pSrcSet->Name);
                    }
                }
            }
        }
    } while (isChange);
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
    strcpy(pSetList->Sets[pSetList->nSetCount++].Name, pName);
}

/**
 * @brief 在 First 集或 Follow 集中查找一个非终结符子集
 *
 * @param pSetList First 集或 Follow 集的指针
 * @param pName 非终结符子集名字
 * @return Set* 如果找到则返回子集的指针，否则返回 NULL
 */
Set *GetSet(SetList *pSetList, const char *pName) {
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
 * @brief 将源子集中的终结符添加至目标子集中
 *
 * @param pDesSet 目标子集指针
 * @param pSrcSet 源子集指针
 * @return int 对目标子集是否有修改
 */
int AddSetToSet(Set *pDesSet, const Set *pSrcSet) {
    int flag = 0;
    for (int i = 0; i < pSrcSet->nTerminalCount; i++) {
        if (AddTerminalToSet(pDesSet, pSrcSet->Terminals[i])) {
            flag = 1;
        }
    }
    return flag;
}

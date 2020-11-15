#include "first.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rule.h"
#include "voidtable.h"

/*
功能：
        输出 First 集合。

参数：
        pFirstSetList -- First 集合指针。
*/
void PrintFirstSet(SetList *pFirstSetList) {
    printf("\nThe First Set:\n");
    for (int i = 0; i < pFirstSetList->nSetCount; i++) {
        printf("First(%s) = { ", pFirstSetList->Sets[i].Name);
        for (int j = 0; j < pFirstSetList->Sets[i].nTerminalCount; j++) {
            if (j == pFirstSetList->Sets[i].nTerminalCount - 1) {
                printf("%s ", pFirstSetList->Sets[i].Terminal[j]);
            } else {
                printf("%s, ", pFirstSetList->Sets[i].Terminal[j]);
            }
        }
        printf("}\n");
    }
}

/*
功能：
        求文法的 First 集合。

参数：
        pHead -- 文法的头指针。
        pFirstSetList -- First 集合指针。
*/
void First(const Rule *pHead, Column *pVoidTable, SetList *pFirstSetList) {
    const Rule *pRule;    // Rule 指针
    int isChange;         // 集合是否被修改的标志
    RuleSymbol *pSymbol;  // Symbol 游标

    // 使用文法链表初始化 First 集合
    for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
        AddOneSet(pFirstSetList, pRule->RuleName);
        if (FindColumn(pVoidTable, pRule->RuleName)->hasVoid) {
            AddTerminalToSet(pFirstSetList->Sets + pFirstSetList->nSetCount - 1,
                             VoidSymbol);
        }
    }

    do {
        isChange = 0;  // 设置修改标志

        for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
            // 根据文法名称在 pFirstSetList 中查找 Set
            Set *pDesSet = GetSet(pFirstSetList, pRule->RuleName);
            for (RuleSymbol *pSelect = pRule->pFirstSymbol; pSelect != NULL;
                 pSelect = pSelect->pOther) {
                int hasVoid = 1;  // First 集合中是否含有ε的标志
                for (pSymbol = pSelect; pSymbol != NULL && hasVoid;
                     pSymbol = pSymbol->pNextSymbol) {
                    if (pSymbol->isToken)  // 终结符
                    {
                        // 调用 AddTerminalToSet 函数将终结符添加到
                        // pDesSet，并设置修改标志
                        if (AddTerminalToSet(pDesSet, pSymbol->SymbolName))
                            isChange = 1;

                        hasVoid = 0;  // 设置 First 集合中是否含有ε的标志
                    } else            // 非终结符
                    {
                        // 根据非终结符名称在 pFirstSetList 中查找 Set
                        Set *pSrcSet =
                            GetSet(pFirstSetList, pSymbol->SymbolName);

                        // 调用 AddSetToSet 函数，将源 Set
                        // 中的所有终结符添加到目标 Set 中，并设置修改标志
                        if (AddSetToSet(pDesSet, pSrcSet)) isChange = 1;

                        // 调用 SetHasVoid 函数，判断源 Set 中是否含有ε
                        hasVoid =
                            FindColumn(pVoidTable, pSrcSet->Name)->hasVoid;
                    }
                }
            }
        }
    } while (isChange);
}

/*
功能：
        添加一个 Set 到 SetList。

参数：
        pSetList -- SetList 指针。
        pName -- 集合名称字符串指针。
*/
void AddOneSet(SetList *pSetList, const char *pName) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (strcmp(pSetList->Sets[i].Name, pName) == 0) {
            return;
        }
    }
    strcpy(pSetList->Sets[pSetList->nSetCount++].Name, pName);
}

/*
功能：
        根据名称在 SetList 中查找 Set。

参数：
        pSetList -- SetList 指针。
        pName -- 集合名称字符串指针。

返回值：
        如果找到返回 Set 的指针，否则返回 NULL。
*/
Set *GetSet(SetList *pSetList, const char *pName) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (strcmp(pSetList->Sets[i].Name, pName) == 0) {
            return pSetList->Sets + i;
        }
    }
    return NULL;
}

/*
功能：
        添加一个终结符到 Set。

参数：
        pSet -- Set 指针。
        pTerminal -- 终结符名称指针。

返回值：
        添加成功返回 1。
        添加不成功返回 0。
*/
int AddTerminalToSet(Set *pSet, const char *pTerminal) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminal[i], pTerminal) == 0) {
            return 0;
        }
    }
    strcpy(pSet->Terminal[pSet->nTerminalCount++], pTerminal);
    return 1;
}

/*
功能：
        将源 Set 中的所有终结符添加到目标 Set 中。

参数：
        pDesSet -- 目标 Set 指针。
        pSrcSet -- 源 Set 指针。

返回值：
        添加成功返回 1，否则返回 0。
*/
int AddSetToSet(Set *pDesSet, const Set *pSrcSet) {
    int flag = 0;
    for (int i = 0; i < pSrcSet->nTerminalCount; i++) {
        if (AddTerminalToSet(pDesSet, pSrcSet->Terminal[i])) {
            flag = 1;
        }
    }
    return flag;
}

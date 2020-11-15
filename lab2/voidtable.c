#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
功能：
    初始化空表。

返回值：
    空表的头指针。
*/
Column *InitVoidTable(Rule *pRule) {
    Column *pHead, **pColumnPrePtr = &pHead;
    for (; pRule != NULL; pRule = pRule->pNextRule) {
        *pColumnPrePtr = (Column *)malloc(sizeof(Column));
        strcpy((*pColumnPrePtr)->RuleName, pRule->RuleName);
        (*pColumnPrePtr)->hasVoid = -1;
        (*pColumnPrePtr)->pNextCol = NULL;
        pColumnPrePtr = &(*pColumnPrePtr)->pNextCol;
    }
    return pHead;
}

/*
功能：
    输出空表。

参数：
    pHead -- 空表的头指针。
*/
void PrintTable(Column *pHead) {
    if (pHead) {
        printf("%s\t%d\n", pHead->RuleName, pHead->hasVoid);
        PrintTable(pHead->pNextCol);
    }
}

/*
功能：
    求出文法的空表。

参数：
    pRuleHead -- 文法的头指针。
    pTableHead -- 空表的头指针。
*/
void VoidTable(Rule *pRuleHead, Column *pTableHead) {
    Rule *pCopiedRule = CopyRule(pRuleHead);
    Rule *pRule = pCopiedRule, **pRulePrePtr = &pCopiedRule;
    while (pRule != NULL) {
        RuleSymbol *pSelect = pRule->pFirstSymbol,
                   **pSelectPrePtr = &pRule->pFirstSymbol;
        int hasVoidSelect = 0;
        while (pSelect != NULL) {
            if (pSelect->pNextSymbol == NULL &&
                strcmp(pSelect->SymbolName, VoidSymbol) == 0) {
                hasVoidSelect = 1;
                break;
            }

            int deleteThisSelect = 0;
            for (RuleSymbol *pSymble = pSelect; pSymble != NULL;
                 pSymble = pSymble->pNextSymbol) {
                if (pSymble->isToken &&
                    strcmp(pSymble->SymbolName, VoidSymbol) != 0) {
                    deleteThisSelect = 1;
                    break;
                }
            }
            if (deleteThisSelect) {
                *pSelectPrePtr = pSelect->pOther;
                // FreeSelect(pSelect);
                pSelect = *pSelectPrePtr;
            } else {
                pSelectPrePtr = &pSelect->pOther;
                pSelect = pSelect->pOther;
            }
        }
        if (hasVoidSelect) {
            FindColumn(pTableHead, pRule->RuleName)->hasVoid = 1;
            *pRulePrePtr = pRule->pNextRule;
            // FreeRule(pRule);
            pRule = *pRulePrePtr;
        } else if (pRule->pFirstSymbol == NULL) {
            FindColumn(pTableHead, pRule->RuleName)->hasVoid = 0;
            *pRulePrePtr = pRule->pNextRule;
            // FreeRule(pRule);
            pRule = *pRulePrePtr;
        } else {
            pRulePrePtr = &pRule->pNextRule;
            pRule = pRule->pNextRule;
        }
    }

    int isChange = 0;
    do {
        isChange = 0;
        Rule *pRule = pCopiedRule, **pRulePrePtr = &pCopiedRule;
        while (pRule != NULL) {
            int hasAllNotVoidSelect, hasVoidSelect;
            for (RuleSymbol *pSelect = pRule->pFirstSymbol; pSelect != NULL;
                 pSelect = pSelect->pOther) {
                RuleSymbol *pSymble = pSelect,
                           **pSymblePrePtr = &pRule->pFirstSymbol;
                hasAllNotVoidSelect = 1;
                hasVoidSelect = 0;
                while (pSymble != NULL) {
                    switch (
                        FindColumn(pTableHead, pSymble->SymbolName)->hasVoid) {
                        case 1:
                            if (pSymble->pNextSymbol == NULL &&
                                pSymblePrePtr == &pRule->pFirstSymbol) {
                                hasVoidSelect = 1;
                                hasAllNotVoidSelect = 0;
                            } else {
                                *pSymblePrePtr = pSymble->pNextSymbol;
                                pSymble->pNextSymbol->pOther = pSymble->pOther;
                                // FreeSymble(pSymble);
                                pSymble = *pSymblePrePtr;
                            }
                            break;
                        case -1:
                            hasAllNotVoidSelect = 0;
                        case 0:
                            pSymblePrePtr = &pSymble->pNextSymbol;
                            pSymble = pSymble->pNextSymbol;
                            break;
                    }
                    if (hasVoidSelect) {
                        break;
                    }
                }
                if (hasAllNotVoidSelect || hasVoidSelect) {
                    break;
                }
            }
            if (hasAllNotVoidSelect || hasVoidSelect) {
                isChange = 1;
                FindColumn(pTableHead, pRule->RuleName)->hasVoid =
                    hasVoidSelect;
                *pRulePrePtr = pRule->pNextRule;
                // FreeRule(pRule);
                pRule = *pRulePrePtr;
            } else {
                pRulePrePtr = &pRule->pNextRule;
                pRule = pRule->pNextRule;
            }
        }
    } while (isChange);
}

/*
功能：
    根据 RuleName 在空表链表中查找名字相同的列。

参数：
    pTableHead -- 空表链表的头指针。
    RuleName -- 文法的名字。

返回值：
    如果存在名字相同的列返回 Column 指针，否则返回 NULL
*/
Column *FindColumn(Column *pTableHead, const char *RuleName) {
    for (; pTableHead != NULL; pTableHead = pTableHead->pNextCol) {
        if (strcmp(pTableHead->RuleName, RuleName) == 0) {
            return pTableHead;
        }
    }
    return NULL;
}

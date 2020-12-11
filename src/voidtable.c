#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
功能：
    输出空表。

参数：
    pRuleHead -- 空表的头指针。

/**
 * @brief
 *
 * @param pVoidTable
 */
void PrintVoidTable(VoidTable *pVoidTable) {
    for (int i = 0; i < pVoidTable->ColCount; i++) {
        printf("%s%c", pVoidTable->pTableHead[i],
               i == pVoidTable->ColCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; i < pVoidTable->ColCount; i++) {
        printf("%d%c", pVoidTable->TableRows[0].hasVoid[i],
               i == pVoidTable->ColCount - 1 ? '\n' : '\t');
    }
}

/*
功能：
    求出文法的空表。

参数：
    pRuleHead -- 文法的头指针。
    pVoidTable -- 空表的头指针。

/**
 * @brief
 *
 * @param pRuleHead
 * @param pVoidTable
 */
void GenVoidTable(Rule *pRuleHead, VoidTable *pVoidTable) {
    pVoidTable->pTableHead = GetNonTerminals(pRuleHead);
    for (pVoidTable->ColCount = 0; pVoidTable->pTableHead[pVoidTable->ColCount];
         pVoidTable->ColCount++) {
        pVoidTable->TableRows[0].hasVoid[pVoidTable->ColCount] = -1;
    }
    Rule *pCopiedRule = CopyRule(pRuleHead);
    Rule *pRule = pCopiedRule, **pRulePrePtr = &pCopiedRule;
    while (pRule != NULL) {
        Production *pProduction = pRule->pFirstProduction,
                   **pProductionPrePtr = &pRule->pFirstProduction;
        int hasVoidProduction = 0;
        while (pProduction != NULL) {
            if (pProduction->pNextSymbol == NULL &&
                strcmp(pProduction->SymbolName, VOID_SYMBOL) == 0) {
                hasVoidProduction = 1;
                break;
            }
            int deleteThisProduction = 0;
            for (Symbol *pSymble = pProduction; pSymble != NULL;
                 pSymble = pSymble->pNextSymbol) {
                if (pSymble->isToken &&
                    strcmp(pSymble->SymbolName, VOID_SYMBOL) != 0) {
                    deleteThisProduction = 1;
                    break;
                }
            }
            if (deleteThisProduction) {
                *pProductionPrePtr = pProduction->pNextProduction;
                // FreeProduction(pProduction);
                pProduction = *pProductionPrePtr;
            } else {
                pProductionPrePtr = &pProduction->pNextProduction;
                pProduction = pProduction->pNextProduction;
            }
        }
        if (hasVoidProduction) {
            *FindHasVoid(pVoidTable, pRule->RuleName) = 1;
            *pRulePrePtr = pRule->pNextRule;
            // FreeRule(pRule);
            pRule = *pRulePrePtr;
        } else if (pRule->pFirstProduction == NULL) {
            *FindHasVoid(pVoidTable, pRule->RuleName) = 0;
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
            int hasAllNotVoidProduction, hasVoidProduction;
            for (Production *pProduction = pRule->pFirstProduction;
                 pProduction != NULL;
                 pProduction = pProduction->pNextProduction) {
                Symbol *pSymble = pProduction,
                       **pSymblePrePtr = &pRule->pFirstProduction;
                hasAllNotVoidProduction = 1;
                hasVoidProduction = 0;
                while (pSymble != NULL) {
                    switch (*FindHasVoid(pVoidTable, pSymble->SymbolName)) {
                    case 1:
                        if (pSymble->pNextSymbol == NULL &&
                            pSymblePrePtr == &pRule->pFirstProduction) {
                            hasVoidProduction = 1;
                            hasAllNotVoidProduction = 0;
                        } else {
                            *pSymblePrePtr = pSymble->pNextSymbol;
                            if (pSymble->pNextSymbol) {
                                pSymble->pNextSymbol->pNextProduction =
                                    pSymble->pNextProduction;
                            }
                            // FreeSymble(pSymble);
                            pSymble = *pSymblePrePtr;
                        }
                        break;
                    case -1:
                        hasAllNotVoidProduction = 0;
                    case 0:
                        pSymblePrePtr = &pSymble->pNextSymbol;
                        pSymble = pSymble->pNextSymbol;
                        break;
                    }
                    if (hasVoidProduction) {
                        break;
                    }
                }
                if (hasAllNotVoidProduction || hasVoidProduction) {
                    break;
                }
            }
            if (hasAllNotVoidProduction || hasVoidProduction) {
                isChange = 1;
                *FindHasVoid(pVoidTable, pRule->RuleName) = hasVoidProduction;
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
    pVoidTable -- 空表链表的头指针。
    RuleName -- 文法的名字。

返回值：
    如果存在名字相同的列返回 Column 指针，否则返回 NULL

/**
 * @brief
 *
 * @param pTable
 * @param RuleName
 * @return int*
 */
int *FindHasVoid(VoidTable *pTable, const char *RuleName) {
    for (int i = 0; i < pTable->ColCount; i++) {
        if (strcmp(pTable->pTableHead[i], RuleName) == 0) {
            return pTable->TableRows[0].hasVoid + i;
        }
    }
    return 0;
}

char **GetNonTerminals(const Rule *pRule) {
    char **pNonTerminals = calloc(32, sizeof(char *));
    for (int i = 0; pRule != NULL; pRule = pRule->pNextRule, i++) {
        pNonTerminals[i] = malloc(MAX_STR_LENGTH);
        strcpy(pNonTerminals[i], pRule->RuleName);
    }
    return pNonTerminals;
}

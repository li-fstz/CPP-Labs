#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 输出空表
 *
 * @param pVoidTable 空表的指针
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

/**
 * @brief 生成文法的空表
 *
 * @param pRuleHead 文法链表的头指针
 * @param pVoidTable 空表的指针
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

/**
 * @brief 在空表中根据文法名字找到值的位置
 *
 * @param pTable 空表的指针
 * @param RuleName 文法的名字
 * @return int* 值在空表中的位置
 */
int *FindHasVoid(VoidTable *pTable, const char *RuleName) {
    for (int i = 0; i < pTable->ColCount; i++) {
        if (strcmp(pTable->pTableHead[i], RuleName) == 0) {
            return pTable->TableRows[0].hasVoid + i;
        }
    }
    return 0;
}

/**
 * @brief 从文法中提取所有的非终结符
 * 
 * @param pRule 文法链表的头指针
 * @return char** 非终结符数组
 */
char **GetNonTerminals(const Rule *pRuleHead) {
    char **pNonTerminals = calloc(32, sizeof(char *));
    for (int i = 0; pRuleHead != NULL; pRuleHead = pRuleHead->pNextRule, i++) {
        pNonTerminals[i] = malloc(MAX_STR_LENGTH);
        strcpy(pNonTerminals[i], pRuleHead->RuleName);
    }
    return pNonTerminals;
}

#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
功能：
    输出空表。

参数：
    pHead -- 空表的头指针。
*/
void PrintTable(VoidTable *pVoidTable) {
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
            *FindHasVoid(pVoidTable, pRule->RuleName) = 1;
            *pRulePrePtr = pRule->pNextRule;
            // FreeRule(pRule);
            pRule = *pRulePrePtr;
        } else if (pRule->pFirstSymbol == NULL) {
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
            int hasAllNotVoidSelect, hasVoidSelect;
            for (RuleSymbol *pSelect = pRule->pFirstSymbol; pSelect != NULL;
                 pSelect = pSelect->pOther) {
                RuleSymbol *pSymble = pSelect,
                           **pSymblePrePtr = &pRule->pFirstSymbol;
                hasAllNotVoidSelect = 1;
                hasVoidSelect = 0;
                while (pSymble != NULL) {
                    switch (*FindHasVoid(pVoidTable, pSymble->SymbolName)) {
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
                *FindHasVoid(pVoidTable, pRule->RuleName) = hasVoidSelect;
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

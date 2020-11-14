#include <stdlib.h>
#include <string.h>

#include "VoidTable.h"

const char *VoidSymbol = "$";  // "ε"

char rule_table_ci[20][256];
char ruleNameArr[20][64];

int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pHead = InitRules();

    //
    // 输出文法
    //
    PrintRule(pHead);

    //
    // 调用 VoidTable 函数求文法的空表
    //
    Column *pVoidTable = InitVoidTable(pHead);
    VoidTable(pHead, pVoidTable);

    //
    // 输出空表
    //
    printf("\nThe Void Table:\n");
    PrintTable(pVoidTable);
    return 0;
}

/*
功能：
    求出文法的空表。

参数：
    pRuleHead -- 文法的头指针。
    pTableHead -- 空表的头指针。
*/
void VoidTable(Rule *pRuleHead, Column *pTableHead) {
    // Rule *pCopiedRule = CopyRule(pRuleHead);
    Rule *pCopiedRule = pRuleHead;
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
Column *FindColumn(Column *pTableHead, char *RuleName) {
    for (; pTableHead != NULL; pTableHead = pTableHead->pNextCol) {
        if (strcmp(pTableHead->RuleName, RuleName) == 0) {
            return pTableHead;
        }
    }
    return NULL;
}

/*
功能：
    使用给定的数据初始化文法链表

返回值：
    Rule 指针
*/
typedef struct _SYMBOL {
    int isToken;
    char Name[MAX_STR_LENGTH];
} SYMBOL;

typedef struct _RULE_ENTRY {
    char RuleName[MAX_STR_LENGTH];
    SYMBOL Selects[64][64];
} RULE_ENTRY;

/* S -> AB | bC
   A -> $ | b
   B -> $ | aD
   C -> AD | b
   D -> aS | c */
static const RULE_ENTRY rule_table[] = {
    {"S", {{{0, "A"}, {0, "B"}}, {{1, "b"}, {0, "C"}}}},
    {"A", {{{1, "$"}}, {{1, "b"}}}},
    {"B", {{{1, "$"}}, {{1, "a"}, {0, "D"}}}},
    {"C", {{{0, "A"}, {0, "D"}}, {{1, "b"}}}},
    {"D", {{{1, "a"}, {0, "S"}}, {{1, "c"}}}}};

/*
功能：
    初始化文法链表。

返回值：
    文法的头指针。
*/
Rule *InitRules() {
    Rule *pHead, *pRule;
    RuleSymbol **pSymbolPtr1, **pSymbolPtr2;
    int nRuleCount = sizeof(rule_table) / sizeof(rule_table[0]);
    int i, j, k;

    Rule **pRulePtr = &pHead;
    for (i = 0; i < nRuleCount; i++) {
        *pRulePtr = CreateRule(rule_table[i].RuleName);
        pRulePtr = &(*pRulePtr)->pNextRule;
    }

    pRule = pHead;
    for (i = 0; i < nRuleCount; i++) {
        pSymbolPtr1 = &pRule->pFirstSymbol;
        for (j = 0; rule_table[i].Selects[j][0].Name[0] != '\0'; j++) {
            pSymbolPtr2 = pSymbolPtr1;
            for (k = 0; rule_table[i].Selects[j][k].Name[0] != '\0'; k++) {
                const SYMBOL *pSymbol = &rule_table[i].Selects[j][k];

                *pSymbolPtr2 = CreateSymbol();
                (*pSymbolPtr2)->isToken = pSymbol->isToken;
                strcpy((*pSymbolPtr2)->SymbolName, pSymbol->Name);
                if (!pSymbol->isToken) {
                    (*pSymbolPtr2)->pRule = FindRule(pHead, pSymbol->Name);
                    if (NULL == (*pSymbolPtr2)->pRule) {
                        printf("Init rules error, miss rule \"%s\"\n",
                               pSymbol->Name);
                        exit(1);
                    }
                }

                pSymbolPtr2 = &(*pSymbolPtr2)->pNextSymbol;
            }

            pSymbolPtr1 = &(*pSymbolPtr1)->pOther;
        }

        pRule = pRule->pNextRule;
    }

    return pHead;
}

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
    创建一个新的 Rule。

参数：
    pRuleName -- 文法的名字。

返回值：
    Rule 指针
*/
Rule *CreateRule(const char *pRuleName) {
    Rule *pRule = (Rule *)malloc(sizeof(Rule));

    strcpy(pRule->RuleName, pRuleName);
    pRule->pFirstSymbol = NULL;
    pRule->pNextRule = NULL;

    return pRule;
}

/*
功能：
    创建一个新的 Symbol。

返回值：
    RuleSymbol 指针
*/
RuleSymbol *CreateSymbol() {
    RuleSymbol *pSymbol = (RuleSymbol *)malloc(sizeof(RuleSymbol));

    pSymbol->pNextSymbol = NULL;
    pSymbol->pOther = NULL;
    pSymbol->isToken = -1;
    pSymbol->SymbolName[0] = '\0';
    pSymbol->pRule = NULL;

    return pSymbol;
}

/*
功能：
    根据 RuleName 在文法链表中查找名字相同的文法。

参数：
    pHead -- 文法链表的头指针。
    RuleName -- 文法的名字。

返回值：
    如果存在名字相同的文法返回 Rule 指针，否则返回 NULL
*/
Rule *FindRule(Rule *pHead, const char *RuleName) {
    Rule *pRule;
    for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
        if (0 == strcmp(pRule->RuleName, RuleName)) {
            break;
        }
    }

    return pRule;
}

/*
功能：
    输出文法。

参数：
    pHead -- 文法链表的头指针。
*/
void PrintRule(Rule *pHead) {
    if (pHead) {
        printf("%s->", pHead->RuleName);
        for (RuleSymbol *pSelect = pHead->pFirstSymbol; pSelect != NULL;
             pSelect = pSelect->pOther) {
            for (RuleSymbol *pSymble = pSelect; pSymble != NULL;
                 pSymble = pSymble->pNextSymbol) {
                printf("%s", pSymble->SymbolName);
            }
            putchar(pSelect->pOther ? '|' : '\n');
        }
        PrintRule(pHead->pNextRule);
    }
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

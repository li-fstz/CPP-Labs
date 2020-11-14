#include <stdlib.h>
#include <string.h>

#include "First.h"

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
    PrintTable(pVoidTable);

    //
    // 调用 First 函数求文法的 First 集合
    //
    SetList FirstSet;
    FirstSet.nSetCount = 0;
    First(pHead, pVoidTable, &FirstSet);

    //
    // 输出First集合
    //
    PrintFirstSet(&FirstSet);

    return 0;
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

/*
功能：
        判断 Set 的终结符中是否含有ε。

参数：
        pSet -- Set 指针。

返回值：
        存在返回 1。
        不存在返回 0。
*/
int SetHasVoid(const Set *pSet) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminal[i], VoidSymbol) == 0) {
            return 1;
        }
    }
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
    Rule *pCopiedRule = CopyRule(pRuleHead);

    PrintRule(pCopiedRule);
    putchar('\n');
    // Rule *pCopiedRule = pRuleHead;
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

Rule *CopyRule(Rule *pHead) {
    if (pHead == NULL) {
        return NULL;
    }
    Rule *pCopiedHead = CreateRule(pHead->RuleName);
    if (pHead->pFirstSymbol != NULL) {
        for (RuleSymbol *pSelect = pHead->pFirstSymbol,
                        **pSelectPrePtr = &pCopiedHead->pFirstSymbol;
             pSelect != NULL; pSelect = pSelect->pOther,
                        pSelectPrePtr = &(*pSelectPrePtr)->pOther) {
            *pSelectPrePtr = CopySelect(pSelect);
        }
    }
    pCopiedHead->pNextRule = CopyRule(pHead->pNextRule);
    return pCopiedHead;
}

/*
功能：
        拷贝一个 Symbol。

参数：
        pSymbolTemplate -- 需要被拷贝的 Symbol 指针。

返回值：
        拷贝获得的新 Symbol 的指针。
*/
RuleSymbol *CopySymbol(const RuleSymbol *pSymbolTemplate) {
    if (pSymbolTemplate == NULL) {
        return NULL;
    }
    RuleSymbol *pTmp = CreateSymbol();
    memcpy(pTmp, pSymbolTemplate, sizeof(RuleSymbol));
    return pTmp;
}

/*
功能：
        拷贝一个 Select。

参数：
        pSelectTemplate -- 需要被拷贝的 Select 指针。

返回值：
        拷贝获得的新 Select 的指针。
*/
RuleSymbol *CopySelect(const RuleSymbol *pSelectTemplate) {
    if (pSelectTemplate == NULL) {
        return NULL;
    }
    RuleSymbol *pHead = CopySymbol(pSelectTemplate);
    pHead->pNextSymbol = CopySelect(pSelectTemplate->pNextSymbol);
    return pHead;
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

typedef struct _SYMBOL {
    int isToken;
    char Name[MAX_STR_LENGTH];
} SYMBOL;

typedef struct _RULE_ENTRY {
    char RuleName[MAX_STR_LENGTH];
    SYMBOL Selects[64][64];
} RULE_ENTRY;

/* exp -> exp addop term| term
   addop -> + | -
   term -> term mulop factor | factor
   mulop -> *
   factor -> (exp) | number */
static const RULE_ENTRY rule_table[] = {
    {"exp", {{{0, "exp"}, {0, "addop"}, {0, "term"}}, {{0, "term"}}}},
    {"addop", {{{1, "+"}}, {{1, "-"}}}},
    {"term", {{{0, "term"}, {0, "mulop"}, {0, "factor"}}, {{0, "factor"}}}},
    {"mulop", {{{1, "*"}}}},
    {"factor", {{{1, "("}, {0, "exp"}, {1, ")"}}, {{1, "number"}}}}};

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
        创建一个新的文法。

参数：
        pRuleName -- 文法的名字。

返回值：
        文法的指针。
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
        创建一个新的符号。

返回值：
        符号的指针。
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
        输出文法。

参数：
        pHead -- 文法的头指针。
*/
void PrintRule(const Rule *pHead) {
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

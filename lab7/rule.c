#include "rule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
功能：
    初始化文法链表。

返回值：
    文法的头指针。
*/
Rule *InitRules(const RULE_ENTRY *rule_table, int nRuleCount) {
    Rule *pHead, *pRule;
    RuleSymbol **pSymbolPtr1, **pSymbolPtr2;
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

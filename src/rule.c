#include "rule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
功能：
    初始化文法链表。

返回值：
    文法的头指针。

/**
 * @brief
 *
 * @param rule_table
 * @param nRuleCount
 * @return Rule*
 */
Rule *InitRules(const struct RULE_ENTRY *rule_table, int nRuleCount) {
    Rule *pRuleHead, *pRule;
    Symbol **pSymbolPtr1, **pSymbolPtr2;
    int i, j, k;
    Rule **pRulePtr = &pRuleHead;
    for (i = 0; i < nRuleCount; i++) {
        *pRulePtr = CreateRule(rule_table[i].RuleName);
        pRulePtr = &(*pRulePtr)->pNextRule;
    }
    pRule = pRuleHead;
    for (i = 0; i < nRuleCount; i++) {
        pSymbolPtr1 = &pRule->pFirstProduction;
        for (j = 0; rule_table[i].Productions[j][0].Name[0] != '\0'; j++) {
            pSymbolPtr2 = pSymbolPtr1;
            for (k = 0; rule_table[i].Productions[j][k].Name[0] != '\0'; k++) {
                const struct SYMBOL *pSymbol = &rule_table[i].Productions[j][k];
                *pSymbolPtr2 = CreateSymbol();
                (*pSymbolPtr2)->isToken = pSymbol->isToken;
                strcpy((*pSymbolPtr2)->SymbolName, pSymbol->Name);
                if (!pSymbol->isToken) {
                    (*pSymbolPtr2)->pRule = FindRule(pRuleHead, pSymbol->Name);
                    if (NULL == (*pSymbolPtr2)->pRule) {
                        printf("Init rules error, miss rule \"%s\"\n",
                               pSymbol->Name);
                        exit(1);
                    }
                }
                pSymbolPtr2 = &(*pSymbolPtr2)->pNextSymbol;
            }
            pSymbolPtr1 = &(*pSymbolPtr1)->pNextProduction;
        }
        pRule = pRule->pNextRule;
    }
    return pRuleHead;
}

/*
功能：
    创建一个新的 Rule。

参数：
    pRuleName -- 文法的名字。

返回值：
    Rule 指针

/**
 * @brief Create a Rule object
 *
 * @param pRuleName
 * @return Rule*
 */
Rule *CreateRule(const char *pRuleName) {
    Rule *pRule = (Rule *)malloc(sizeof(Rule));
    strcpy(pRule->RuleName, pRuleName);
    pRule->pFirstProduction = NULL;
    pRule->pNextRule = NULL;
    return pRule;
}

/*
功能：
    创建一个新的 Symbol。

返回值：
    Symbol 指针

/**
 * @brief Create a Symbol object
 *
 * @return Symbol*
 */
Symbol *CreateSymbol() {
    Symbol *pSymbol = (Symbol *)malloc(sizeof(Symbol));
    pSymbol->pNextSymbol = NULL;
    pSymbol->pNextProduction = NULL;
    pSymbol->isToken = -1;
    pSymbol->SymbolName[0] = '\0';
    pSymbol->pRule = NULL;
    return pSymbol;
}

/*
功能：
    根据 RuleName 在文法链表中查找名字相同的文法。

参数：
    pRuleHead -- 文法链表的头指针。
    RuleName -- 文法的名字。

返回值：
    如果存在名字相同的文法返回 Rule 指针，否则返回 NULL

/**
 * @brief
 *
 * @param pRuleHead
 * @param RuleName
 * @return Rule*
 */
Rule *FindRule(Rule *pRuleHead, const char *RuleName) {
    Rule *pRule;
    for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
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
    pRuleHead -- 文法链表的头指针。

/**
 * @brief
 *
 * @param pRuleHead
 */
void PrintRule(Rule *pRuleHead) {
    if (pRuleHead) {
        printf("%s->", pRuleHead->RuleName);
        for (Production *pProduction = pRuleHead->pFirstProduction;
             pProduction != NULL; pProduction = pProduction->pNextProduction) {
            for (Symbol *pSymble = pProduction; pSymble != NULL;
                 pSymble = pSymble->pNextSymbol) {
                printf("%s", pSymble->SymbolName);
            }
            putchar(pProduction->pNextProduction ? '|' : '\n');
        }
        PrintRule(pRuleHead->pNextRule);
    }
}

Rule *CopyRule(Rule *pRuleHead) {
    if (pRuleHead == NULL) {
        return NULL;
    }
    Rule *pCopiedHead = CreateRule(pRuleHead->RuleName);
    if (pRuleHead->pFirstProduction != NULL) {
        for (Production *
                 pProduction = pRuleHead->pFirstProduction,
                **pProductionPrePtr = &pCopiedHead->pFirstProduction;
             pProduction != NULL; pProduction = pProduction->pNextProduction,
                pProductionPrePtr = &(*pProductionPrePtr)->pNextProduction) {
            *pProductionPrePtr = CopyProduction(pProduction);
        }
    }
    pCopiedHead->pNextRule = CopyRule(pRuleHead->pNextRule);
    return pCopiedHead;
}

/*
功能：
        拷贝一个 Symbol。

参数：
        pSymbolTemplate -- 需要被拷贝的 Symbol 指针。

返回值：
        拷贝获得的新 Symbol 的指针。

/**
 * @brief
 *
 * @param pSymbolTemplate
 * @return Symbol*
 */
Symbol *CopySymbol(const Symbol *pSymbolTemplate) {
    if (pSymbolTemplate == NULL) {
        return NULL;
    }
    Symbol *pTmp = CreateSymbol();
    memcpy(pTmp, pSymbolTemplate, sizeof(Symbol));
    return pTmp;
}

/*
功能：
        拷贝一个 Production。

参数：
        pProductionTemplate -- 需要被拷贝的 Production 指针。

返回值：
        拷贝获得的新 Production 的指针。

/**
 * @brief
 *
 * @param pProductionTemplate
 * @return Symbol*
 */
Symbol *CopyProduction(const Production *pProductionTemplate) {
    if (pProductionTemplate == NULL) {
        return NULL;
    }
    Symbol *pRuleHead = CopySymbol(pProductionTemplate);
    pRuleHead->pNextSymbol = CopyProduction(pProductionTemplate->pNextSymbol);
    return pRuleHead;
}

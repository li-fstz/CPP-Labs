#include "rule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 初始化文法链表
 *
 * @param rule_table 原始文法实体的指针
 * @param nRuleCount 文法的数量
 * @return Rule* 文法链表的头指针
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

/**
 * @brief 创建一个新的文法并初始化
 *
 * @param pRuleName 文法的名字
 * @return Rule* 新文法的指针
 */
Rule *CreateRule(const char *pRuleName) {
    Rule *pRule = (Rule *)malloc(sizeof(Rule));
    strcpy(pRule->RuleName, pRuleName);
    pRule->pFirstProduction = NULL;
    pRule->pNextRule = NULL;
    return pRule;
}

/**
 * @brief 创建一个新符号并初始化
 *
 * @return 新符号的指针
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

/**
 * @brief 从文法链表中使用名字查找文法
 *
 * @param pRuleHead 文法链表的头指针
 * @param RuleName 文法的名字
 * @return Rule* 如果存在该文法则返回文法的指针，否则返回 NULL
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

/**
 * @brief 输出文法
 *
 * @param pRuleHead 文法链表的头指针
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

/**
 * @brief 复制整个文法链表
 * 
 * @param pRuleHead 文法链表的头指针
 * @return Rule* 新文法链表的头指针
 */
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

/**
 * @brief 复制一个符号
 *
 * @param pSymbolTemplate 被复制的符号的指针
 * @return Symbol* 新符号的指针
 */
Symbol *CopySymbol(const Symbol *pSymbolTemplate) {
    if (pSymbolTemplate == NULL) {
        return NULL;
    }
    Symbol *pTmp = CreateSymbol();
    memcpy(pTmp, pSymbolTemplate, sizeof(Symbol));
    return pTmp;
}

/**
 * @brief 复制一个产生式
 *
 * @param pProductionTemplate 被产生式的产生式指针
 * @return Symbol* 新产生式的指针
 */
Production *CopyProduction(const Production *pProductionTemplate) {
    if (pProductionTemplate == NULL) {
        return NULL;
    }
    Symbol *pRuleHead = CopySymbol(pProductionTemplate);
    pRuleHead->pNextSymbol = CopyProduction(pProductionTemplate->pNextSymbol);
    return pRuleHead;
}

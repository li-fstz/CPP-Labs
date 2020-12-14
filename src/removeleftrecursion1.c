#include "removeleftrecursion1.h"

/**
 * @brief 将一个符号添加到产生式末尾
 *
 * @param pProduction 产生式的指针
 * @param pNewSymbol 符号的指针
 */
void AddSymbolToProduction(Production *pProduction, Symbol *pNewSymbol) {
    while (pProduction->pNextSymbol) {
        pProduction = pProduction->pNextSymbol;
    }
    pProduction->pNextSymbol = pNewSymbol;
}

/**
 * @brief 将产生式加入到文法中的产生式链表中
 *
 * @param pRule 文法的指针
 * @param pNewProduction 产生式的指针
 */
void AddProductionToRule(Rule *pRule, Production *pNewProduction) {
    if (!pNewProduction) {
        pNewProduction = CreateSymbol();
        pNewProduction->isToken = 1;
        strcpy(pNewProduction->SymbolName, VOID_SYMBOL);
    }
    if (pRule->pFirstProduction) {
        Production *pTmp = pRule->pFirstProduction;
        while (pTmp->pNextProduction) {
            pTmp = pTmp->pNextProduction;
        }
        pTmp->pNextProduction = pNewProduction;
    } else {
        pRule->pFirstProduction = pNewProduction;
    }
}

/**
 * @brief 消除左递归
 *
 * @param pRuleHead 文法链表的头指针
 */
void RemoveLeftRecursion(Rule *pRuleHead) {
    Rule *pNewRule;
    Production *pProduction = pRuleHead->pFirstProduction;
    Production **pProductionPrePtr = &pRuleHead->pFirstProduction;

    /**
     * @brief 创建一条新文法
     */
    pNewRule = CreateRule(pRuleHead->RuleName);
    strcat(pNewRule->RuleName, POSTFIX);

    while (pProduction != NULL) {

        /**
         * @brief 创建新文法的符号
         */
        Symbol *pTmp = CreateSymbol();
        pTmp->isToken = 0;
        pTmp->pRule = pNewRule;
        strcpy(pTmp->SymbolName, pNewRule->RuleName);

        /**
         * @brief 如果当前产生式包含左递归，则移除此产生式，
         * 然后将此产生式的左递归转换为右递归，然后将其加入到新文法中；
         * 如果当前产生式不包含左递归，则在产生式尾部添加新文法的符号。
         */
        if (0 == pProduction->isToken && pProduction->pRule == pRuleHead) {
            *pProductionPrePtr = (*pProductionPrePtr)->pNextProduction;
            pProduction = pProduction->pNextSymbol;
            AddSymbolToProduction(pProduction, pTmp);
            AddProductionToRule(pNewRule, pProduction);
            pProduction = *pProductionPrePtr;
        } else {
            AddSymbolToProduction(pProduction, pTmp);
            pProductionPrePtr = &(*pProductionPrePtr)->pNextProduction;
            pProduction = pProduction->pNextProduction;
        }
    }
    /**
     * @brief 向新文法添加 ε 产生式，然后将新文法加入到文法链表中
     */
    AddProductionToRule(pNewRule, NULL);
    while (pRuleHead->pNextRule) {
        pRuleHead = pRuleHead->pNextRule;
    }
    pRuleHead->pNextRule = pNewRule;
    return;
}

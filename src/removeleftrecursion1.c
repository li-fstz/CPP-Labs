#include "removeleftrecursion1.h"

/**
 * @brief 将一个符号添加到产生式末尾
 *
 * @param production 产生式的指针
 * @param newSymbol 符号的指针
 */
void AddSymbolToProduction(Production *production, Symbol *newSymbol) {
    Append(SYMBOLHEAD(production), newSymbol);
}

/**
 * @brief 将产生式加入到文法中的产生式链表中
 *
 * @param rule 文法的指针
 * @param newProduction 产生式的指针
 */
void AddProductionToRule(Rule *rule, Production *newProduction) {
    if (!newProduction) {
        newProduction = CreateSymbol();
        newProduction->isToken = 1;
        strcpy(newProduction->SymbolName, VOID_SYMBOL);
    }
    if (PRODUCTIONHEAD(rule)) {
        Production *pTmp = PRODUCTIONHEAD(rule);
        while (pTmp->next) {
            pTmp = pTmp->next;
        }
        pTmp->next = newProduction;
    } else {
        PRODUCTIONHEAD(rule) = newProduction;
    }
}

/**
 * @brief 消除左递归
 *
 * @param ruleHead 文法链表的头指针
 */
void RemoveLeftRecursion(Rule *ruleHead) {
    Rule *newRule;
    Production *production = PRODUCTIONHEAD(ruleHead);
    Production **pProductionPrePtr = &PRODUCTIONHEAD(ruleHead);

    /**
     * @brief 创建一条新文法
     */
    newRule = NewRule(RULENAME(ruleHead));
    strcat(newRule->ruleName, POSTFIX);

    while (production != NULL) {

        /**
         * @brief 创建新文法的符号
         */
        Symbol *pTmp = CreateSymbol();
        pTmp->isToken = 0;
        pTmp->rule = newRule;
        strcpy(pTmp->SymbolName, newRule->ruleName);

        /**
         * @brief 如果当前产生式包含左递归，则移除此产生式，
         * 然后将此产生式的左递归转换为右递归，然后将其加入到新文法中；
         * 如果当前产生式不包含左递归，则在产生式尾部添加新文法的符号。
         */
        if (!ISTOKEN(production) && production->rule == ruleHead) {
            *pProductionPrePtr = (*pProductionPrePtr)->next;
            production = production->next;
            AddSymbolToProduction(production, pTmp);
            AddProductionToRule(newRule, production);
            production = *pProductionPrePtr;
        } else {
            AddSymbolToProduction(production, pTmp);
            pProductionPrePtr = &(*pProductionPrePtr)->next;
            production = production->next;
        }
    }
    /**
     * @brief 向新文法添加 ε 产生式，然后将新文法加入到文法链表中
     */
    AddProductionToRule(newRule, NULL);
    while (ruleHead->next) {
        ruleHead = ruleHead->next;
    }
    ruleHead->next = newRule;
    return;
}

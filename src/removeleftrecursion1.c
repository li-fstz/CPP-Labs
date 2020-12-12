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
    Production *pProduction;                    // Production 游标
    Rule *pNewRule;                             // Rule 指针
    pNewRule = CreateRule(pRuleHead->RuleName); // 创建新 Rule
    strcat(pNewRule->RuleName, POSTFIX);
    pProduction = pRuleHead->pFirstProduction; // 初始化 Production 游标
    Production **pProductionPrePtr = &pRuleHead->pFirstProduction;
    while (pProduction != NULL) // 循环处理所有的 Production
    {
        Symbol *pTmp = CreateSymbol();
        pTmp->isToken = 0;
        pTmp->pRule = pNewRule;
        strcpy(pTmp->SymbolName, pNewRule->RuleName);
        if (0 == pProduction->isToken &&
            pProduction->pRule == pRuleHead) // Production 存在左递归
        {
            // 移除包含左递归的 Production，将其转换为右递归后添加到新 Rule
            // 的末尾，并移动游标
            *pProductionPrePtr = (*pProductionPrePtr)->pNextProduction;
            pProduction = pProduction->pNextSymbol;
            AddSymbolToProduction(pProduction, pTmp);
            AddProductionToRule(pNewRule, pProduction);
            pProduction = *pProductionPrePtr;
        } else // Production 不存在左递归
        {
            // 在没有左递归的 Production 末尾添加指向新 Rule
            // 的非终结符，并移动游标
            AddSymbolToProduction(pProduction, pTmp);
            pProductionPrePtr = &(*pProductionPrePtr)->pNextProduction;
            pProduction = pProduction->pNextProduction;
        }
    }
    // 在新 Rule 的最后加入ε(用 '$' 代替)
    // 将新 Rule 插入文法链表
    AddProductionToRule(pNewRule, NULL);
    while (pRuleHead->pNextRule) {
        pRuleHead = pRuleHead->pNextRule;
    }
    pRuleHead->pNextRule = pNewRule;
    return;
}

#include "removeleftrecursion1.h"

/*
功能：
        将一个 Symbol 添加到 Production 的末尾。

参数：
        pProduction -- Production 指针。
        pNewSymbol -- Symbol 指针。

/**
 * @brief
 *
 * @param pProduction
 * @param pNewSymbol
 */
void AddSymbolToProduction(Production *pProduction, Symbol *pNewSymbol) {
    while (pProduction->pNextSymbol) {
        pProduction = pProduction->pNextSymbol;
    }
    pProduction->pNextSymbol = pNewSymbol;
}

/*
功能：
        将一个 Production 加入到文法末尾。当 Production 为 NULL
时就将一个ε终结符加入到文法末尾。

参数：
        pRule -- 文法指针。
        pNewProduction -- Production 指针。

/**
 * @brief
 *
 * @param pRule
 * @param pNewProduction
 */
void AddProductionToRule(Rule *pRule, Symbol *pNewProduction) {
    if (!pNewProduction) {
        pNewProduction = CreateSymbol();
        pNewProduction->isToken = 1;
        strcpy(pNewProduction->SymbolName, VOID_SYMBOL);
    }
    if (pRule->pFirstProduction) {
        Symbol *pTmp = pRule->pFirstProduction;
        while (pTmp->pNextProduction) {
            pTmp = pTmp->pNextProduction;
        }
        pTmp->pNextProduction = pNewProduction;
    } else {
        pRule->pFirstProduction = pNewProduction;
    }
}

/*
功能：
        消除左递归。

参数：
        pRuleHead -- 文法链表的头指针。

/**
 * @brief
 *
 * @param pRuleHead
 */
void RemoveLeftRecursion(Rule *pRuleHead) {
    Production *pProduction;                    // Production 游标
    Rule *pNewRule;                             // Rule 指针
    pNewRule = CreateRule(pRuleHead->RuleName); // 创建新 Rule
    strcat(pNewRule->RuleName, POSTFIX);
    pProduction = pRuleHead->pFirstProduction; // 初始化 Production 游标
    Symbol **pProductionPrePtr = &pRuleHead->pFirstProduction;
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

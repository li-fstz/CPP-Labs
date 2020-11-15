#include "removeleftrecursion.h"

/*
功能：
        将一个 Symbol 添加到 Select 的末尾。

参数：
        pSelect -- Select 指针。
        pNewSymbol -- Symbol 指针。
*/
void AddSymbolToSelect(RuleSymbol *pSelect, RuleSymbol *pNewSymbol) {
    while (pSelect->pNextSymbol) {
        pSelect = pSelect->pNextSymbol;
    }
    pSelect->pNextSymbol = pNewSymbol;
}

/*
功能：
        将一个 Select 加入到文法末尾。当 Select 为 NULL
时就将一个ε终结符加入到文法末尾。

参数：
        pRule -- 文法指针。
        pNewSelect -- Select 指针。
*/
void AddSelectToRule(Rule *pRule, RuleSymbol *pNewSelect) {
    if (!pNewSelect) {
        pNewSelect = CreateSymbol();
        pNewSelect->isToken = 1;
        strcpy(pNewSelect->SymbolName, VoidSymbol);
    }
    if (pRule->pFirstSymbol) {
        RuleSymbol *pTmp = pRule->pFirstSymbol;
        while (pTmp->pOther) {
            pTmp = pTmp->pOther;
        }
        pTmp->pOther = pNewSelect;
    } else {
        pRule->pFirstSymbol = pNewSelect;
    }
}

/*
功能：
        消除左递归。

参数：
        pHead -- 文法链表的头指针。
*/
void RemoveLeftRecursion(Rule *pHead) {
    RuleSymbol *pSelect;  // Select 游标
    Rule *pNewRule;       // Rule 指针

    pNewRule = CreateRule(pHead->RuleName);  // 创建新 Rule
    strcat(pNewRule->RuleName, Postfix);

    pSelect = pHead->pFirstSymbol;  // 初始化 Select 游标
    RuleSymbol **pSelectPrePtr = &pHead->pFirstSymbol;
    while (pSelect != NULL)  // 循环处理所有的 Select
    {
        RuleSymbol *pTmp = CreateSymbol();
        pTmp->isToken = 0;
        pTmp->pRule = pNewRule;
        strcpy(pTmp->SymbolName, pNewRule->RuleName);
        if (0 == pSelect->isToken &&
            pSelect->pRule == pHead)  // Select 存在左递归
        {
            // 移除包含左递归的 Select，将其转换为右递归后添加到新 Rule
            // 的末尾，并移动游标
            *pSelectPrePtr = (*pSelectPrePtr)->pOther;
            pSelect = pSelect->pNextSymbol;
            AddSymbolToSelect(pSelect, pTmp);
            AddSelectToRule(pNewRule, pSelect);
            pSelect = *pSelectPrePtr;
        } else  // Select 不存在左递归
        {
            // 在没有左递归的 Select 末尾添加指向新 Rule 的非终结符，并移动游标
            AddSymbolToSelect(pSelect, pTmp);
            pSelectPrePtr = &(*pSelectPrePtr)->pOther;
            pSelect = pSelect->pOther;
        }
    }

    // 在新 Rule 的最后加入ε(用 '$' 代替)
    // 将新 Rule 插入文法链表
    AddSelectToRule(pNewRule, NULL);
    while (pHead->pNextRule) {
        pHead = pHead->pNextRule;
    }
    pHead->pNextRule = pNewRule;
    return;
}

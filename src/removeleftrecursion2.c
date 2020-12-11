#include "removeleftrecursion2.h"
/*
功能：
        判断当前 Rule 中的一个 Symbol 是否需要被替换。
        如果 Symbol 是一个非终结符，且 Symbol 对应的
        Rule 在当前 Rule 之前，就需要被替换。

参数：
        pCurRule -- 当前 Rule 的指针。
        pSymbol -- Symbol 指针。

返回值：
        需要替换返回 1。
        不需要替换返回 0。

/**
 * @brief
 *
 * @param pCurRule
 * @param pSymbol
 * @return int
 */
int SymbolNeedReplace(const Rule *pCurRule, const Symbol *pSymbol) {
    if (pSymbol->isToken) {
        return 0;
    } else {
        Rule *pTmp = pSymbol->pRule->pNextRule;
        while (pTmp) {
            if (pCurRule == pTmp) {
                return 1;
            } else {
                pTmp = pTmp->pNextRule;
            }
        }
        return 0;
    }
}

/*
功能：
        替换一个 Production 的第一个 Symbol。

参数：
        pProductionTemplate -- 需要被替换的 Production 指针。

返回值：
        替换后获得的新 Production 的指针。
        注意，替换后可能会有一个新的 Production，
        也可能会有多个 Production 链接在一起。

/**
 * @brief
 *
 * @param pProductionTemplate
 * @return Symbol*
 */
Symbol *ReplaceProduction(const Production *pProductionTemplate) {
    Production *pProductionsOfFirstSymble =
                   CopyProduction(pProductionTemplate->pRule->pFirstProduction),
               *pTmpProduction = pProductionsOfFirstSymble;
    while (pTmpProduction) {
        Symbol *pTmpSymble = pTmpProduction;
        while (pTmpSymble->pNextSymbol) {
            pTmpSymble = pTmpSymble->pNextSymbol;
        }
        pTmpSymble->pNextSymbol =
            CopyProduction(pProductionTemplate->pNextSymbol);
        if (pTmpProduction->pNextProduction) {
            pTmpProduction->pNextProduction =
                CopyProduction(pTmpProduction->pNextProduction);
        }
        pTmpProduction = pTmpProduction->pNextProduction;
    }
    return pProductionsOfFirstSymble;
}

/*
功能：
        释放一个 Production 的内存。

参数：
        pProduction -- 需要释放的 Production 的指针。

/**
 * @brief
 *
 * @param pProduction
 */
void FreeProduction(Production *pProduction) {
    if (pProduction->pNextSymbol) {
        FreeProduction(pProduction->pNextSymbol);
    } else {
        free(pProduction);
    }
}

/*
功能：
        判断一条 Rule 是否存在左递归。

参数：
        prRule -- Rule 指针。

返回值：
        存在返回 1。
        不存在返回 0。

/**
 * @brief
 *
 * @param pProduction
 * @param pRule
 * @return int
 */
int ProductionHasLeftRecursion(Production *pProduction, Rule *pRule) {
    if (pProduction->isToken) {
        return 0;
    } else {
        return pProduction->pRule == pRule;
    }
}
int RuleHasLeftRecursion(Rule *pRule) {
    Symbol *pTmp = pRule->pFirstProduction;
    while (pTmp) {
        if (ProductionHasLeftRecursion(pTmp, pRule)) {
            return 1;
        }
        pTmp = pTmp->pNextProduction;
    }
    return 0;
}

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
        将一个 Production 加入到文法末尾，当 Production 为 NULL
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
    Rule *pRule;                // Rule 游标
    Production *pProduction;    // Production 游标
    Rule *pNewRule;             // Rule 指针
    int isChange;               // Rule 是否被替换的标记
    Symbol **pProductionPrePtr; // Symbol 指针的指针

    for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
        //
        // 替换
        //
        do {
            isChange = 0;
            // 在 Rule 的所有 Production 中查找是否需要替换
            for (pProduction = pRule->pFirstProduction,
                pProductionPrePtr = &pRule->pFirstProduction;
                 pProduction != NULL;
                 pProductionPrePtr = &pProduction->pNextProduction,
                pProduction = pProduction->pNextProduction) {
                if (SymbolNeedReplace(pRule,
                                      pProduction)) // 判断 Production 的第一个
                                                    // Symbol 是否需要替换
                {
                    isChange = 1;
                    // 调用 ReplaceProduction 函数，替换 Production 的第一个
                    // Symbol 后得到新的 Productions
                    Symbol *pNewProductions = ReplaceProduction(pProduction);
                    // 使用新的 Productions 替换原有的 Production，并调用
                    // FreeProduction 函数释放原有的 Production 内存
                    *pProductionPrePtr = pNewProductions;
                    while (pNewProductions->pNextProduction) {
                        pNewProductions = pNewProductions->pNextProduction;
                    }
                    pNewProductions->pNextProduction =
                        pProduction->pNextProduction;
                    FreeProduction(pProduction);
                    break;
                }

                if (isChange)
                    break;
            }
        } while (isChange);
        // 忽略没有左递归的 Rule;
        if (!RuleHasLeftRecursion(pRule))
            continue;
        //
        // 消除左递归
        //
        pNewRule = CreateRule(pRule->RuleName); // 创建新 Rule
        strcat(pNewRule->RuleName, POSTFIX);
        pProduction = pRule->pFirstProduction; // 初始化 Production 游标
        pProductionPrePtr = &pRule->pFirstProduction;
        while (pProduction != NULL) // 循环处理所有的 Production
        {
            Symbol *pTmp = CreateSymbol();
            pTmp->isToken = 0;
            pTmp->pRule = pNewRule;
            strcpy(pTmp->SymbolName, pNewRule->RuleName);
            if (0 == pProduction->isToken &&
                pProduction->pRule == pRule) // Production 存在左递归
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
        pNewRule->pNextRule = pRule->pNextRule;
        pRule->pNextRule = pNewRule;
        pRule = pNewRule;
    }
}

#include "removeleftrecursion2.h"

/**
 * @brief 判断当前文法中的符号是否需要被替换，
 *        如果这个符号是非终结符且指向的文法在当前文法之前，
 *        那么该终结符应该被替换
 *
 * @param pCurRule 文法的指针
 * @param pSymbol 符号的指针
 * @return int 是否需要被替换
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

/**
 * @brief 替换产生式的第一个符号
 *
 * @param pProductionTemplate 需要被替换的产生式的指针
 * @return Symbol* 替换后的新产生式的指针，
 *                 注意第一个符号指向的产生式可能是一个链表，
 *                 所以返回的新产生是也可能是一个链表 
 */
Production *ReplaceProduction(const Production *pProductionTemplate) {
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

/**
 * @brief 释放一个产生式链表的内存
 *
 * @param pProduction 产生式链表的头指针
 */
void FreeProduction(Production *pProduction) {
    if (pProduction->pNextSymbol) {
        FreeProduction(pProduction->pNextSymbol);
    } else {
        free(pProduction);
    }
}

/**
 * @brief 判断该产生式是否存在左递归
 *
 * @param pProduction 产生式的指针
 * @param pRule 指向该产生式的文法的指针
 * @return int 是否存在左递归
 */
int ProductionHasLeftRecursion(Production *pProduction, Rule *pRule) {
    if (pProduction->isToken) {
        return 0;
    } else {
        return pProduction->pRule == pRule;
    }
}

/**
 * @brief 判断该文法是否存在左递归
 * 
 * @param pRule 文法的指针
 * @return int  是否存在左递归
 */
int RuleHasLeftRecursion(Rule *pRule) {
    Production *pTmp = pRule->pFirstProduction;
    while (pTmp) {
        if (ProductionHasLeftRecursion(pTmp, pRule)) {
            return 1;
        }
        pTmp = pTmp->pNextProduction;
    }
    return 0;
}

/**
 * @brief 将一个符号添加到产生式尾部
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
 * @brief 讲一个产生式添加到文法指向的产生式链表尾部，
 *        当产生式指针为 NULL 时则添加 ε
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
    Rule *pRule;                // Rule 游标
    Production *pProduction;    // Production 游标
    Rule *pNewRule;             // Rule 指针
    int isChange;               // Rule 是否被替换的标记
    Production **pProductionPrePtr; // Symbol 指针的指针

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
                    Production *pNewProductions = ReplaceProduction(pProduction);
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

#include "pickupleftfactor.h"

#include <stdio.h>
#include <string.h>

#include "rule.h"

/**
 * @brief 提取左因子
 *
 * @param pRuleHead 文法链表的头指针
 */
void PickupLeftFactor(Rule *pRuleHead) {
    Rule *pRule;                     // Rule 游标
    int isChange;                    // Rule 是否被提取左因子的标志
    Production *pProductionTemplate; // Production 游标
    Rule *pNewRule;                  // Rule 指针
    Production *pProduction;         // Production 游标
    do {
        isChange = 0;
        for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {
            // 取 Rule 中的一个 Production 作为模板，调用 LeftFactorMaxLength
            // 函数确定左因子的最大长度
            int Count = 0;
            for (pProductionTemplate = pRule->pFirstProduction;
                 pProductionTemplate != NULL;
                 pProductionTemplate = pProductionTemplate->pNextProduction) {
                if ((Count = LeftFactorMaxLength(pProductionTemplate)) > 0)
                    break;
            }
            // 忽略没用左因子的 Rule
            if (Count == 0)
                continue;
            pNewRule = CreateRule(pRule->RuleName); // 创建新 Rule
            GetUniqueRuleName(pRule, pNewRule->RuleName);
            isChange = 1; // 设置标志
            // 调用 AddProductionToRule 函数把模板左因子之后的部分加到新 Rule
            // 的末尾 将模板左因子之后的部分替换为指向新 Rule 的非终结符
            AddProductionToRule(pNewRule,
                                GetSymbol(pProductionTemplate, Count));
            Symbol *pTmp = CreateSymbol();
            pTmp->isToken = 0;
            pTmp->pRule = pNewRule;
            GetSymbol(pProductionTemplate, Count - 1)->pNextSymbol = pTmp;
            // 从模板之后的位置循环查找包含左因子的 Production，并提取左因子
            pProduction = pProductionTemplate->pNextProduction;
            Production **pProductionPtr = &pProductionTemplate->pNextProduction;
            while (pProduction != NULL) {
                if (NeedPickup(pProductionTemplate, Count,
                               pProduction)) // Production 包含左因子
                {
                    // 调用 AddProductionToRule 函数把左因子之后的部分加到新
                    // Rule 的末尾 将该 Production 从 Rule
                    // 中移除，释放内存，并移动游标
                    AddProductionToRule(pNewRule,
                                        GetSymbol(pProduction, Count));
                    *pProductionPtr = pProduction->pNextProduction;
                    GetSymbol(pProduction, Count - 1)->pNextSymbol = NULL;
                    FreeProduction(pProduction);
                    pProduction = *pProductionPtr;
                } else // Production 不包含左因子
                {
                    // 移动游标
                    pProductionPtr = &pProduction->pNextProduction;
                    pProduction = pProduction->pNextProduction;
                }
            }
            // 将新 Rule 加入到文法链表
            while (pRule->pNextRule) {
                pRule = pRule->pNextRule;
            }
            pRule->pNextRule = pNewRule;
        }
    } while (isChange == 1);
}

/**
 * @brief 根据下标找到 Production 中的 Symbol
 *
 * @param pProduction Production 的指针
 * @param index 下标
 * @return Symbol* 如果找到则返回 Symbol 的指针，否则返回 NULL
 */
Symbol *GetSymbol(Production *pProduction, int index) {
    int i = 0;
    Symbol *pSymbol;
    for (pSymbol = pProduction, i = 0; pSymbol != NULL;
         pSymbol = pSymbol->pNextSymbol, i++) {
        if (i == index) {
            return pSymbol;
        }
    }
    return NULL;
}

/**
 * @brief 计算该条产生式中左因子的最大长度
 *
 * @param pProduction 产生式指针
 * @return int 最大长度
 */
int LeftFactorMaxLength(Production *pProduction) {
    int maxLength = 0;
    Production *pCmpProduction = pProduction->pNextProduction;
    while (pCmpProduction) {
        int length = 0;
        for (int i = 0;; i++) {
            Symbol *pA = GetSymbol(pProduction, i),
                   *pB = GetSymbol(pCmpProduction, i);
            if (!pA || !pB || !SymbolCmp(pA, pB)) {
                length = i;
                break;
            }
        }
        maxLength = length > maxLength ? length : maxLength;
        pCmpProduction = pCmpProduction->pNextProduction;
    }
    return maxLength;
}

/**
 * @brief 比较 Symbol 是否相等
 *
 * @param pSymbol1 Symbol1 的指针
 * @param pSymbol2 Symbol2 的指针
 * @return int 是否相等
 */
int SymbolCmp(Symbol *pSymbol1, Symbol *pSymbol2) {
    if (pSymbol1->isToken == pSymbol2->isToken) {
        if (pSymbol1->isToken) {
            return strcmp(pSymbol1->SymbolName, pSymbol2->SymbolName) == 0;
        } else {
            return pSymbol1->pRule == pSymbol2->pRule;
        }
    } else {
        return 0;
    }
}

/**
 * @brief 判断产生式是否需要被替换
 *
 * @param pProductionTemplate 模板产生式的指针
 * @param Count 指定要比较的符号个数
 * @param pProduction 产生式的指针
 * @return int
 */
int NeedPickup(Production *pProductionTemplate, int Count,
               Production *pProduction) {
    for (int i = 0; i < Count; i++) {
        Symbol *pA = GetSymbol(pProductionTemplate, i),
               *pB = GetSymbol(pProduction, i);
        if (!pA || !pB || !SymbolCmp(pA, pB)) {
            return 0;
        }
    }
    return 1;
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
 * @brief 得到一个不重复的新文法名，如果存在冲突则添加后缀
 *
 * @param pRuleHead 文法链表的头指针
 * @param pRuleName 像取得新文法名
 */
void GetUniqueRuleName(Rule *pRuleHead, char *pRuleName) {
    Rule *pRuleCursor = pRuleHead;
    for (; pRuleCursor != NULL;) {
        if (0 == strcmp(pRuleCursor->RuleName, pRuleName)) {
            strcat(pRuleName, POSTFIX);
            pRuleCursor = pRuleHead;
            continue;
        }
        pRuleCursor = pRuleCursor->pNextRule;
    }
}

/**
 * @brief 释放整个产生式链表的内存
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

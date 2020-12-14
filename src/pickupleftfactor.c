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
    Rule *pRule, *pNewRule;
    Production *pProduction, *pProductionTemplate;

    /**
     * @brief 对文法是否修改的标志
     */
    int isChange;

    do {
        isChange = 0;
        for (pRule = pRuleHead; pRule != NULL; pRule = pRule->pNextRule) {

            /**
             * @brief 从当前文法指向的产生式链表中以一条产生式为模板
             * 求出后续产生式的最长的左因子长度
             */
            int Count = 0;
            for (pProductionTemplate = pRule->pFirstProduction;
                 pProductionTemplate != NULL;
                 pProductionTemplate = pProductionTemplate->pNextProduction) {
                if ((Count = LeftFactorMaxLength(pProductionTemplate)) > 0)
                    break;
            }

            /**
             * @brief 如果当前文法没有左因子则继续判断下一条文法
             */
            if (Count == 0) {
                continue;
            }

            /**
             * @brief 创建一条以当前文法为名称模板的新文法
             */
            pNewRule = CreateRule(pRule->RuleName);
            GetUniqueRuleName(pRule, pNewRule->RuleName);

            isChange = 1;

            /**
             * @brief 把模板产生式左因子之后的产生式加入到新文法中
             * 然后将其替换为新文法的符号
             */
            AddProductionToRule(pNewRule,
                                GetSymbol(pProductionTemplate, Count));
            Symbol *pTmp = CreateSymbol();
            pTmp->isToken = 0;
            pTmp->pRule = pNewRule;
            strcpy(pTmp->SymbolName, pNewRule->RuleName);
            GetSymbol(pProductionTemplate, Count - 1)->pNextSymbol = pTmp;

            /**
             * @brief 在模板产生式之后查找包含左因子的产生式，然后提取左因子
             */
            pProduction = pProductionTemplate->pNextProduction;
            Production **pProductionPtr = &pProductionTemplate->pNextProduction;
            while (pProduction != NULL) {

                /**
                 * @brief 如果产生式中包含左因子，则将左因子后的产生式
                 * 加入到新文法中，并将该产生式从文法中移除
                 */
                if (NeedPickup(pProductionTemplate, Count, pProduction)) {
                    AddProductionToRule(pNewRule,
                                        GetSymbol(pProduction, Count));
                    *pProductionPtr = pProduction->pNextProduction;
                    GetSymbol(pProduction, Count - 1)->pNextSymbol = NULL;
                    FreeProduction(pProduction);
                    pProduction = *pProductionPtr;
                } else {
                    pProductionPtr = &pProduction->pNextProduction;
                    pProduction = pProduction->pNextProduction;
                }
            }

            /**
             * @brief 将新文法加入到文法链表中
             */
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
    for (Production *pCmpProduction = pProduction->pNextProduction;
         pCmpProduction != NULL;
         pCmpProduction = pCmpProduction->pNextProduction) {
        int length = 0;
        for (Symbol *pA = pProduction, *pB = pCmpProduction;
             !pA || !pB || !SymbolCmp(pA, pB);
             length++, pA = pA->pNextSymbol, pB = pB->pNextSymbol) {
        }
        maxLength = length > maxLength ? length : maxLength;
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
int SymbolCmp(const Symbol *pSymbol1, const Symbol *pSymbol2) {
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
int NeedPickup(const Production *pProductionTemplate, int Count,
               const Production *pProduction) {
    Symbol *pA = pProductionTemplate, *pB = pProduction;
    for (int i = 0; i < Count;
         i++, pA = pA->pNextSymbol, pB = pB->pNextSymbol) {
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
 * @param pProduction 产生式的指针
 */
void AddProductionToRule(Rule *pRule, Production *pProduction) {
    if (!pProduction) {
        pProduction = CreateSymbol();
        pProduction->isToken = 1;
        strcpy(pProduction->SymbolName, VOID_SYMBOL);
    }
    if (pRule->pFirstProduction) {
        Production *pTmp = pRule->pFirstProduction;
        while (pTmp->pNextProduction) {
            pTmp = pTmp->pNextProduction;
        }
        pTmp->pNextProduction = pProduction;
    } else {
        pRule->pFirstProduction = pProduction;
    }
}

/**
 * @brief 得到一个不重复的新文法名，如果存在冲突则添加后缀
 *
 * @param pRuleHead 文法链表的头指针
 * @param pRuleName 像取得新文法名
 */
void GetUniqueRuleName(const Rule *pRuleHead, char *pRuleName) {
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

#include "removeleftrecursion2.h"

/**
 * @brief 判断当前文法中的符号是否需要被替换，
 *        如果这个符号是非终结符且指向的文法在当前文法之前，
 *        那么该终结符应该被替换
 *
 * @param pCurRule 文法的指针
 * @param symbol 符号的指针
 * @return int 是否需要被替换
 */
int SymbolNeedReplace(const Rule *pCurRule, const Symbol *symbol) {
    if (ISTOKEN(symbol)) {
        return 0;
    } else {
        Rule *pTmp = symbol->rule->next;
        while (pTmp) {
            if (pCurRule == pTmp) {
                return 1;
            } else {
                pTmp = pTmp->next;
            }
        }
        return 0;
    }
}

/**
 * @brief 替换产生式的第一个符号
 *
 * @param productionTemplate 需要被替换的产生式的指针
 * @return Symbol* 替换后的新产生式的指针，
 *                 注意第一个符号指向的产生式可能是一个链表，
 *                 所以返回的新产生是也可能是一个链表
 */
Production *ReplaceProduction(const Production *productionTemplate) {
    Production *pProductionsOfFirstSymble =
                   CopyProduction(productionTemplate->PRODUCTIONHEAD(rule)),
               *pTmpProduction = pProductionsOfFirstSymble;
    while (pTmpProduction) {
        Symbol *pTmpSymble = pTmpProduction;
        while (pTmpSymble->next) {
            pTmpSymble = pTmpSymble->next;
        }
        pTmpSymble->next =
            CopyProduction(productionTemplate->next);
        if (pTmpProduction->next) {
            pTmpProduction->next =
                CopyProduction(pTmpProduction->next);
        }
        pTmpProduction = pTmpProduction->next;
    }
    return pProductionsOfFirstSymble;
}

/**
 * @brief 释放一个产生式链表的内存
 *
 * @param production 产生式链表的头指针
 */
void FreeProduction(Production *production) {
    if (production->next) {
        FreeProduction(production->next);
    } else {
        free(production);
    }
}

/**
 * @brief 判断该产生式是否存在左递归
 *
 * @param production 产生式的指针
 * @param rule 指向该产生式的文法的指针
 * @return int 是否存在左递归
 */
int ProductionHasLeftRecursion(Production *production, Rule *rule) {
    if (production->isToken) {
        return 0;
    } else {
        return production->rule == rule;
    }
}

/**
 * @brief 判断该文法是否存在左递归
 *
 * @param rule 文法的指针
 * @return int  是否存在左递归
 */
int RuleHasLeftRecursion(const Rule *rule) {
    Production *pTmp = PRODUCTIONHEAD(rule);
    while (pTmp) {
        if (ProductionHasLeftRecursion(pTmp, rule)) {
            return 1;
        }
        pTmp = pTmp->next;
    }
    return 0;
}

/**
 * @brief 将一个符号添加到产生式尾部
 *
 * @param production 产生式的指针
 * @param newSymbol 符号的指针
 */
void AddSymbolToProduction(Production *production, const Symbol *newSymbol) {
    while (production->next) {
        production = production->next;
    }
    production->next = newSymbol;
}

/**
 * @brief 讲一个产生式添加到文法指向的产生式链表尾部，
 *        当产生式指针为 NULL 时则添加 ε
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
    Rule *rule, *newRule;
    Production *production;
    Production **pProductionPrePtr;

    /**
     * @brief 标记文法是否被修改
     */
    int isChange;

    for (rule = ruleHead; rule != NULL; rule = rule->next) {
        do {
            isChange = 0;

            /**
             * @brief 对当前文法对应的所有产生式进行替换
             */
            for (production = PRODUCTIONHEAD(rule),
                pProductionPrePtr = &PRODUCTIONHEAD(rule);
                 production != NULL;
                 pProductionPrePtr = &production->next,
                production = production->next) {
                /**
                 * @brief 判断当前产生式的第一个符号是否需要替换
                 * 如果这个符号对应的文法在当前文法的前面，则需要被替换
                 */
                if (SymbolNeedReplace(rule, production)) {
                    isChange = 1;
                    Production *pNewProductions =
                        ReplaceProduction(production);

                    /**
                     * @brief 用替换后的产生式链表替换之前的产生式，
                     * 最后释放之前产生式的内存
                     */
                    *pProductionPrePtr = pNewProductions;
                    while (pNewProductions->next) {
                        pNewProductions = pNewProductions->next;
                    }
                    pNewProductions->next =
                        production->next;
                    FreeProduction(production);
                    break;
                }
                if (isChange) {
                    break;
                }
            }
        } while (isChange);

        /**
         * @brief 忽略不包含左递归的文法
         */
        if (!RuleHasLeftRecursion(rule)) {
            continue;
        }

        /**
         * @brief 创建新文法
         */
        newRule = NewRule(RULENAME(rule)); // 创建新 Rule
        strcat(newRule->ruleName, POSTFIX);

        production = PRODUCTIONHEAD(rule);
        pProductionPrePtr = &PRODUCTIONHEAD(rule);
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
            if (0 == production->isToken && production->rule == rule) {
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
        newRule->next = rule->next;
        rule->next = newRule;
        rule = newRule;
    }
}

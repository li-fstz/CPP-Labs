#include "pickupleftfactor.h"

#include <stdio.h>
#include <string.h>

#include "rule.h"

/**
 * @brief 提取左因子
 *
 * @param ruleHead 文法链表的头指针
 */
void PickupLeftFactor(Rule *ruleHead) {
    Rule *rule, *newRule;
    Production *production, *productionTemplate;

    /**
     * @brief 对文法是否修改的标志
     */
    int isChange;

    do {
        isChange = 0;
        for (rule = ruleHead; rule != NULL; rule = rule->next) {

            /**
             * @brief 从当前文法指向的产生式链表中以一条产生式为模板
             * 求出后续产生式的最长的左因子长度
             */
            int count = 0;
            for (productionTemplate = PRODUCTIONHEAD(rule);
                 productionTemplate != NULL;
                 productionTemplate = productionTemplate->next) {
                if ((count = LeftFactorMaxLength(productionTemplate)) > 0)
                    break;
            }

            /**
             * @brief 如果当前文法没有左因子则继续判断下一条文法
             */
            if (count == 0) {
                continue;
            }

            /**
             * @brief 创建一条以当前文法为名称模板的新文法
             */
            newRule = NewRule(RULENAME(rule));
            char *newName = GetUniqueRuleName(rule, RULENAME(newRule));
            free (RULENAME(newRule));
            RULENAME(newRule) = newName;
            isChange = 1;

            /**
             * @brief 把模板产生式左因子之后的产生式加入到新文法中
             * 然后将其替换为新文法的符号
             */
            AddProductionToRule(newRule,
                                GetSymbol(productionTemplate, count));
            Symbol *tmp = NewSymbol(RULENAME(newRule));
            RULE(tmp) = newRule;
            GetSymbol(productionTemplate, count - 1)->next = tmp;

            /**
             * @brief 在模板产生式之后查找包含左因子的产生式，然后提取左因子
             */
            production = productionTemplate->next;
            Production **pProductionPtr = &productionTemplate->next;
            while (production != NULL) {

                /**
                 * @brief 如果产生式中包含左因子，则将左因子后的产生式
                 * 加入到新文法中，并将该产生式从文法中移除
                 */
                if (NeedPickup(productionTemplate, count, production)) {
                    AddProductionToRule(newRule,
                                        GetSymbol(production, count));
                    *pProductionPtr = production->next;
                    GetSymbol(production, count - 1)->next = NULL;
                    FreeProduction(production);
                    production = *pProductionPtr;
                } else {
                    pProductionPtr = &production->next;
                    production = production->next;
                }
            }

            /**
             * @brief 将新文法加入到文法链表中
             */
            while (rule->next) {
                rule = rule->next;
            }
            rule->next = newRule;
        }
    } while (isChange == 1);
}

/**
 * @brief 根据下标找到 Production 中的 Symbol
 *
 * @param production Production 的指针
 * @param index 下标
 * @return Symbol* 如果找到则返回 Symbol 的指针，否则返回 NULL
 */
Symbol *GetSymbol(Production *production, int index) {
    int i = 0;
    Symbol *symbol;
    for (symbol = SYMBOLHEAD(production), i = 0; symbol != NULL;
         symbol = symbol->next, i++) {
        if (i == index) {
            return symbol;
        }
    }
    return NULL;
}

/**
 * @brief 计算该条产生式中左因子的最大长度
 *
 * @param production 产生式指针
 * @return int 最大长度
 */
int LeftFactorMaxLength(Production *production) {
    int maxLength = 0;
    for (Production *pCmpProduction = production->next;
         pCmpProduction != NULL;
         pCmpProduction = pCmpProduction->next) {
        int length = 0;
        for (Symbol *pA = production, *pB = pCmpProduction;
             !pA || !pB || !SymbolCmp(pA, pB);
             length++, pA = pA->next, pB = pB->next) {
        }
        maxLength = length > maxLength ? length : maxLength;
    }
    return maxLength;
}

/**
 * @brief 比较 Symbol 是否相等
 *
 * @param symbol1 Symbol1 的指针
 * @param symbol2 Symbol2 的指针
 * @return int 是否相等
 */
int SymbolCmp(const Symbol *symbol1, const Symbol *symbol2) {
    if (ISTOKEN(symbol1) == ISTOKEN(symbol2)) {
        if (ISTOKEN(symbol1)) {
            return strcmp(SYMBOLNAME(symbol1), SYMBOLNAME(symbol2)) == 0;
        } else {
            return RULE(symbol1) == RULE(symbol2);
        }
    } else {
        return 0;
    }
}

/**
 * @brief 判断产生式是否需要被替换
 *
 * @param productionTemplate 模板产生式的指针
 * @param count 指定要比较的符号个数
 * @param production 产生式的指针
 * @return int
 */
int NeedPickup(const Production *productionTemplate, int count,
               const Production *production) {
    Symbol *pA = productionTemplate, *pB = production;
    for (int i = 0; i < count;
         i++, pA = pA->next, pB = pB->next) {
        if (!pA || !pB || !SymbolCmp(pA, pB)) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 将产生式加入到文法中的产生式链表中
 *
 * @param rule 文法的指针
 * @param production 产生式的指针
 */
void AddProductionToRule(Rule *rule, Production *production) {
    if (!production) {
        production = NewProduction();
        Symbol *symbol = NewSymbol(VOID_SYMBOL);
        SYMBOLHEAD(production) = symbol;
    }
    PRODUCTIONHEAD(rule) = Append(PRODUCTIONHEAD(rule), production);
}

/**
 * @brief 得到一个不重复的新文法名，如果存在冲突则添加后缀
 *
 * @param ruleHead 文法链表的头指针
 * @param ruleName 像取得新文法名
 */
char *GetUniqueRuleName(const Rule *ruleHead, const char *ruleName) {
    char tmp[MAX_STR_LENGTH];
    strcpy(tmp, ruleName);
    for (Rule *rule = ruleHead; rule != NULL;) {
        if (strcmp(RULENAME(rule), tmp) == 0) {
            strcat(tmp, POSTFIX);
            rule = ruleHead;
            continue;
        }
        rule = rule->next;
    }
    return strdup (tmp);
}

/**
 * @brief 释放整个产生式链表的内存
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

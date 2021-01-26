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
            for (productionTemplate = PRODUCTION_HEAD(rule);
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
            newRule = NewRule(RULE_NAME(rule));
            char *newName = GetUniqueRuleName(rule, RULE_NAME(newRule));
            free (RULE_NAME(newRule));
            RULE_NAME(newRule) = newName;
            isChange = 1;

            /**
             * @brief 把模板产生式左因子之后的产生式加入到新文法中
             * 然后将其替换为新文法的符号
             */
            
            Production *newProduction = NewProduction();
            SYMBOL_HEAD(newProduction) = GetSymbol(productionTemplate, count);
            AddProductionToRule(newRule, newProduction);
            Symbol *tmp = NewSymbol(RULE_NAME(newRule));
            RULE(tmp) = newRule;
            GetSymbol(productionTemplate, count - 1)->next = tmp;

            /**
             * @brief 在模板产生式之后查找包含左因子的产生式，然后提取左因子
             */
            production = productionTemplate->next;
            while (production != NULL) {

                /**
                 * @brief 如果产生式中包含左因子，则将左因子后的产生式
                 * 加入到新文法中，并将该产生式从文法中移除
                 */
                if (NeedPickup(productionTemplate, count, production)) {
                    Production *newProduction = NewProduction();
                    SYMBOL_HEAD(newProduction) = GetSymbol(production, count);
                    AddProductionToRule(newRule, newProduction);
                    Production *next = production->next;
                    productionTemplate->next = DeleteNode(productionTemplate->next, production);
                    production = next;
                    //FreeProduction(production);
                } else {
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
    for (symbol = SYMBOL_HEAD(production), i = 0; symbol != NULL;
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
    for (Production *cmpProduction = production->next;
         cmpProduction != NULL;
         cmpProduction = cmpProduction->next) {
        int length = 0;
        for (Symbol *a = SYMBOL_HEAD(production), *b = SYMBOL_HEAD(cmpProduction);
             a && b && !strcmp(SYMBOL_NAME(a), SYMBOL_NAME(b));
             length++, a = a->next, b = b->next) {
        }
        maxLength = length > maxLength ? length : maxLength;
    }
    return maxLength;
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
    Symbol *a = SYMBOL_HEAD(productionTemplate), *b = SYMBOL_HEAD(production);
    for (int i = 0; i < count;
         i++, a = a->next, b = b->next) {
        if (!a || !b || strcmp(SYMBOL_NAME(a), SYMBOL_NAME(b))) {
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
        SYMBOL_HEAD(production) = symbol;
    }
    PRODUCTION_HEAD(rule) = AppendNode(PRODUCTION_HEAD(rule), production);
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
        if (strcmp(RULE_NAME(rule), tmp) == 0) {
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

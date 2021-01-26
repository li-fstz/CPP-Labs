#include "removeleftrecursion2.h"

#include <stdio.h>

/**
 * @brief 判断当前文法中的符号是否需要被替换，
 *        如果这个符号是非终结符且指向的文法在当前文法之前，
 *        那么该终结符应该被替换
 *
 * @param rule 文法指针
 * @param symbol 符号指针
 * @return int 是否需要被替换
 */
int SymbolNeedReplace(const Rule *rule, const Symbol *symbol) {
    assert(IS_RULE(rule) && IS_SYMBOL(symbol));
    if (IS_TOKEN(symbol)) {
        return 0;
    } else {
        Rule *tmp = RULE(symbol)->next;
        while (tmp) {
            if (rule == tmp) {
                return 1;
            } else {
                tmp = tmp->next;
            }
        }
        return 0;
    }
}

/**
 * @brief 替换产生式的第一个符号
 *
 * @param productionTemplate 需要被替换的产生式指针
 * @return Production* 替换后的新产生式指针，
 *                 注意第一个符号指向的产生式可能是一个链表，
 *                 所以返回的新产生是也可能是一个链表
 */
Production *ReplaceProduction(const Production *productionTemplate) {
    assert(IS_PRODUCTION(productionTemplate));
    Production *productionsOfFirstSymble = CopyProductions(
                   PRODUCTION_HEAD(RULE(SYMBOL_HEAD(productionTemplate)))),
               *tmpProduction = productionsOfFirstSymble;
    while (tmpProduction) {
        // leak
        AppendNode(SYMBOL_HEAD(tmpProduction),
                   SYMBOL_HEAD(CopyProduction(productionTemplate))->next);
        tmpProduction = tmpProduction->next;
    }
    return productionsOfFirstSymble;
}

/**
 * @brief 释放一个产生式链表的内存
 *
 * @param production 产生式链表头指针
 */
void FreeProduction(Production *production) {
    assert(IS_PRODUCTION(production));
    if (production->next) {
        FreeProduction(production->next);
    } else {
        free(production);
    }
}

/**
 * @brief 判断该产生式是否存在左递归
 *
 * @param production 产生式指针
 * @param rule 指向该产生式的文法指针
 * @return int 是否存在左递归
 */
int ProductionHasLeftRecursion(Production *production, Rule *rule) {
    assert(IS_PRODUCTION(production) && IS_RULE(rule));
    if (IS_TOKEN(SYMBOL_HEAD(production))) {
        return 0;
    } else {
        return RULE(SYMBOL_HEAD(production)) == rule;
    }
}

/**
 * @brief 判断该文法是否存在左递归
 *
 * @param rule 文法指针
 * @return int  是否存在左递归
 */
int RuleHasLeftRecursion(const Rule *rule) {
    assert(IS_RULE(rule));
    Production *tmp = PRODUCTION_HEAD(rule);
    while (tmp) {
        if (ProductionHasLeftRecursion(tmp, rule)) {
            return 1;
        }
        tmp = tmp->next;
    }
    return 0;
}

/**
 * @brief 讲一个产生式添加到文法指向的产生式链表尾部，
 *        当产生式指针为 NULL 时则添加 ε
 *
 * @param rule 文法指针
 * @param production 产生式指针
 */
void AddProductionToRule(Rule *rule, const Production *production) {
    assert(IS_RULE(rule) && (!production || IS_PRODUCTION(production)));
    if (!production) {
        production = NewProduction();
        Symbol *symbol = NewSymbol(VOID_SYMBOL);
        SYMBOL_HEAD(production) = symbol;
    }
    PRODUCTION_HEAD(rule) = AppendNode(PRODUCTION_HEAD(rule), production);
}

/**
 * @brief 消除左递归
 *
 * @param ruleHead 文法链表头指针
 */
void RemoveLeftRecursion(Rule *ruleHead) {
    assert(IS_RULE(ruleHead));
    Rule *rule, *newRule;
    Production *production;

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
            for (production = PRODUCTION_HEAD(rule); production != NULL;
                 production = production->next) {
                /**
                 * @brief 判断当前产生式的第一个符号是否需要替换
                 * 如果这个符号对应的文法在当前文法的前面，则需要被替换
                 */
                if (SymbolNeedReplace(rule, SYMBOL_HEAD(production))) {
                    isChange = 1;
                    Production *newProductions = ReplaceProduction(production);

                    /**
                     * @brief 用替换后的产生式链表替换之前的产生式，
                     * 最后释放之前产生式的内存
                     */
                    PRODUCTION_HEAD(rule) = InsertNode(
                        PRODUCTION_HEAD(rule), production, newProductions);
                    PRODUCTION_HEAD(rule) =
                        DeleteNode(PRODUCTION_HEAD(rule), production);
                    // FreeProduction(production);
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
        char newName[MAX_STR_LENGTH];
        strcpy(newName, RULE_NAME(rule));
        strcat(newName, POSTFIX);
        newRule = NewRule(newName);

        production = PRODUCTION_HEAD(rule);
        while (production != NULL) {

            /**
             * @brief 创建新文法的符号
             */
            Symbol *tmp = NewSymbol(newName);
            RULE(tmp) = newRule;

            /**
             * @brief 如果当前产生式包含左递归，则移除此产生式，
             * 然后将此产生式的左递归转换为右递归，然后将其加入到新文法中；
             * 如果当前产生式不包含左递归，则在产生式尾部添加新文法的符号。
             */
            if (!IS_TOKEN(SYMBOL_HEAD(production)) &&
                RULE(SYMBOL_HEAD(production)) == rule) {
                Production *next = production->next;
                PRODUCTION_HEAD(rule) =
                    DeleteNode(PRODUCTION_HEAD(rule), production);
                production->next = NULL;
                SYMBOL_HEAD(production) = SYMBOL_HEAD(production)->next;
                AppendNode(SYMBOL_HEAD(production), tmp);
                AddProductionToRule(newRule, production);
                production = next;
            } else {
                AppendNode(SYMBOL_HEAD(production), tmp);
                production = production->next;
            }
        }

        /**
         * @brief 向新文法添加 ε 产生式，然后将新文法加入到文法链表中
         */
        AddProductionToRule(newRule, NULL);
        InsertNode(rule, rule->next, newRule);
        rule = rule->next;
    }
}

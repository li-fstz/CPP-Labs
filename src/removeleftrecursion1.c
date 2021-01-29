#include "removeleftrecursion1.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief 将产生式加入到文法中的产生式链表中
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
    Rule *newRule;
    Production *production = PRODUCTION_HEAD(ruleHead);

    /**
     * @brief 创建一条新文法
     */
    char newName[MAX_STR_LENGTH];
    strcpy(newName, RULE_NAME(ruleHead));
    strcat(newName, POSTFIX);
    newRule = NewRule(newName);

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
            RULE(SYMBOL_HEAD(production)) == ruleHead) {
            Production *next = production->next;
            PRODUCTION_HEAD(ruleHead) =
                DeleteNode(PRODUCTION_HEAD(ruleHead), production);
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
    AppendNode(ruleHead, newRule);
    return;
}

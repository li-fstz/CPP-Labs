#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 输出空表
 *
 * @param voidTable 空表指针
 */
void PrintVoidTable(const VoidTable *voidTable) {
    assert(IS_VOID_TABLE(voidTable));
    for (int i = 0; i < voidTable->colCount; i++) {
        printf("%s%c", voidTable->tableHead[i],
               i == voidTable->colCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; i < voidTable->colCount; i++) {
        printf("%d%c", VOIDTABLE_ROW(voidTable, 0).hasVoid[i],
               i == voidTable->colCount - 1 ? '\n' : '\t');
    }
}

/**
 * @brief 在空表中标记一定能或不能推出 ε 的文法，
 * 并在复制过得文法链表中删去这些文法，
 * 同时还会删去一定不能推出 ε 的产生式。
 *
 * @param copiedRulePrePtr 复制后的文法链表指针指针
 * @param voidTable 空表指针
 */
void MarkVoidAndNotVoid(Rule **copiedRulePrePtr, VoidTable *voidTable) {
    assert(copiedRulePrePtr && IS_VOID_TABLE(voidTable));
    Rule *rule = *copiedRulePrePtr;
    Production *production;
    while (rule != NULL) {
        production = PRODUCTION_HEAD(rule);

        /**
         * @brief 标记当前文法是否包含 ε 产生式
         */
        int hasVoidProduction = 0;
        while (production != NULL) {
            /**
             * @brief 如果该产生式只有一个符号且是 ε
             * 则标记文法包含 ε 产生式
             */
            Symbol *firstSymbol = SYMBOL_HEAD(production);
            if (firstSymbol->next == NULL &&
                strcmp(SYMBOL_NAME(firstSymbol), VOID_SYMBOL) == 0) {
                hasVoidProduction = 1;
                break;
            }

            /**
             * @brief 标记该产生式是否需要被删除
             * 如果该产生式一定不能推出 ε 则该产生式需要被删除
             */
            int deleteThisProduction = 0;
            for (Symbol *symbol = SYMBOL_HEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                if (IS_TOKEN(symbol) &&
                    strcmp(SYMBOL_NAME(symbol), VOID_SYMBOL) != 0) {
                    deleteThisProduction = 1;
                    break;
                }
            }
            if (deleteThisProduction) {
                Production *next = production->next;
                PRODUCTION_HEAD(rule) =
                    DeleteNode(PRODUCTION_HEAD(rule), production);
                // FreeProduction(production);
                production = next;
            } else {
                production = production->next;
            }
        }

        /**
         * @brief 如果该文法包含 ε 产生式，
         * 则标记该文法可以推出 ε，并删除该文法；
         * 如果该文法指向的产生式链表为空，
         * 则标记该文法不可以推出 ε，并删除该文法。
         */
        if (hasVoidProduction || PRODUCTION_HEAD(rule) == NULL) {
            *FindHasVoid(voidTable, RULE_NAME(rule)) = hasVoidProduction;
            Rule *next = rule->next;
            *copiedRulePrePtr = DeleteNode(*copiedRulePrePtr, rule);
            // FreeRule(rule);
            rule = next;
        } else {
            rule = rule->next;
        }
    }
}

/**
 * @brief 生成文法的空表
 *
 * @param ruleHead 文法链表头指针
 * @return VoidTable* 生成的空表指针
 */
VoidTable *GenVoidTable(const Rule *ruleHead) {
    assert(IS_RULE(ruleHead));
    VoidTable *voidTable = malloc(sizeof(VoidTable));
    voidTable->rowCount = 1;
    voidTable->tableHead = GetNonTerminals(ruleHead, &voidTable->colCount);
    voidTable->tableRows = malloc(sizeof(struct VoidTableRow));
    voidTable->type = Void;
    VOIDTABLE_ROW(voidTable, 0).hasVoid =
        malloc(voidTable->colCount * sizeof(int));
    memset(VOIDTABLE_ROW(voidTable, 0).hasVoid, -1,
           voidTable->colCount * sizeof(int));
    Rule *copiedRule = CopyRules(ruleHead), *rule;

    /**
     * @brief 首先扫描并标记文法链表中一定能或不能推出 ε 的文法，
     * 并在文法链表中删去这戏文法，
     * 同时还会删去一定不能推出 ε 的产生式。
     */
    MarkVoidAndNotVoid(&copiedRule, voidTable);
    int isChange;
    do {
        isChange = 0;
        rule = copiedRule;
        while (rule != NULL) {

            /**
             * @brief 标记该文法剩余的所有产生式都不能推出 ε
             */
            int allProductionNotVoid = 1;

            /**
             * @brief 标记该文法含有一条可以推出 ε 的产生式
             */
            int hasVoidProduction;
            for (Production *production = PRODUCTION_HEAD(rule);
                 production != NULL; production = production->next) {
                Symbol *symbol = SYMBOL_HEAD(production);
                hasVoidProduction = 0;
                while (symbol != NULL) {
                    switch (*FindHasVoid(voidTable, SYMBOL_NAME(symbol))) {
                    case 1:

                        /**
                         * @brief 这个符号对应的文法可以推出 ε
                         * 则判断该产生式是否只剩此符号
                         * 如果只剩此符号说明该文法包含可以推出 ε 的产生式
                         * 否则在此产生式中删去该符号
                         */
                        if (symbol->next == NULL &&
                            symbol == SYMBOL_HEAD(production)) {
                            hasVoidProduction = 1;
                        } else {
                            Symbol *next = symbol->next;
                            SYMBOL_HEAD(production) =
                                DeleteNode(SYMBOL_HEAD(production), symbol);
                            // FreeSymble(symbol);
                            symbol = next;
                        }
                        break;
                    case -1:
                        /**
                         * @brief 不知道这个符号对应的文法是否可以推出 ε
                         * 则要取消对该文法剩余的所有产生式都不能推出 ε 的标记
                         */
                        allProductionNotVoid = 0;
                    case 0:
                        symbol = symbol->next;
                        break;
                    }
                    if (hasVoidProduction) {
                        break;
                    }
                }
                if (hasVoidProduction) {
                    break;
                }
            }

            /**
             * @brief 如果这个文法包含可以推出 ε
             * 的产生式或者所有产生式都不能推出 ε
             * 则在空表中标记该产生式，并且在文法链表中删去该文法
             */
            if (allProductionNotVoid || hasVoidProduction) {
                isChange = 1;
                *FindHasVoid(voidTable, RULE_NAME(rule)) = hasVoidProduction;
                Rule *next = rule->next;
                copiedRule = DeleteNode(copiedRule, rule);
                // FreeRule(rule);
                rule = next;
            } else {
                rule = rule->next;
            }
        }
    } while (isChange);
    return voidTable;
}

/**
 * @brief 在空表中根据文法名字找到值的位置
 *
 * @param voidTable 空表指针
 * @param ruleName 文法的名字
 * @return int* 值在空表中的位置
 */
int *FindHasVoid(VoidTable *voidTable, const char *ruleName) {
    assert(IS_VOID_TABLE(voidTable) && ruleName);
    for (int i = 0; i < voidTable->colCount; i++) {
        if (strcmp(voidTable->tableHead[i], ruleName) == 0) {
            return VOIDTABLE_ROW(voidTable, 0).hasVoid + i;
        }
    }
    return 0;
}

/**
 * @brief 从文法中提取所有的非终结符
 *
 * @param rule 文法链表头指针
 * @return char** 非终结符数组
 */
char **GetNonTerminals(const Rule *ruleHead, int *count) {
    assert(IS_RULE(ruleHead) && count);
    int i;
    char **nonTerminals = NULL;
    for (i = 0; ruleHead != NULL; ruleHead = ruleHead->next, i++) {
        nonTerminals = realloc(nonTerminals, (i + 1) * sizeof(char *));
        nonTerminals[i] = strdup(RULE_NAME(ruleHead));
    }
    *count = i;
    return nonTerminals;
}

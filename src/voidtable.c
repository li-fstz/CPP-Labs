#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 输出空表
 *
 * @param voidTable 空表的指针
 */
void PrintVoidTable(const VoidTable *voidTable) {
    for (int i = 0; i < voidTable->colCount; i++) {
        printf("%s%c", voidTable->tableHead[i],
               i == voidTable->colCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; i < voidTable->colCount; i++) {
        printf("%d%c", voidTable->tableRows[0].hasVoid[i],
               i == voidTable->colCount - 1 ? '\n' : '\t');
    }
}

/**
 * @brief 在空表中标记一定能或不能推出 ε 的文法，
 * 并在复制过得文法链表中删去这些文法，
 * 同时还会删去一定不能推出 ε 的产生式。
 *
 * @param copiedRulePrePtr 复制后的文法链表的指针的指针
 * @param voidTable 空表的指针
 */
void MarkVoidAndNotVoid(Rule **copiedRulePrePtr, VoidTable *voidTable) {
    Rule *rule = *copiedRulePrePtr;
    Production *production;
    while (rule != NULL) {
        production = PRODUCTIONHEAD(rule);

        /**
         * @brief 标记当前文法是否包含 ε 产生式
         */
        int hasVoidProduction = 0;
        while (production != NULL) {
            /**
             * @brief 如果该产生式只有一个符号且是 ε
             * 则标记文法包含 ε 产生式
             */
            Symbol *firstSymbol = SYMBOLHEAD(production);
            if (firstSymbol->next == NULL &&
                strcmp(SYMBOLNAME(firstSymbol), VOID_SYMBOL) == 0) {
                hasVoidProduction = 1;
                break;
            }

            /**
             * @brief 标记该产生式是否需要被删除
             * 如果该产生式一定不能推出 ε 则该产生式需要被删除
             */
            int deleteThisProduction = 0;
            for (Symbol *symbol = SYMBOLHEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                if (ISTOKEN(symbol) &&
                    strcmp(SYMBOLNAME(symbol), VOID_SYMBOL) != 0) {
                    deleteThisProduction = 1;
                    break;
                }
            }
            if (deleteThisProduction) {
                Production *next = production->next;
                PRODUCTIONHEAD(rule) = Delete(PRODUCTIONHEAD(rule), production);
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
        if (hasVoidProduction || PRODUCTIONHEAD(rule) == NULL) {
            *FindHasVoid(voidTable, RULENAME(rule)) = hasVoidProduction;
            Rule *next = rule->next;
            *copiedRulePrePtr = Delete(*copiedRulePrePtr, rule);
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
 * @param ruleHead 文法链表的头指针
 * @return VoidTable* 生成的空表的指针
 */
VoidTable *GenVoidTable(const Rule *ruleHead) {
    VoidTable *voidTable = malloc(sizeof(VoidTable));
    voidTable->tableHead = GetNonTerminals(ruleHead);
    for (voidTable->colCount = 0; voidTable->tableHead[voidTable->colCount];
         voidTable->colCount++) {
        voidTable->tableRows[0].hasVoid[voidTable->colCount] = -1;
    }
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
            for (Production *production = PRODUCTIONHEAD(rule);
                 production != NULL;
                 production = production->next) {
                Symbol *symbol = SYMBOLHEAD(production);
                hasVoidProduction = 0;
                while (symbol != NULL) {
                    switch (*FindHasVoid(voidTable, SYMBOLNAME(symbol))) {
                    case 1:

                        /**
                         * @brief 这个符号对应的文法可以推出 ε
                         * 则判断该产生式是否只剩此符号
                         * 如果只剩此符号说明该文法包含可以推出 ε 的产生式
                         * 否则在此产生式中删去该符号
                         */
                        if (symbol->next == NULL &&
                            symbol == SYMBOLHEAD(production)) {
                            hasVoidProduction = 1;
                        } else {
                            Symbol *next = symbol->next;
                            SYMBOLHEAD(production) = Delete(SYMBOLHEAD(production), symbol);
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
                *FindHasVoid(voidTable, RULENAME(rule)) = hasVoidProduction;
                Rule *next = rule->next;
                ruleHead = Delete(ruleHead, rule);
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
 * @param pTable 空表的指针
 * @param ruleName 文法的名字
 * @return int* 值在空表中的位置
 */
int *FindHasVoid(VoidTable *pTable, const char *ruleName) {
    for (int i = 0; i < pTable->colCount; i++) {
        if (strcmp(pTable->tableHead[i], ruleName) == 0) {
            return pTable->tableRows[0].hasVoid + i;
        }
    }
    return 0;
}

/**
 * @brief 从文法中提取所有的非终结符
 *
 * @param rule 文法链表的头指针
 * @return char** 非终结符数组
 */
char **GetNonTerminals(const Rule *ruleHead) {
    char **nonTerminals = calloc(32, sizeof(char *));
    for (int i = 0; ruleHead != NULL; ruleHead = ruleHead->next, i++) {
        nonTerminals[i] = strdup(RULENAME(ruleHead));
    }
    return nonTerminals;
}

#include "follow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "first.h"
#include "rule.h"

/**
 * @brief 从子集中删去 ε
 *
 * @param set 子集指针
 * @return int 返回是否修改
 */
int RemoveVoidFromSet(Set *set) {
    assert(set);
    for (int i = 0; i < set->terminalCount; i++) {
        if (strcmp(set->terminals[i], VOID_SYMBOL) == 0) {
            if (i != set->terminalCount - 1) {
                memmove(set->terminals + i, set->terminals + i + 1,
                        sizeof(char *) * (set->terminalCount - i - 1));
            }
            set->terminalCount--;
            set->terminals =
                realloc(set->terminals, set->terminalCount * sizeof(char *));
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 生成文法的 Follow 集
 *
 * @param ruleHead 文法链表头指针
 * @param voidTable 空表指针
 * @param firstSetList First 集指针
 * @return FollowSetList* 生成 Follow 集指针
 */
FollowSetList *GenFollowSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const FirstSetList *firstSetList) {
    assert(IS_RULE(ruleHead) && voidTable && IS_FIRST_SET(firstSetList));
    FollowSetList *followSetList = calloc(1, sizeof(SetList));
    followSetList->type = FollowSet;

    const Rule *rule;
    const Production *production;
    const Symbol *symbol;

    /**
     * @brief Follow 集是否被修改的标志
     */
    int isChange;

    /**
     * @brief 初始化 Follow 集
     * 在开始符号的 Follow 子集中加入结束符
     */
    for (rule = ruleHead; rule != NULL; rule = rule->next) {
        AddOneSet(followSetList, strdup(RULE_NAME(rule)), strKeyCmp);
    }
    AddTerminalToSet(&followSetList->sets[0], END_SYMBOL);

    do {
        isChange = 0;
        for (rule = ruleHead; rule != NULL; rule = rule->next) {
            for (production = PRODUCTION_HEAD(rule); production != NULL;
                 production = production->next) {
                for (symbol = SYMBOL_HEAD(production); symbol != NULL;
                     symbol = symbol->next) {
                    if (IS_TOKEN(symbol))
                        continue; // 忽略终结符

                    /**
                     * @brief 初始化一个临时子集
                     */
                    Set tmpSet;
                    memset(&tmpSet, 0, sizeof(Set));

                    Symbol *tmpSymbol;

                    /**
                     * @brief
                     * 从当前符号向后查找，
                     * 如果为非 ε 的终结符，则将其加入临时子集，
                     * 并结束向后查找；
                     * 如果为非终结符，
                     * 则将其对应的 First 子集中的符号加入临时子集，
                     * 并且，当其不能推出 ε 时结束向后查找；
                     */
                    for (tmpSymbol = symbol->next; tmpSymbol != NULL;
                         tmpSymbol = tmpSymbol->next) {
                        if (IS_TOKEN(tmpSymbol)) {
                            if (strcmp(SYMBOL_NAME(tmpSymbol), VOID_SYMBOL) !=
                                0) {
                                AddTerminalToSet(&tmpSet,
                                                 SYMBOL_NAME(tmpSymbol));
                                break;
                            }
                        } else {
                            AddSetToSet(&tmpSet, GetSet(firstSetList,
                                                        SYMBOL_NAME(tmpSymbol),
                                                        strKeyCmp));
                            if (!*FindHasVoid(voidTable,
                                              SYMBOL_NAME(tmpSymbol))) {
                                break;
                            }
                        }
                    }

                    /**
                     * @brief Follow 集中不包含 ε
                     * 所以要在临时子集中删去 ε 之后再合并到目标 Follow 子集。
                     */
                    RemoveVoidFromSet(&tmpSet);

                    /**
                     * @brief 目标 Follow 子集
                     */
                    Set *desSet =
                        GetSet(followSetList, SYMBOL_NAME(symbol), strKeyCmp);
                    isChange = AddSetToSet(desSet, &tmpSet) || isChange;

                    /**
                     * @brief
                     * 如果当前符号是最后一个符号或后续符号可以推出空
                     * 则将该产生式对应文法的 Follow 子集中的符号
                     * 加入目标 Follow子集。
                     */
                    if (tmpSymbol == NULL) {
                        isChange = AddSetToSet(desSet, GetSet(followSetList,
                                                              RULE_NAME(rule),
                                                              strKeyCmp)) ||
                                   isChange;
                    }
                }
            }
        }
    } while (isChange);
    return followSetList;
}

/**
 * @brief 输出 Follow 集
 *
 * @param followSetList Follow 集指针
 */
void PrintFollowSetList(const FollowSetList *followSetList) {
    assert(IS_FOLLOW_SET(followSetList));
    printf("\nThe Follow Set:\n");
    for (int i = 0; i < followSetList->setCount; i++) {
        printf("Follow(%s) = { ", followSetList->sets[i].key);
        for (int j = 0; j < followSetList->sets[i].terminalCount; j++) {
            if (j == followSetList->sets[i].terminalCount - 1) {
                printf("%s ", followSetList->sets[i].terminals[j]);
            } else {
                printf("%s, ", followSetList->sets[i].terminals[j]);
            }
        }
        printf("}\n");
    }
}

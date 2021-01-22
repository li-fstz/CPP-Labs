#include "first.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rule.h"
#include "voidtable.h"

/**
 * @brief 输出 First 集
 *
 * @param firstSetList First 集指针
 */
void PrintFirstSetList(const SetList *firstSetList) {
    printf("\nThe First Set:\n");
    for (int i = 0; i < firstSetList->setCount; i++) {
        printf("First(%s) = { ", firstSetList->sets[i].name);
        for (int j = 0; j < firstSetList->sets[i].terminalCount; j++) {
            if (j == firstSetList->sets[i].terminalCount - 1) {
                printf("%s ", firstSetList->sets[i].terminals[j]);
            } else {
                printf("%s, ", firstSetList->sets[i].terminals[j]);
            }
        }
        printf("}\n");
    }
}

/**
 * @brief 生成文法的 First 集
 *
 * @param ruleHead 文法链表的头指针
 * @param voidTable 空表的指针
 * @return SetList* 生成的 First 集的指针
 */
SetList *GenFirstSetList(const Rule *ruleHead, const VoidTable *voidTable) {
    SetList *firstSetList = malloc(sizeof(SetList));
    firstSetList->setCount = 0;

    const Rule *rule;
    const Production *production;
    const Symbol *symbol;

    /**
     * @brief First 集是否被修改的标志
     */
    int isChange;

    /**
     * @brief 初始化 First 集
     * 如果该文法可以推出 ε，则向 First 集中加 ε
     */
    for (rule = ruleHead; rule != NULL; rule = rule->next) {
        AddOneSet(firstSetList, RULENAME(rule));
        if (*FindHasVoid(voidTable, RULENAME(rule))) {
            AddTerminalToSet(firstSetList->sets + firstSetList->setCount - 1,
                             VOID_SYMBOL);
        }
    }

    do {
        isChange = 0;
        for (rule = ruleHead; rule != NULL; rule = rule->next) {

            /**
             * @brief 该文法对应的 First 子集
             */
            Set *desSet = GetSet(firstSetList, RULENAME(rule));
            for (production = PRODUCTIONHEAD(rule); production != NULL;
                 production = production->next) {
                for (symbol = SYMBOLHEAD(production); symbol != NULL;
                     symbol = symbol->next) {

                    /**
                     * @brief
                     *
                     * 如果是终结符，
                     * 则将该终结符加入到该文法 First 子集中，
                     * 并结束对该产生式的查找；
                     *
                     * 如果是非终结符，
                     * 则将非终结符对应的文法 First 子集中的终结符添加到
                     * 该文法 First 子集中，
                     * 如果非终结符对应的文法不能推出空,则结束对该产生式的查找。
                     */
                    if (ISTOKEN(symbol)) {
                        isChange =
                            AddTerminalToSet(desSet, SYMBOLNAME(symbol)) ||
                            isChange;
                        break;
                    } else {

                        /**
                         * @brief 非终结符对应的文法 First 子集
                         */
                        Set *srcSet =
                            GetSet(firstSetList, SYMBOLNAME(symbol));
                        isChange = AddSetToSet(desSet, srcSet) || isChange;
                        if (!*FindHasVoid(voidTable, srcSet->name)) {
                            break;
                        }
                    }
                }
            }
        }
    } while (isChange);
    return firstSetList;
}

/**
 * @brief 在 First 集或 Follow 集中添加一个非终结符子集
 *
 * @param setList First 集或 Follow 集的指针
 * @param name 非终结符子集名字
 */
void AddOneSet(SetList *setList, const char *name) {
    for (int i = 0; i < setList->setCount; i++) {
        if (strcmp(setList->sets[i].name, name) == 0) {
            return;
        }
    }
    setList->sets[setList->setCount].terminalCount = 0;
    setList->sets[setList->setCount++].name = strdup (name);
}

/**
 * @brief 在 First 集或 Follow 集中查找一个非终结符子集
 *
 * @param setList First 集或 Follow 集的指针
 * @param name 非终结符子集名字
 * @return Set* 如果找到则返回子集的指针，否则返回 NULL
 */
Set *GetSet(const SetList *setList, const char *name) {
    for (int i = 0; i < setList->setCount; i++) {
        if (strcmp(setList->sets[i].name, name) == 0) {
            return setList->sets + i;
        }
    }
    return NULL;
}

/**
 * @brief 添加一个终结符到子集
 *
 * @param set 子集指针
 * @param terminal 终结符
 * @return int 对子集是否有修改
 */
int AddTerminalToSet(Set *set, const char *terminal) {
    for (int i = 0; i < set->terminalCount; i++) {
        if (strcmp(set->terminals[i], terminal) == 0) {
            return 0;
        }
    }
    set->terminals[set->terminalCount++] = strdup (terminal);
    return 1;
}

/**
 * @brief 将源子集中的所有终结符添加至目标子集中
 *
 * @param desSet 目标子集指针
 * @param srcSet 源子集指针
 * @return int 对目标子集是否有修改
 */
int AddSetToSet(Set *desSet, const Set *srcSet) {
    int flag = 0;
    for (int i = 0; i < srcSet->terminalCount; i++) {
        flag = AddTerminalToSet(desSet, srcSet->terminals[i]) || flag;
    }
    return flag;
}

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
        printf("First(%s) = { ", firstSetList->sets[i].key);
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

int strKeyCmp (const void *a, const void *b) {
    return strcmp(a, b) == 0;
}

/**
 * @brief 生成文法的 First 集
 *
 * @param ruleHead 文法链表的头指针
 * @param voidTable 空表的指针
 * @return SetList* 生成的 First 集的指针
 */
FirstSetList *GenFirstSetList(const Rule *ruleHead, const VoidTable *voidTable) {
    FirstSetList *firstSetList = calloc(1, sizeof(SetList));
    firstSetList->type = FirstSet;

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
        AddOneSet(firstSetList, strdup(RULE_NAME(rule)), strKeyCmp);
        if (*FindHasVoid(voidTable, RULE_NAME(rule))) {
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
            Set *desSet = GetSet(firstSetList, RULE_NAME(rule), strKeyCmp);
            for (production = PRODUCTION_HEAD(rule); production != NULL;
                 production = production->next) {
                for (symbol = SYMBOL_HEAD(production); symbol != NULL;
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
                    if (IS_TOKEN(symbol)) {
                        isChange =
                            AddTerminalToSet(desSet, SYMBOL_NAME(symbol)) ||
                            isChange;
                        break;
                    } else {

                        /**
                         * @brief 非终结符对应的文法 First 子集
                         */
                        Set *srcSet =
                            GetSet(firstSetList, SYMBOL_NAME(symbol), strKeyCmp);
                        isChange = AddSetToSet(desSet, srcSet) || isChange;
                        if (!*FindHasVoid(voidTable, srcSet->key)) {
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
void AddOneSet(SetList *setList, const void *key, CmpFunc cmp) {
    for (int i = 0; i < setList->setCount; i++) {
        if (cmp(setList->sets[i].key, key)) {
            return;
        }
    }
    setList->sets = realloc(setList->sets, (setList->setCount + 1) * sizeof (Set));
    setList->sets[setList->setCount].terminalCount = 0;
    setList->sets[setList->setCount].terminals = NULL;
    setList->sets[setList->setCount++].key = key;
}

/**
 * @brief 在 First 集或 Follow 集中查找一个非终结符子集
 *
 * @param setList First 集或 Follow 集的指针
 * @param name 非终结符子集名字
 * @return Set* 如果找到则返回子集的指针，否则返回 NULL
 */
Set *GetSet(const SetList *setList, const void *key, CmpFunc cmp) {
    for (int i = 0; i < setList->setCount; i++) {
        if (cmp(setList->sets[i].key, key)) {
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
    set->terminals = realloc (set->terminals, (set->terminalCount + 1) * sizeof (char *));
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

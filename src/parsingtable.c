#include "parsingtable.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"

int ruleProductionKeyCmp (const void *a, const void * b) {
    return ((struct SelectSetKey *)a)->production == ((struct SelectSetKey *)b)->production;
}

/**
 * @brief 生成 Select 集
 *
 * @param ruleHead 文法链表的头指针
 * @param voidTable 空表的指针
 * @param firstSetList First 集的指针
 * @param followSetList Follow 集的指针
 * @return SetList* 生成的 Select 集的指针
 */
SelectSetList *GenSelectSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const SetList *firstSetList,
                                const SetList *followSetList) {
    SelectSetList *selectSetList = calloc(1, sizeof(SetList));
    selectSetList->type = SelectSet;

    for (; ruleHead != NULL; ruleHead = ruleHead->next) {
        for (Production *production = PRODUCTION_HEAD(ruleHead);
             production != NULL; production = production->next) {

            /**
             * @brief Select 集中的每个子集对应文法中的每一条产生式
             * 首先将这条产生式对应的 First 集中的符号加入到 Select 子集中
             */
            AddOneSelectSet(selectSetList, ruleHead, production);
            Set *selectSet =
                selectSetList->sets + selectSetList->setCount - 1;
            for (Symbol *symbol = SYMBOL_HEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                if (IS_TOKEN(symbol)) {
                    AddTerminalToSet(selectSet, SYMBOL_NAME(symbol));
                    if (strcmp(SYMBOL_NAME(symbol), VOID_SYMBOL) != 0) {
                        break;
                    }
                } else {
                    AddSetToSet(
                        selectSet, GetSet(firstSetList, SYMBOL_NAME(symbol), strKeyCmp));
                    if (!*FindHasVoid(voidTable, SYMBOL_NAME(symbol))) {
                        break;
                    }
                }
            }

            /**
             * @brief 如果该产生式对应的 First 子集中包含 ε
             * 则去除 ε，并且向其中加入文法对应的 Follow 子集中的符号
             */
            if (RemoveVoidFromSet(selectSet)) {
                AddSetToSet(selectSet,
                                  GetSet(followSetList, RULE_NAME(ruleHead), strKeyCmp));
            }
        }
    }
    return selectSetList;
}

/**
 * @brief 生成预测分析表
 *
 * @param ruleHead 文法链表的头指针
 * @param selectSetList Select 集的指针
 * @return ParsingTable* 生成的预测分析表的指针
 */
ParsingTable *GenParsingTable(const Rule *ruleHead,
                              const SetList *selectSetList) {
    ParsingTable *parsingTable = calloc(1, sizeof(ParsingTable));
    parsingTable->tableHead = GetTerminals(ruleHead, &parsingTable->colCount);
    for (int i = 0; ruleHead != NULL; ruleHead = ruleHead->next, i++) {
        parsingTable->tableRows = realloc(parsingTable->tableRows, sizeof(struct ParsingTableRow) * (parsingTable->rowCount + 1));
        PARSINGTABLE_ROW(parsingTable, parsingTable->rowCount).rule = ruleHead;
        PARSINGTABLE_ROW(parsingTable, parsingTable->rowCount++).productions = calloc(parsingTable->colCount, sizeof (Production));
    }
    for (int i = 0; i < selectSetList->setCount; i++) {
        for (int j = 0; j < selectSetList->sets[i].terminalCount; j++) {
            Production **foundProduction =
                FindProduction(parsingTable, RULE_KEY(selectSetList->sets[i]),
                               selectSetList->sets[i].terminals[j]);
            if (*foundProduction) {
                puts("该文法不是 LL(1) 文法！");
            } else {
                *foundProduction = PRODUCTION_KEY(selectSetList->sets[i]);
            }
        }
    }
    return parsingTable;
}

/**
 * @brief 输出 Select 集
 *
 * @param selectSetList Select 集的指针
 */
void PrintSelectSetList(const SelectSetList *selectSetList) {
    printf("\nThe Select Set:\n");
    for (int i = 0; i < selectSetList->setCount; i++) {
        printf("Select(%s->", RULE_NAME(RULE_KEY(selectSetList->sets[i])));
        PrintProduction(PRODUCTION_KEY(selectSetList->sets[i]));
        printf(") = { ");
        for (int j = 0; j < selectSetList->sets[i].terminalCount; j++) {
            printf("%s%s", selectSetList->sets[i].terminals[j],
                   j == selectSetList->sets[i].terminalCount - 1 ? " }\n"
                                                                   : ", ");
        }
    }
}

/**
 * @brief 输出预测分析表
 *
 * @param parsingTable 预测分析表的指针
 */
void PrintParsingTable(const ParsingTable *parsingTable) {
    putchar('\t');
    for (int i = 0; i < parsingTable->colCount; i++) {
        printf("%s%c", parsingTable->tableHead[i],
               i == parsingTable->colCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; i < parsingTable->rowCount; i++) {
        printf("%s\t", RULE_NAME(PARSINGTABLE_ROW(parsingTable, i).rule));
        for (int j = 0; j < parsingTable->colCount; j++) {
            if (PARSINGTABLE_ROW(parsingTable, i).productions[j]) {
                PrintProduction(PARSINGTABLE_ROW(parsingTable, i).productions[j]);
            }
            putchar(j == parsingTable->colCount - 1 ? '\n' : '\t');
        }
    }
}

/**
 * @brief 在 Select 子集中添加一个产生式子集
 *
 * @param setList Select 子集的指针
 * @param rule 指向产生式的文法的指针
 * @param production 产生式的指针
 */
void AddOneSelectSet(SelectSetList *setList, const Rule *rule,
                     const Production *production) {
    struct SelectSetKey *key = malloc(sizeof (struct SelectSetKey));
    key->rule = rule;
    key->production = production;
    AddOneSet(setList, key, ruleProductionKeyCmp);
}

/**
 * @brief 从文法中提取所有的终结符
 *
 * @param ruleHead 文法链表的头指针
 * @return char** 终结符数组
 */
char **GetTerminals(const Rule *ruleHead, int *count) {
    char **terminals = NULL;
    *count = 0;
    for (; ruleHead != NULL; ruleHead = ruleHead->next) {
        for (Production *production = PRODUCTION_HEAD(ruleHead);
             production != NULL; production = production->next) {
            for (Symbol *symbol = SYMBOL_HEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                if (!IS_TOKEN(symbol)) {
                    continue;
                }
                for (int i = 0; i < *count; i++) {
                    if (strcmp(terminals[i], SYMBOL_NAME(symbol)) == 0) {
                        continue;
                    }
                }
                terminals = realloc(terminals, (*count + 1) * sizeof (char *));
                terminals[(*count) ++] = strdup(SYMBOL_NAME(symbol));
            }
        }
    }
    terminals[(*count) ++] = strdup(END_SYMBOL);
    return terminals;
}

/**
 * @brief 从预测分析表中根据文法和终结符查找产生式的位置
 *
 * @param parsingTable 预测分析表的指针
 * @param rule 文法的指针
 * @param terminal 终结符
 * @return Symbol** 如果存在则返回该产生式在预测分析表中的位置，否则返回 NULL
 */
Production **FindProduction(const ParsingTable *parsingTable,
                            const Rule *rule, const char *terminal) {
    int i;
    for (i = 0; i < parsingTable->colCount; i++) {
        if (strcmp(parsingTable->tableHead[i], terminal) == 0) {
            break;
        }
    }
    for (int j = 0; j < parsingTable->rowCount; j++) {
        if (PARSINGTABLE_ROW(parsingTable, j).rule == rule) {
            return PARSINGTABLE_ROW(parsingTable, j).productions + i;
        }
    }
    return NULL;
}

/**
 * @brief 输出产生式
 *
 * @param production 产生式的指针
 */
void PrintProduction(const Production *production) {
    for (Symbol *symbol = SYMBOL_HEAD(production); symbol != NULL; symbol = symbol->next) {
        printf(SYMBOL_NAME(symbol));
    }
}

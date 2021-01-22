#include "parsingtable.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief 生成 Select 集
 *
 * @param ruleHead 文法链表的头指针
 * @param voidTable 空表的指针
 * @param firstSetList First 集的指针
 * @param followSetList Follow 集的指针
 * @return SelectSetList* 生成的 Select 集的指针
 */
SelectSetList *GenSelectSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const SetList *firstSetList,
                                const SetList *followSetList) {
    SelectSetList *selectSetList = malloc(sizeof(SelectSetList));
    selectSetList->setCount = 0;

    for (; ruleHead != NULL; ruleHead = ruleHead->next) {
        for (Production *production = PRODUCTIONHEAD(ruleHead);
             production != NULL; production = production->next) {

            /**
             * @brief Select 集中的每个子集对应文法中的每一条产生式
             * 首先将这条产生式对应的 First 集中的符号加入到 Select 子集中
             */
            AddOneSelectSet(selectSetList, ruleHead, production);
            SelectSet *pSelectSet =
                selectSetList->sets + selectSetList->setCount - 1;
            for (Symbol *symbol = SYMBOLHEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                if (ISTOKEN(symbol)) {
                    AddTerminalToSelectSet(pSelectSet, SYMBOLNAME(symbol));
                    if (strcmp(SYMBOLNAME(symbol), VOID_SYMBOL) != 0) {
                        break;
                    }
                } else {
                    AddSetToSelectSet(
                        pSelectSet, GetSet(firstSetList, SYMBOLNAME(symbol)));
                    if (!*FindHasVoid(voidTable, SYMBOLNAME(symbol))) {
                        break;
                    }
                }
            }

            /**
             * @brief 如果该产生式对应的 First 子集中包含 ε
             * 则去除 ε，并且向其中加入文法对应的 Follow 子集中的符号
             */
            if (RemoveVoidFromSelectSet(pSelectSet)) {
                AddSetToSelectSet(pSelectSet,
                                  GetSet(followSetList, RULENAME(ruleHead)));
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
                              const SelectSetList *selectSetList) {
    ParsingTable *parsingTable = calloc(1, sizeof(ParsingTable));

    parsingTable->tableHead = GetTerminals(ruleHead);
    for (parsingTable->colCount = 0;
         parsingTable->tableHead[++parsingTable->colCount];)
        ;
    for (int i = 0; ruleHead != NULL; ruleHead = ruleHead->next, i++) {
        parsingTable->tableRows[i].rule = ruleHead;
    }
    for (int i = 0; i < selectSetList->setCount; i++) {
        for (int j = 0; j < selectSetList->sets[i].terminalCount; j++) {
            Production **pFindProduction =
                FindProduction(parsingTable, selectSetList->sets[i].rule,
                               selectSetList->sets[i].terminals[j]);
            if (*pFindProduction) {
                puts("该文法不是 LL(1) 文法！");
            } else {
                *pFindProduction = selectSetList->sets[i].production;
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
        printf("Select(%s->", RULENAME(selectSetList->sets[i].rule));
        PrintProduction(selectSetList->sets[i].production);
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
    for (int i = 0; parsingTable->tableRows[i].rule; i++) {
        printf("%s\t", RULENAME(parsingTable->tableRows[i].rule));
        for (int j = 0; j < parsingTable->colCount; j++) {
            if (parsingTable->tableRows[i].productions[j]) {
                PrintProduction(parsingTable->tableRows[i].productions[j]);
            }
            putchar(j == parsingTable->colCount - 1 ? '\n' : '\t');
        }
    }
}

/**
 * @brief 从 Select 子集中删除 $
 *
 * @param pSelectSet Select 子集指针
 * @return int 对子集是否有修改
 */
int RemoveVoidFromSelectSet(SelectSet *pSelectSet) {
    for (int i = 0; i < pSelectSet->terminalCount; i++) {
        if (strcmp(pSelectSet->terminals[i], VOID_SYMBOL) == 0) {
            if (i != pSelectSet->terminalCount - 1) {
                memmove(pSelectSet->terminals + i,
                        pSelectSet->terminals + i + 1,
                        sizeof(pSelectSet->terminals[0]) *
                            (pSelectSet->terminalCount - i - 1));
            }
            pSelectSet->terminalCount--;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 在 Select 集中查找一个 Select 子集
 *
 * @param setList Select 集指针
 * @param production 产生式的指针
 * @return SelectSet* 如果找到则返回子集的指针，否则返回 NULL
 */
SelectSet *GetSelectSet(const SelectSetList *setList,
                        const Production *production) {
    for (int i = 0; i < setList->setCount; i++) {
        if (setList->sets[i].production == production) {
            return setList->sets + i;
        }
    }
    return NULL;
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
    for (int i = 0; i < setList->setCount; i++) {
        if (setList->sets[i].production == production) {
            return;
        }
    }
    setList->sets[setList->setCount].terminalCount = 0;
    setList->sets[setList->setCount].rule = rule;
    setList->sets[setList->setCount++].production = production;
}

/**
 * @brief 添加一个终结符到 Select 子集
 *
 * @param set Select 子集的指针
 * @param terminal 终结符
 * @return int 是否对子集有修改
 */
int AddTerminalToSelectSet(SelectSet *set, const char *terminal) {
    for (int i = 0; i < set->terminalCount; i++) {
        if (strcmp(set->terminals[i], terminal) == 0) {
            return 0;
        }
    }
    set->terminals[set->terminalCount++] = strdup(terminal);
    return 1;
}

/**
 * @brief 将源 First 子集或 Follow 子集中的所有终结符添加至目标 Select 子集中
 *
 * @param desSet 目标 Select 子集
 * @param srcSet 源 First 子集或 Follow 子集
 * @return int 是否对目标 Select 子集有修改
 */
int AddSetToSelectSet(SelectSet *desSet, const Set *srcSet) {
    int flag = 0;
    for (int i = 0; i < srcSet->terminalCount; i++) {
        if (AddTerminalToSelectSet(desSet, srcSet->terminals[i])) {
            flag = 1;
        }
    }
    return flag;
}

/**
 * @brief 从文法中提取所有的终结符
 *
 * @param ruleHead 文法链表的头指针
 * @return char** 终结符数组
 */
char **GetTerminals(const Rule *ruleHead) {
    char **terminals = calloc(32, sizeof(char *));
    int t;
    for (; ruleHead != NULL; ruleHead = ruleHead->next) {
        for (Production *production = PRODUCTIONHEAD(ruleHead);
             production != NULL; production = production->next) {
            for (Symbol *symbol = SYMBOLHEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                if (!ISTOKEN(symbol)) {
                    continue;
                }
                for (t = 0; terminals[t] != NULL; t++) {
                    if (strcmp(terminals[t], SYMBOLNAME(symbol)) == 0) {
                        break;
                    }
                }
                if (terminals[t] == NULL) {
                    terminals[t] = strdup(SYMBOLNAME(symbol));
                }
            }
        }
    }
    terminals[t + 1] = END_SYMBOL;
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
    for (int j = 0; parsingTable->tableRows[j].rule; j++) {
        if (parsingTable->tableRows[j].rule == rule) {
            return parsingTable->tableRows[j].productions + i;
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
    for (Symbol *symbol = SYMBOLHEAD(production); symbol != NULL; symbol = symbol->next) {
        printf(SYMBOLNAME(symbol));
    }
}

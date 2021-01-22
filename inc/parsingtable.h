#ifndef PARSINGTABLE_H_
#define PARSINGTABLE_H_

#include "first.h"
#include "rule.h"

typedef struct {
    int colCount;      // 列宽
    char **tableHead; // 表头
    struct TableRows {
        Rule *rule;                 // 行首的文法
        Production *productions[32]; // 行内的产生式
    } tableRows[32];                 // 行
} ParsingTable;

typedef struct {
    Rule *rule;                        // 文法
    Production *production;            // 产生式
    char *terminals[32]; // 终结符数组
    int terminalCount;                 // 终结符个数
} SelectSet;

typedef struct {
    SelectSet sets[32]; // Select 子集
    int setCount;      // 子集数量
} SelectSetList;

SelectSet *GetSelectSet(const SelectSetList *setList,
                        const Production *production);
void AddOneSelectSet(SelectSetList *setList, const Rule *rule,
                     const Production *production);
int AddTerminalToSelectSet(SelectSet *set, const char *terminal);
int AddSetToSelectSet(SelectSet *desSet, const Set *srcSet);
int RemoveVoidFromSelectSet(SelectSet *set);
SelectSetList *GenSelectSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const SetList *firstSetList,
                                const SetList *followSetList);
ParsingTable *GenParsingTable(const Rule *ruleHead,
                              const SelectSetList *productionSetList);
Production **FindProduction(const ParsingTable *parsingTable,
                            const Rule *rule, const char *terminal);
void PrintSelectSetList(const SelectSetList *productionSetList);
void PrintParsingTable(const ParsingTable *parsingTable);
char **GetTerminals(const Rule *ruleHead);
void PrintProduction(const Production *production);

#endif

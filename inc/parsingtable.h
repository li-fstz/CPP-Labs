#ifndef PARSINGTABLE_H_
#define PARSINGTABLE_H_

#include "first.h"
#include "rule.h"

typedef struct {
    int ColCount;      // 列宽
    char **pTableHead; // 表头
    struct {
        Rule *pRule;                 // 行首的文法
        Production *Productions[32]; // 行内的产生式
    } TableRows[32];                 // 行
} ParsingTable;

typedef struct {
    Rule *pRule;                        // 文法
    Production *pProduction;            // 产生式
    char Terminals[32][MAX_STR_LENGTH]; // 终结符数组
    int nTerminalCount;                 // 终结符个数
} SelectSet;

typedef struct {
    SelectSet Sets[32]; // Select 子集
    int nSetCount;      // 子集数量
} SelectSetList;

SelectSet *GetSelectSet(const SelectSetList *pSetList,
                        const Production *pProduction);
void AddOneSelectSet(SelectSetList *pSetList, const Rule *pRule,
                     const Production *pProduction);
int AddTerminalToSelectSet(SelectSet *pSet, const char *pTerminal);
int AddSetToSelectSet(SelectSet *pDesSet, const Set *pSrcSet);
int RemoveVoidFromSelectSet(SelectSet *pSet);
void GenSelectSetList(const Rule *pRuleHead, const VoidTable *pVoidTable,
                      const SetList *pFirstSetList,
                      const SetList *pFollowSetList,
                      SelectSetList *pSelectSetList);
void GenParsingTable(const Rule *pRuleHead,
                     const SelectSetList *pProductionSetList,
                     ParsingTable *pTable);
Production **FindProduction(const ParsingTable *pParsingTable,
                            const Rule *pRule, const char *Terminal);
void PrintSelectSetList(const SelectSetList *pProductionSetList);
void PrintParsingTable(const ParsingTable *PrintParsingTable);
char **GetTerminals(const Rule *pRuleHead);
void PrintProduction(const Production *pProduction);

#endif

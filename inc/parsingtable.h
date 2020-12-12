#ifndef PARSINGTABLE_H_
#define PARSINGTABLE_H_

#include "first.h"
#include "rule.h"

typedef struct {
    int ColCount;
    char **pTableHead;
    struct {
        Rule *pRule;                    // 指向下一个 Symbol
        struct Production *Productions[32]; // 指向下一个 Select
    } TableRows[32];
} ParsingTable;

typedef struct {
    Rule *pRule;
    Production *pProduction;            // 集合的名称
    char Terminals[32][MAX_STR_LENGTH]; // 终结符数组
    int nTerminalCount;                 // 数组元素个数
} SelectSet;

typedef struct {
    SelectSet Sets[32]; // 集合数组
    int nSetCount;      // 数组元素个数
} SelectSetList;

SelectSet *GetSelectSet(SelectSetList *pSetList, const Production *pProduction);
void AddOneSelectSet(SelectSetList *pSetList, const Rule *pRule,
                     const Production *pProduction);
int AddTerminalToSelectSet(SelectSet *pSet, const char *pTerminal);
int AddSetToSelectSet(SelectSet *pDesSet, const Set *pSrcSet);
int RemoveVoidFromSelectSet(SelectSet *pSet);
void GenSelectSetList(Rule *pRuleHead, VoidTable *pVoidTable,
                      SetList *pFirstSetList, SetList *pFollowSetList,
                      SelectSetList *pSelectSetList);
void GenParsingTable(Rule *pRuleHead, SelectSetList *pProductionSetList,
                     ParsingTable *pTable);
Production **FindProduction(ParsingTable *pParsingTable, Rule *pRule,
                            const char *Terminal);
void PrintSelectSetList(SelectSetList *pProductionSetList);
void PrintParsingTable(ParsingTable *PrintParsingTable);
char **GetTerminals(Rule *pRuleHead);
void PrintProduction(Production *pProduction);

#endif /* TableRow_H_ */

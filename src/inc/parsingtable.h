#ifndef PARSINGTABLE_H_
#define PARSINGTABLE_H_
#include "first.h"
#include "rule.h"

typedef struct _ParsingTable {
    int ColCount;
    char **pTableHead;
    struct _ParsingTableRow {
        struct _Rule *pRule;       // 指向下一个 Symbol
        struct _RuleSymbol *Select[32];  // 指向下一个 Select
    } TableRows[32];
} ParsingTable;

typedef struct _SelectSet {
    struct _Rule *pRule;
    struct _RuleSymbol *pSelect;        // 集合的名称
    char Terminal[32][MAX_STR_LENGTH];  // 终结符数组
    int nTerminalCount;                 // 数组元素个数
} SelectSet;

typedef struct _SelectSetList {
    SelectSet Sets[32];  // 集合数组
    int nSetCount;       // 数组元素个数
} SelectSetList;

SelectSet *GetSelectSet(SelectSetList *pSetList, const RuleSymbol *pSelect);
void AddOneSelectSet(SelectSetList *pSetList, const Rule *pRule,
                     const RuleSymbol *pSelect);
int AddTerminalToSelectSet(SelectSet *pSet, const char *pTerminal);
int AddSetToSelectSet(SelectSet *pDesSet, const Set *pSrcSet);

int RemoveVoidFromSelectSet(SelectSet *pSet);
void Select(Rule *pRule, VoidTable *pVoidTable, SetList *pFirstSetList,
            SetList *pFollowSetList, SelectSetList *pSelectSetList);

void GenParsingTable(Rule *pRule, ParsingTable *pTable,
                     SelectSetList *pSelectSetList);
RuleSymbol **FindSelect(ParsingTable *pParsingTable, Rule *pRule,
                       const char *Terminal);
void PrintSelectSet(SelectSetList *pSelectSetList);

void PrintParsingTable(ParsingTable *PrintParsingTable);

char **GetTerminals(Rule *pRule);

void PrintSelect(RuleSymbol *pSelect);
#endif /* TableRow_H_ */

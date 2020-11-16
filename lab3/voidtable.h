#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

#include "rule.h"

typedef struct _VoidTable {
    int ColCount;
    char **pTableHead;
    struct _VoidTableRow{
        int hasVoid[32];
    } TableRows[1];
} VoidTable;

//
// 在此处声明函数
//

void GenVoidTable(Rule *pRuleHead, VoidTable *pTable);
int *FindHasVoid(VoidTable *pTable, const char *RuleName);
void PrintTable(VoidTable *pTable);
char **GetNonTerminals(const Rule *pRule);
#endif /* VOIDTABLE_H_ */

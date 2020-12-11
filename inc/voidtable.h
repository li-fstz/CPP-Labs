#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

#include "rule.h"

typedef struct {
    int ColCount;
    char **pTableHead;
    struct {
        int hasVoid[32];
    } TableRows[1];
} VoidTable;

void GenVoidTable(Rule *pRuleHead, VoidTable *pTable);
int *FindHasVoid(VoidTable *pTable, const char *RuleName);
void PrintVoidTable(VoidTable *pTable);
char **GetNonTerminals(const Rule *pRule);

#endif /* VOIDTABLE_H_ */

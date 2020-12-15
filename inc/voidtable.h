#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

#include "rule.h"

typedef struct {
    int ColCount;      // 列宽
    char **pTableHead; // 表头
    struct {
        int hasVoid[32]; // 是否为空
    } TableRows[1];
} VoidTable;

VoidTable *GenVoidTable(const Rule *pRuleHead);
int *FindHasVoid(VoidTable *pTable, const char *RuleName);
void PrintVoidTable(const VoidTable *pTable);
char **GetNonTerminals(const Rule *pRuleHead);

#endif

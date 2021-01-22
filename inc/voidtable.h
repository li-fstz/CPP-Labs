#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

#include "rule.h"

typedef struct {
    int colCount;      // 列宽
    char **tableHead; // 表头
    struct {
        int hasVoid[32]; // 是否为空
    } tableRows[1];
} VoidTable;

VoidTable *GenVoidTable(const Rule *ruleHead);
int *FindHasVoid(VoidTable *table, const char *ruleName);
void PrintVoidTable(const VoidTable *table);
char **GetNonTerminals(const Rule *ruleHead);

#endif

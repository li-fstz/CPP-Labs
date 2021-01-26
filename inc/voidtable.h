#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

#include "rule.h"

#define VOIDTABLE_ROW(t,i) (((struct VoidTableRow *)t->tableRows)[i])

struct Table {
    int colCount;      // 列宽
    int rowCount;
    char **tableHead; // 表头
    void *tableRows;
};
struct VoidTableRow {
    int *hasVoid;
};
typedef struct Table Table, VoidTable;
VoidTable *GenVoidTable(const Rule *ruleHead);
int *FindHasVoid(VoidTable *table, const char *ruleName);
void PrintVoidTable(const VoidTable *table);
char **GetNonTerminals(const Rule *ruleHead, int *count);
#endif

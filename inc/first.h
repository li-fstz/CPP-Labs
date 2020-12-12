#ifndef FIRST_H_
#define FIRST_H_

#include "rule.h"
#include "voidtable.h"
#include <stdio.h>

typedef struct {
    char Name[MAX_STR_LENGTH];          // 集合的名称
    char Terminals[32][MAX_STR_LENGTH]; // 终结符数组
    int nTerminalCount;                 // 数组元素个数
} Set;

typedef struct {
    Set Sets[32];  // 集合数组
    int nSetCount; // 数组元素个数
} SetList;

void GenFirstSetList(const Rule *pRuleHead, VoidTable *pVoidTable,
                 SetList *pFirstSetList);
Set *GetSet(SetList *pSetList, const char *pName);
void AddOneSet(SetList *pSetList, const char *pName);
int AddTerminalToSet(Set *pSet, const char *pTerminal);
int AddSetToSet(Set *pDesSet, const Set *pSrcSet);
void PrintFirstSetList(SetList *pFirstSetList);

#endif /* FIRST_H_ */

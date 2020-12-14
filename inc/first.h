#ifndef FIRST_H_
#define FIRST_H_

#include "rule.h"
#include "voidtable.h"
#include <stdio.h>

typedef struct {
    char Name[MAX_STR_LENGTH];          // 子集的名称
    char Terminals[32][MAX_STR_LENGTH]; // 终结符数组
    int nTerminalCount;                 // 终结符个数
} Set;

typedef struct {
    Set Sets[32];  // 子集数组
    int nSetCount; // 子集个数
} SetList;

void GenFirstSetList(const Rule *pRuleHead, const VoidTable *pVoidTable,
                     SetList *pFirstSetList);
Set *GetSet(const SetList *pSetList, const char *pName);
void AddOneSet(SetList *pSetList, const char *pName);
int AddTerminalToSet(Set *pSet, const char *pTerminal);
int AddSetToSet(Set *pDesSet, const Set *pSrcSet);
void PrintFirstSetList(const SetList *pFirstSetList);

#endif

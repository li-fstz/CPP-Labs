#ifndef FIRST_H_
#define FIRST_H_

#include "rule.h"
#include "voidtable.h"
#include <stdio.h>

typedef struct {
    char *name;          // 子集的名称
    char *terminals[32]; // 终结符数组
    int terminalCount;                 // 终结符个数
} Set;

typedef struct {
    Set sets[32];  // 子集数组
    int setCount; // 子集个数
} SetList;

SetList *GenFirstSetList(const Rule *ruleHead, const VoidTable *voidTable);
Set *GetSet(const SetList *setList, const char *name);
void AddOneSet(SetList *setList, const char *name);
int AddTerminalToSet(Set *set, const char *terminal);
int AddSetToSet(Set *desSet, const Set *srcSet);
void PrintFirstSetList(const SetList *firstSetList);

#endif

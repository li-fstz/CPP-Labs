#ifndef FIRST_H_
#define FIRST_H_

#include "rule.h"
#include "voidtable.h"

#define IS_FIRST_SET(s) ((s) && (s)->type == FirstSet)

typedef enum { FirstSet, FollowSet, SelectSet } SetType;

typedef struct {
    const void *key;
    char **terminals;  // 终结符数组
    int terminalCount; // 终结符个数
} Set;

struct SetList {
    Set *sets;    // 子集数组
    int setCount; // 子集个数
    SetType type;
};

typedef struct SetList SetList, FirstSetList;
typedef int (*CmpFunc)(const void *a, const void *b);

int strKeyCmp(const void *a, const void *b);
FirstSetList *GenFirstSetList(const Rule *ruleHead, const VoidTable *voidTable);
Set *GetSet(const SetList *setList, const void *key, CmpFunc cmp);
void AddOneSet(SetList *setList, const void *key, CmpFunc cmp);
int AddTerminalToSet(Set *set, const char *terminal);
int AddSetToSet(Set *desSet, const Set *srcSet);
void PrintFirstSetList(const FirstSetList *firstSetList);

#endif

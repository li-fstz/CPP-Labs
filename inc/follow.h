#ifndef FOLLOW_H_
#define FOLLOW_H_

#define END_SYMBOL "#"

#include "first.h"
#include "rule.h"
#include "voidtable.h"

#define IS_FOLLOW_SET(s) ((s) && (s)->type == FollowSet)

typedef struct SetList FollowSetList;

FollowSetList *GenFollowSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const FirstSetList *firstSetList);
void PrintFollowSetList(const FollowSetList *followSetList);
int RemoveVoidFromSet(Set *set);

#endif

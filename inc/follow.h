#ifndef FOLLOW_H_
#define FOLLOW_H_

#define END_SYMBOL "#"

#include "first.h"
#include "rule.h"
#include "voidtable.h"

SetList *GenFollowSetList(const Rule *pRuleHead, const VoidTable *pVoidTable,
                          const SetList *pFirstSetList);
void PrintFollowSetList(const SetList *pFollowSetList);
void RemoveVoidFromSet(Set *pSet);

#endif

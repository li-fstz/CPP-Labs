#ifndef FOLLOW_H_
#define FOLLOW_H_

#define END_SYMBOL "#"

#include "first.h"
#include "rule.h"
#include "voidtable.h"

void GenFollowSetList(const Rule *pRuleHead, const VoidTable *pVoidTable,
                      const SetList *pFirstSetList, SetList *pFollowSetList);
void PrintFollowSetList(const SetList *pFollowSetList);
void RemoveVoidFromSet(Set *pSet);

#endif

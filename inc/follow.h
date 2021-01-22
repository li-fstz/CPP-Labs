#ifndef FOLLOW_H_
#define FOLLOW_H_

#define END_SYMBOL "#"

#include "first.h"
#include "rule.h"
#include "voidtable.h"

SetList *GenFollowSetList(const Rule *ruleHead, const VoidTable *voidTable,
                          const SetList *firstSetList);
void PrintFollowSetList(const SetList *followSetList);
void RemoveVoidFromSet(Set *set);

#endif

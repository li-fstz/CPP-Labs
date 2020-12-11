#ifndef FOLLOW_H_
#define FOLLOW_H_

#define END_SYMBOL "#"

#include "first.h"
#include "rule.h"
#include "voidtable.h"

void GenFollowSet(const Rule *pRuleHead, VoidTable *pVoidTable,
                  SetList *pFollowSetList, SetList *pFirstSetList);
void PrintFollowSet(SetList *pFollowSetList);
void RemoveVoidFromSet(Set *pSet);

#endif /* FOLLOW_H_ */

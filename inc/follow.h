#ifndef FOLLOW_H_
#define FOLLOW_H_

#define END_SYMBOL "#"

#include "first.h"
#include "rule.h"
#include "voidtable.h"

void GenFollowSetList(const Rule *pRuleHead, VoidTable *pVoidTable,
                      SetList *pFirstSetList, SetList *pFollowSetList);
void PrintFollowSetList(SetList *pFollowSetList);
void RemoveVoidFromSet(Set *pSet);

#endif /* FOLLOW_H_ */

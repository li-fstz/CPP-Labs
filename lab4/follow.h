#ifndef FOLLOW_H_
#define FOLLOW_H_
#include "first.h"
#include "rule.h"
#include "voidtable.h"
//
// 在此处包含 C 标准库头文件
//

void Follow(const Rule *pHead, VoidTable *pVoidTable, SetList *pFollowSetList,
            SetList *pFirstSetList);
void PrintFollowSet(SetList *pFollowSetList);
void RemoveVoidFromSet(Set *pSet);
//
// 在此声明全局变量
//

extern const char *EndSymbol;

#endif /* FOLLOW_H_ */

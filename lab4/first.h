#ifndef FIRST_H_
#define FIRST_H_

//
// 在此处包含 C 标准库头文件
//
#include "rule.h"
#include "voidtable.h"
#include <stdio.h>

typedef struct _Set {
    char Name[MAX_STR_LENGTH];          // 集合的名称
    char Terminal[32][MAX_STR_LENGTH];  // 终结符数组
    int nTerminalCount;                 // 数组元素个数
} Set;

typedef struct _SetList {
    Set Sets[32];   // 集合数组
    int nSetCount;  // 数组元素个数
} SetList;

//
// 在此处声明函数
//

void First(const Rule *pHead, VoidTable *pVoidTable, SetList *pFirstSetList);
Set *GetSet(SetList *pSetList, const char *pName);
void AddOneSet(SetList *pSetList, const char *pName);
int AddTerminalToSet(Set *pSet, const char *pTerminal);
int AddSetToSet(Set *pDesSet, const Set *pSrcSet);

void PrintFirstSet(SetList *pFirstSetList);

//
// 在此声明全局变量
//

extern const char *VoidSymbol;

#endif /* FIRST_H_ */

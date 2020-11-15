#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

#include "rule.h"

typedef struct _Column {
    char RuleName[MAX_STR_LENGTH];  // 该非终结符的名称
    struct _Column *pNextCol;       // 指向下一列
    int hasVoid;  // 该非终结符是否能推出 ε。1 表示能推出 ε，0 表示不能推出 ε，
                  // -1 表示未知
} Column;

//
// 在此处声明函数
//

Column *InitVoidTable(Rule *pRule);
void VoidTable(Rule *pRuleHead, Column *pTableHead);
Column *FindColumn(Column *pTableHead, const char *RuleName);
void PrintTable(Column *pHead);

#endif /* VOIDTABLE_H_ */

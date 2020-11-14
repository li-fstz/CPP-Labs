#ifndef VOIDTABLE_H_
#define VOIDTABLE_H_

//
// 在此处包含 C 标准库头文件
//

#include <stdio.h>

//
// 在此处包含其他头文件
//

//
// 在此处定义数据结构
//

#define MAX_STR_LENGTH 64

struct _Rule;
typedef struct _RuleSymbol {
    struct _RuleSymbol *pNextSymbol;  // 指向下一个 Symbol
    struct _RuleSymbol *pOther;       // 指向下一个 Select
    int isToken;  // 是否为终结符。1 表示终结符，0 表示非终结符
    char SymbolName[MAX_STR_LENGTH];  // 终结符和非终结符的名称
    struct _Rule *pRule;  // 指向 Symbol 对应的 Rule。isToken 为 0 时这个域有效
} RuleSymbol;

typedef struct _Rule {
    char RuleName[MAX_STR_LENGTH];  // 文法的名称
    struct _RuleSymbol
        *pFirstSymbol;  // 指向文法的第一个 Select 的第一个 Symbol
    struct _Rule *pNextRule;  // 指向下一条文法
} Rule;

typedef struct _Column {
    char RuleName[MAX_STR_LENGTH];  // 该非终结符的名称
    struct _Column *pNextCol;       // 指向下一列
    int hasVoid;  // 该非终结符是否能推出 ε。1 表示能推出 ε，0 表示不能推出 ε，
                  // -1 表示未知
} Column;

//
// 在此处声明函数
//

Rule *InitRules();
Rule *CreateRule(const char *pRuleName);
RuleSymbol *CreateSymbol();
Rule *FindRule(Rule *pHead, const char *RuleName);
Column *InitVoidTable(Rule *pRule);

void VoidTable(Rule *pRuleHead, Column *pTableHead);
Column *FindColumn(Column *pTableHead, char *RuleName);

void PrintRule(Rule *pHead);
void PrintTable(Column *pHead);
//
// 在此声明全局变量
//

extern const char *VoidSymbol;

#endif /* VOIDTABLE_H_ */

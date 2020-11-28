#ifndef REMOVELEFTRECURSIONNOREPLACE_H_
#define REMOVELEFTRECURSIONNOREPLACE_H_
#include "rule.h"
//
// 在此处包含 C 标准库头文件
//

#include <stdio.h>

//
// 在此处声明函数
//

void AddSymbolToSelect(RuleSymbol *pSelect, RuleSymbol *pNewSymbol);
void AddSelectToRule(Rule *pRule, RuleSymbol *pNewSelect);
void RemoveLeftRecursion(Rule *pHead);

//
// 在此声明全局变量
//

extern const char *Postfix;

#endif /* REMOVELEFTRECURSIONNOREPLACE_H_ */

#ifndef REMOVELEFTRECURSION_H_
#define REMOVELEFTRECURSION_H_

//
// 在此处包含 C 标准库头文件
//
#include <stdio.h>

#include "rule.h"

//
// 在此处包含其他头文件
//

//
// 在此处定义数据结构
//

int RuleHasLeftRecursion(Rule *pRule);
void AddSymbolToSelect(RuleSymbol *pSelect, RuleSymbol *pNewSymbol);
void AddSelectToRule(Rule *pRule, RuleSymbol *pNewSelect);
void FreeSelect(RuleSymbol *pSelect);
RuleSymbol *ReplaceSelect(const RuleSymbol *pSelectTemplate);
void RemoveLeftRecursion(Rule *pHead);

extern const char *Postfix;

#endif /* REMOVELEFTRECURSION_H_ */

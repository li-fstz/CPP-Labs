#ifndef PICKUPLEFTFACTOR_H_
#define PICKUPLEFTFACTOR_H_

//
// 在此处包含 C 标准库头文件
//

#include <stdio.h>

#include "rule.h"
//
// 在此处声明函数
//

RuleSymbol *GetSymbol(RuleSymbol *pSelect, int index);
int LeftFactorMaxLength(RuleSymbol *pSelectTemplate);
int SymbolCmp(RuleSymbol *pRuleSymbol1, RuleSymbol *pRuleSymbol2);
int NeedPickup(RuleSymbol *pSelectTemplate, int Count, RuleSymbol *pSelect);
void AddSelectToRule(Rule *pRule, RuleSymbol *pRuleSymbol);
void GetUniqueRuleName(Rule *pHead, char *pRuleName);
void PickupLeftFactor(Rule *pHead);
void FreeSelect(RuleSymbol *pSelect);

//
// 在此声明全局变量
//

extern const char *VoidSymbol;
extern const char *Postfix;

#endif /* PICKUPLEFTFACTOR_H_ */

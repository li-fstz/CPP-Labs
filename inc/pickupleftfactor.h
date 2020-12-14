#ifndef PICKUPLEFTFACTOR_H_
#define PICKUPLEFTFACTOR_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

Symbol *GetSymbol(Production *pProduction, int index);
int LeftFactorMaxLength(Production *pProductionTemplate);
int SymbolCmp(const Symbol *pSymbol1, const Symbol *pSymbol2);
int NeedPickup(const Production *pProductionTemplate, int Count,
               const Production *pProduction);
void AddProductionToRule(Rule *pRule, Production *pProduction);
void GetUniqueRuleName(const Rule *pRuleHead, char *pRuleName);
void PickupLeftFactor(Rule *pRuleHead);
void FreeProduction(Production *pProduction);

#endif

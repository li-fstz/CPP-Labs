#ifndef PICKUPLEFTFACTOR_H_
#define PICKUPLEFTFACTOR_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

Symbol *GetSymbol(Production *pProduction, int index);
int LeftFactorMaxLength(Production *pProductionTemplate);
int SymbolCmp(Symbol *pSymbol1, Symbol *pSymbol2);
int NeedPickup(Production *pProductionTemplate, int Count,
               Production *pProduction);
void AddProductionToRule(Rule *pRule, Production *pProduction);
void GetUniqueRuleName(Rule *pRuleHead, char *pRuleName);
void PickupLeftFactor(Rule *pRuleHead);
void FreeProduction(Production *pProduction);

#endif /* PICKUPLEFTFACTOR_H_ */

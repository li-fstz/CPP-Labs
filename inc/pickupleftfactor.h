#ifndef PICKUPLEFTFACTOR_H_
#define PICKUPLEFTFACTOR_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

Symbol *GetSymbol(Production *production, int index);
int LeftFactorMaxLength(Production *productionTemplate);
int SymbolCmp(const Symbol *symbol1, const Symbol *symbol2);
int NeedPickup(const Production *productionTemplate, int count,
               const Production *production);
void AddProductionToRule(Rule *rule, Production *production);
char *GetUniqueRuleName(const Rule *ruleHead, const char *ruleName);
void PickupLeftFactor(Rule *ruleHead);
void FreeProduction(Production *production);

#endif

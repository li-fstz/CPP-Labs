#ifndef REMOVELEFTRECURSIONNOREPLACE_H_
#define REMOVELEFTRECURSIONNOREPLACE_H_

#define POSTFIX "'"

#include "rule.h"

void AddProductionToRule(Rule *rule, const Production *newProduction);
void RemoveLeftRecursion(Rule *ruleHead);

#endif

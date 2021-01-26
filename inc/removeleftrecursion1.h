#ifndef REMOVELEFTRECURSIONNOREPLACE_H_
#define REMOVELEFTRECURSIONNOREPLACE_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

void AddProductionToRule(Rule *rule, Production *newProduction);
void RemoveLeftRecursion(Rule *ruleHead);

#endif

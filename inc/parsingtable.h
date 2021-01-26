#ifndef PARSINGTABLE_H_
#define PARSINGTABLE_H_

#include "first.h"
#include "rule.h"

#define PARSINGTABLE_ROW(t,i) (((struct ParsingTableRow *)t->tableRows)[i])
#define PRODUCTION_KEY(s) (((struct SelectSetKey *)s.key)->production)
#define RULE_KEY(s) (((struct SelectSetKey *)s.key)->rule)

typedef struct SetList SelectSetList;

struct ParsingTableRow {
    Rule *rule;                 // 行首的文法
    Production **productions; // 行内的产生式
};

typedef struct Table ParsingTable;

struct SelectSetKey {
    Rule *rule;                        // 文法
    Production *production;            // 产生式
};

void AddOneSelectSet(SetList *setList, const Rule *rule,
                     const Production *production);
SetList *GenSelectSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const SetList *firstSetList,
                                const SetList *followSetList);
ParsingTable *GenParsingTable(const Rule *ruleHead,
                              const SetList *productionSetList);
Production **FindProduction(const ParsingTable *parsingTable,
                            const Rule *rule, const char *terminal);
void PrintSelectSetList(const SetList *productionSetList);
void PrintParsingTable(const ParsingTable *parsingTable);
char **GetTerminals(const Rule *ruleHead, int *count);
void PrintProduction(const Production *production);

#endif

#ifndef PARSINGTABLE_H_
#define PARSINGTABLE_H_

#include "first.h"
#include "follow.h"
#include "rule.h"

#define PARSINGTABLE_ROW(t, i) (((struct ParsingTableRow *)t->tableRows)[i])
#define PRODUCTION_KEY(s) (((struct SelectSetKey *)(s)->key)->production)
#define RULE_KEY(s) (((struct SelectSetKey *)(s)->key)->rule)
#define IS_PARSING_TABLE(t) ((t) && (t)->type == Parsing)
#define IS_SELECT_SET(s) ((s) && (s)->type == SelectSet)

struct ParsingTableRow {
    const Rule *rule;               // 行首的文法
    const Production **productions; // 行内的产生式
};

struct SelectSetKey {
    const Rule *rule;             // 文法
    const Production *production; // 产生式
};

typedef struct Table ParsingTable;
typedef struct SetList SelectSetList;

void AddOneSelectSet(SetList *setList, const Rule *rule,
                     const Production *production);
SelectSetList *GenSelectSetList(const Rule *ruleHead,
                                const VoidTable *voidTable,
                                const FirstSetList *firstSetList,
                                const FollowSetList *followSetList);
ParsingTable *GenParsingTable(const Rule *ruleHead,
                              const SelectSetList *selectSetList);
const Production **FindProduction(const ParsingTable *parsingTable, const Rule *rule,
                            const char *terminal);
void PrintSelectSetList(const SelectSetList *selectSetList);
void PrintParsingTable(const ParsingTable *parsingTable);
char **GetTerminals(const Rule *ruleHead, int *count);
void PrintProduction(const Production *production);

#endif

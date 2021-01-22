#ifndef RULE_H_
#define RULE_H_

#define MAX_STR_LENGTH 64
#define VOID_SYMBOL "$"

#define R(r) ((r)->rule)
#define P(p) ((p)->production)
#define S(s) ((s)->symbol)
#define ISTOKEN(s) (S(s).rule == NULL)
#define RULENAME(r) (R(r).ruleName)
#define SYMBOLNAME(s) (S(s).symbolName)
#define PRODUCTIONHEAD(r) (R(r).productionHead)
#define SYMBOLHEAD(p) (P(p).symbolHead)
#define RULE(s) (S(s).rule)

typedef struct LinkedNode LinkedNode, Rule, Production, Symbol;

struct LinkedNode {
    union {
        struct Rule rule;
        struct Production production;
        struct Symbol symbol;
    };
    LinkedNode *next;
};

struct Symbol {
    char *symbolName;
    Rule *rule;
};

struct Production {
    Symbol *symbolHead;
};

struct Rule {
    char *ruleName;
    Production *productionHead;
};

struct RULE_ENTRY {
    char ruleName[MAX_STR_LENGTH];
    struct {
        int isToken;
        char symbolName[MAX_STR_LENGTH];
    } productions[64][64];
};

Rule *InitRules(const struct RULE_ENTRY *ruleTable, int ruleCount);
Rule *NewRule(const char *ruleName);
Symbol *NewSymbol(const char *symbolName);
Production *NewProduction();
const Rule *FindRule(const Rule *ruleHead, const char *ruleName);
void PrintRule(const Rule *ruleHead);
Symbol *CopySymbol(const Symbol *symbolTemplate);
Production *CopyProduction(const Production *productionTemplate);
Rule *CopyRule(const Rule *ruleHead);

LinkedNode *Delete(LinkedNode *head, LinkedNode *node);
LinkedNode *Append(LinkedNode *head, const LinkedNode *node);
LinkedNode *NewNode();
#endif

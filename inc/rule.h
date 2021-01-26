#ifndef RULE_H_
#define RULE_H_

#include <assert.h>

#define MAX_STR_LENGTH 64
#define VOID_SYMBOL "$"

#define R(r) ((struct Rule *)(r)->value)
#define P(p) ((struct Production *)(p)->value)
#define S(s) ((struct Symbol *)(s)->value)
#define IS_TOKEN(s) (S(s)->rule == NULL)
#define RULE_NAME(r) (R(r)->ruleName)
#define SYMBOL_NAME(s) (S(s)->symbolName)
#define PRODUCTION_HEAD(r) (R(r)->productionHead)
#define SYMBOL_HEAD(p) (P(p)->symbolHead)
#define RULE(s) (S(s)->rule)
#define IS_RULE(r) ((r) && (r)->type == RuleNode)
#define IS_PRODUCTION(r) ((r) && (r)->type == ProductionNode)
#define IS_SYMBOL(r) ((r) && (r)->type == SymbolNode)


typedef struct LinkedNode LinkedNode, Rule, Production, Symbol;

typedef enum NodeType {
    RuleNode,
    ProductionNode,
    SymbolNode
} NodeType;

struct LinkedNode {
    void *value;
    LinkedNode *next;
    NodeType type;
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
Production *CopyProductions(const Production *productionTemplate);
Production *CopyProduction(const Production *productionTemplate);
Rule *CopyRule(const Rule *ruleHead);
Rule *CopyRules(const Rule *ruleHeadTemplate);
LinkedNode *DeleteNode(LinkedNode *head, LinkedNode *node);
LinkedNode *AppendNode(LinkedNode *head, const LinkedNode *node);
LinkedNode *InsertNode(LinkedNode *head, const LinkedNode *destNode, LinkedNode *node);
LinkedNode *NewNode();
#endif

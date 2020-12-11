#ifndef RULE_H_
#define RULE_H_

#define MAX_STR_LENGTH 64
#define VOID_SYMBOL "$"

typedef struct Rule Rule;
typedef struct Symbol Symbol;
typedef struct Symbol Production;

struct Symbol {
    Symbol *pNextSymbol;         // 指向下一个 Symbol
    Production *pNextProduction; // 指向下一个 Production
    int isToken; // 是否为终结符。1 表示终结符，0 表示非终结符
    char SymbolName[MAX_STR_LENGTH]; // 终结符和非终结符的名称
    Rule *pRule; // 指向 Symbol 对应的 Rule。isToken 为 0 时这个域有效
};
struct Rule {
    char RuleName[MAX_STR_LENGTH]; // 文法的名称
    Production *pFirstProduction; // 指向文法的第一个 Production 的第一个 Symbol
    Rule *pNextRule;              // 指向下一条文法
};

struct SYMBOL {
    int isToken;
    char Name[MAX_STR_LENGTH];
};

struct RULE_ENTRY {
    char RuleName[MAX_STR_LENGTH];
    struct SYMBOL Productions[64][64];
};

Rule *InitRules(const struct RULE_ENTRY *rule_table, int nRuleCount);
Rule *CreateRule(const char *pRuleName);
Symbol *CreateSymbol();
Rule *FindRule(Rule *pRuleHead, const char *RuleName);
void PrintRule(Rule *pRuleHead);
Symbol *CopySymbol(const Symbol *pSymbolTemplate);
Symbol *CopyProduction(const Production *pProductionTemplate);
Rule *CopyRule(Rule *pRuleHead);

#endif /* RULE_H_ */

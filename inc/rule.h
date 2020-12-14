#ifndef RULE_H_
#define RULE_H_

#define MAX_STR_LENGTH 64
#define VOID_SYMBOL "$"

typedef struct Rule Rule;
typedef struct Symbol Symbol, Production;

struct Symbol {
    Symbol *pNextSymbol;         // 指向下一个符号
    Production *pNextProduction; // 指向下一个产生式
    int isToken; // 是否为终结符。1 表示终结符，0 表示非终结符
    char SymbolName[MAX_STR_LENGTH]; // 终结符和非终结符的名称
    Rule *pRule; // 指向符号对应的文法。isToken 为 0 时这个域有效
};

struct Rule {
    char RuleName[MAX_STR_LENGTH]; // 文法的名称
    Production *pFirstProduction;  // 指向文法的第一个产生式
    Rule *pNextRule;               // 指向下一条文法
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
Rule *FindRule(const Rule *pRuleHead, const char *RuleName);
void PrintRule(const Rule *pRuleHead);
Symbol *CopySymbol(const Symbol *pSymbolTemplate);
Production *CopyProduction(const Production *pProductionTemplate);
Rule *CopyRule(const Rule *pRuleHead);

#endif

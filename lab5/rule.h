#ifndef RULE_H_
#define RULE_H_
#define MAX_STR_LENGTH 64

struct _Rule;
typedef struct _RuleSymbol {
    struct _RuleSymbol *pNextSymbol;  // 指向下一个 Symbol
    struct _RuleSymbol *pOther;       // 指向下一个 Select
    int isToken;  // 是否为终结符。1 表示终结符，0 表示非终结符
    char SymbolName[MAX_STR_LENGTH];  // 终结符和非终结符的名称
    struct _Rule *pRule;  // 指向 Symbol 对应的 Rule。isToken 为 0 时这个域有效
} RuleSymbol;

typedef struct _Rule {
    char RuleName[MAX_STR_LENGTH];  // 文法的名称
    struct _RuleSymbol
        *pFirstSymbol;  // 指向文法的第一个 Select 的第一个 Symbol
    struct _Rule *pNextRule;  // 指向下一条文法
} Rule;

typedef struct _SYMBOL {
    int isToken;
    char Name[MAX_STR_LENGTH];
} SYMBOL;

typedef struct _RULE_ENTRY {
    char RuleName[MAX_STR_LENGTH];
    SYMBOL Selects[64][64];
} RULE_ENTRY;

Rule *InitRules(const RULE_ENTRY *rule_table, int nRuleCount);
Rule *CreateRule(const char *pRuleName);
RuleSymbol *CreateSymbol();
Rule *FindRule(Rule *pHead, const char *RuleName);
void PrintRule(Rule *pHead);

extern const char *VoidSymbol;

RuleSymbol *CopySymbol(const RuleSymbol *pSymbolTemplate);
RuleSymbol *CopySelect(const RuleSymbol *pSelectTemplate);
Rule *CopyRule(Rule *pHead);
#endif /* RULE_H_ */

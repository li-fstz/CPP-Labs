#include <stdlib.h>
#include <string.h>

#include "first.h"
#include "rule.h"
#include "voidtable.h"

/**
 * @brief
 * exp -> exp addop term| term
 * addop -> + | -
 * term -> term mulop factor | factor
 * mulop -> *
 * factor -> (exp) | number
 */
const struct RULE_ENTRY ruleTable[] = {
    {"exp", {{{0, "exp"}, {0, "addop"}, {0, "term"}}, {{0, "term"}}}},
    {"addop", {{{1, "+"}}, {{1, "-"}}}},
    {"term", {{{0, "term"}, {0, "mulop"}, {0, "factor"}}, {{0, "factor"}}}},
    {"mulop", {{{1, "*"}}}},
    {"factor", {{{1, "("}, {0, "exp"}, {1, ")"}}, {{1, "number"}}}}};

int main(int argc, char *argv[]) {
    Rule *ruleHead =
        InitRules(ruleTable, sizeof(ruleTable) / sizeof(struct RULE_ENTRY));
    PrintRule(ruleHead);
    VoidTable *voidTable = GenVoidTable(ruleHead);
    PrintVoidTable(voidTable);

    /**
     * @brief 调用 GenFirstSetList 函数求文法的 First 集合
     */
    FirstSetList *firstSetList = GenFirstSetList(ruleHead, voidTable);

    /**
     * @brief 输出 First 集
     */
    PrintFirstSetList(firstSetList);
    return 0;
}

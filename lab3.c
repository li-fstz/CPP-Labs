#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief
 * E -> TE'
 * E' -> +TE' | $
 * T -> FT'
 * T' -> *FT' | $
 * F -> i | (E)
 */
const struct RULE_ENTRY ruleTable[] = {
    {"E", {{{0, "T"}, {0, "E'"}}}},
    {"E'", {{{1, "+"}, {0, "T"}, {0, "E'"}}, {{1, "$"}}}},
    {"T", {{{0, "F"}, {0, "T'"}}}},
    {"T'", {{{1, "*"}, {0, "F"}, {0, "T'"}}, {{1, "$"}}}},
    {"F", {{{1, "i"}}, {{1, "("}, {0, "E"}, {1, ")"}}}}};

int main(int argc, char *argv[]) {
    Rule *ruleHead =
        InitRules(ruleTable, sizeof(ruleTable) / sizeof(struct RULE_ENTRY));
    PrintRule(ruleHead);
    VoidTable *voidTable = GenVoidTable(ruleHead);
    PrintVoidTable(voidTable);
    SetList *firstSetList = GenFirstSetList(ruleHead, voidTable);
    PrintFirstSetList(firstSetList);

    /**
     * @brief 调用 GenFollowSetList 函数求文法的 Follow 集合
     */
    SetList *followSetList =
        GenFollowSetList(ruleHead, voidTable, firstSetList);

    /**
     * @brief 输出 Follow 集
     */
    PrintFollowSetList(followSetList);
    return 0;
}

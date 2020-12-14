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
const struct RULE_ENTRY rule_table[] = {
    {"E", {{{0, "T"}, {0, "E'"}}}},
    {"E'", {{{1, "+"}, {0, "T"}, {0, "E'"}}, {{1, "$"}}}},
    {"T", {{{0, "F"}, {0, "T'"}}}},
    {"T'", {{{1, "*"}, {0, "F"}, {0, "T'"}}, {{1, "$"}}}},
    {"F", {{{1, "i"}}, {{1, "("}, {0, "E"}, {1, ")"}}}}};

int main(int argc, char *argv[]) {
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));
    PrintRule(pRuleHead);
    VoidTable VoidTable;
    GenVoidTable(pRuleHead, &VoidTable);
    PrintVoidTable(&VoidTable);
    SetList FirstSetList, FollowSetList;
    FirstSetList.nSetCount = 0;
    FollowSetList.nSetCount = 0;
    GenFirstSetList(pRuleHead, &VoidTable, &FirstSetList);
    PrintFirstSetList(&FirstSetList);

    /**
     * @brief 调用 GenFollowSetList 函数求文法的 Follow 集合
     */
    GenFollowSetList(pRuleHead, &VoidTable, &FirstSetList, &FollowSetList);

    /**
     * @brief 输出 Follow 集
     */
    PrintFollowSetList(&FollowSetList);
    return 0;
}

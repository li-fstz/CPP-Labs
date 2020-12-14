#include <stdlib.h>
#include <string.h>

#include "follow.h"
#include "parsingtable.h"

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
    GenFollowSetList(pRuleHead, &VoidTable, &FirstSetList, &FollowSetList);
    PrintFollowSetList(&FollowSetList);
    SelectSetList SelectSetList;
    SelectSetList.nSetCount = 0;

    /**
     * @brief 调用 GenSelectSetList 求出文法的 Select 集
     */
    GenSelectSetList(pRuleHead, &VoidTable, &FirstSetList, &FollowSetList,
                     &SelectSetList);

    /**
     * @brief 输出 Select 集
     */
    PrintSelectSetList(&SelectSetList);
    ParsingTable ParsingTable;

    /**
     * @brief 调用 GenParsingTable 求出文法的预测分析表
     */
    GenParsingTable(pRuleHead, &SelectSetList, &ParsingTable);

    /**
     * @brief 输出预测分析表
     */
    PrintParsingTable(&ParsingTable);
    return 0;
}

#include <stdlib.h>
#include <string.h>

#include "follow.h"
#include "parsingtable.h"

/* exp -> exp addop term| term
   addop -> + | -
   term -> term mulop factor | factor
   mulop -> *
   factor -> (exp) | number */

/**
 * @brief
 *
 */
const struct RULE_ENTRY rule_table[] = {
    {"E", {{{0, "T"}, {0, "E'"}}}},
    {"E'", {{{1, "+"}, {0, "T"}, {0, "E'"}}, {{1, "$"}}}},
    {"T", {{{0, "F"}, {0, "T'"}}}},
    {"T'", {{{1, "*"}, {0, "F"}, {0, "T'"}}, {{1, "$"}}}},
    {"F", {{{1, "i"}}, {{1, "("}, {0, "E"}, {1, ")"}}}}};

int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));
    PrintRule(pRuleHead);

    //
    // 调用 VoidTable 函数求文法的空表
    //
    VoidTable VoidTable;
    GenVoidTable(pRuleHead, &VoidTable);
    PrintVoidTable(&VoidTable);

    //
    // 初始化 First 集合、Follow 集合
    //
    SetList FirstSetList, FollowSetList;
    FirstSetList.nSetCount = 0;
    FollowSetList.nSetCount = 0;

    //
    // 调用 First 函数求文法的 First 集合
    //
    GenFirstSet(pRuleHead, &VoidTable, &FirstSetList);
    PrintFirstSet(&FirstSetList);

    //
    // 调用 Follow 函数求文法的 First 集合、Follow 集合
    //
    GenFollowSet(pRuleHead, &VoidTable, &FollowSetList, &FirstSetList);
    PrintFollowSet(&FollowSetList);

    SelectSetList SelectSetList;
    GenSelectSet(pRuleHead, &VoidTable, &FirstSetList, &FollowSetList,
                 &SelectSetList);
    PrintSelectSet(&SelectSetList);

    ParsingTable ParsingTable;
    GenParsingTable(pRuleHead, &ParsingTable, &SelectSetList);
    PrintParsingTable(&ParsingTable);
    return 0;
}

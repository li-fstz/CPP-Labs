#include <stdlib.h>
#include <string.h>

#include "follow.h"
#include "parsingtable.h"

/**
 * @brief
 * exp -> exp addop term| term
 * addop -> + | -
 * term -> term mulop factor | factor
 * mulop -> *
 * factor -> (exp) | number
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
    GenFirstSetList(pRuleHead, &VoidTable, &FirstSetList);
    PrintFirstSetList(&FirstSetList);

    //
    // 调用 Follow 函数求文法的 First 集合、Follow 集合
    //
    GenFollowSetList(pRuleHead, &VoidTable, &FirstSetList, &FollowSetList);
    PrintFollowSetList(&FollowSetList);

    SelectSetList SelectSetList;
    GenSelectSetList(pRuleHead, &VoidTable, &FirstSetList, &FollowSetList,
                 &SelectSetList);
    PrintSelectSetList(&SelectSetList);

    ParsingTable ParsingTable;
    GenParsingTable(pRuleHead, &SelectSetList, &ParsingTable);
    PrintParsingTable(&ParsingTable);
    return 0;
}

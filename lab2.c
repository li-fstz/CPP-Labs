#include <stdlib.h>
#include <string.h>

#include "first.h"
#include "rule.h"
#include "voidtable.h"

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
    {"exp", {{{0, "exp"}, {0, "addop"}, {0, "term"}}, {{0, "term"}}}},
    {"addop", {{{1, "+"}}, {{1, "-"}}}},
    {"term", {{{0, "term"}, {0, "mulop"}, {0, "factor"}}, {{0, "factor"}}}},
    {"mulop", {{{1, "*"}}}},
    {"factor", {{{1, "("}, {0, "exp"}, {1, ")"}}, {{1, "number"}}}}};

int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));

    //
    // 输出文法
    //
    PrintRule(pRuleHead);

    //
    // 调用 VoidTable 函数求文法的空表
    //

    VoidTable VoidTable;
    GenVoidTable(pRuleHead, &VoidTable);
    PrintVoidTable(&VoidTable);

    //
    // 调用 First 函数求文法的 First 集合
    //
    SetList FirstSetList;
    FirstSetList.nSetCount = 0;
    GenFirstSet(pRuleHead, &VoidTable, &FirstSetList);

    //
    // 输出First集合
    //
    PrintFirstSet(&FirstSetList);

    return 0;
}

#include <stdlib.h>
#include <string.h>

#include "follow.h"

const char *VoidSymbol = "$";  // "ε"
const char *EndSymbol = "#";

/* exp -> exp addop term| term
   addop -> + | -
   term -> term mulop factor | factor
   mulop -> *
   factor -> (exp) | number */
const RULE_ENTRY rule_table[] = {
    {"exp", {{{0, "exp"}, {0, "addop"}, {0, "term"}}, {{0, "term"}}}},
    {"addop", {{{1, "+"}}, {{1, "-"}}}},
    {"term", {{{0, "term"}, {0, "mulop"}, {0, "factor"}}, {{0, "factor"}}}},
    {"mulop", {{{1, "*"}}}},
    {"factor", {{{1, "("}, {0, "exp"}, {1, ")"}}, {{1, "number"}}}}};

int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pHead  = InitRules(rule_table, sizeof(rule_table) / sizeof(RULE_ENTRY));;
    PrintRule(pHead);
    //
    // 调用 VoidTable 函数求文法的空表
    //
    Column *pVoidTable = InitVoidTable(pHead);
    VoidTable(pHead, pVoidTable);
    PrintTable(pVoidTable);

    //
    // 初始化 First 集合、Follow 集合
    //
    SetList FirstSetList, FollowSetList;
    FirstSetList.nSetCount = 0;
    FollowSetList.nSetCount = 0;

    //
    // 调用 Follow 函数求文法的 First 集合、Follow 集合
    //
    Follow(pHead, pVoidTable, &FollowSetList, &FirstSetList);

    //
    // 输出文法
    //
    PrintRule(pHead);

    //
    // 输出Follow集合
    //
    PrintFollowSet(&FollowSetList);

    return 0;
}



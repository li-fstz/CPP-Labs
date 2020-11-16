#include <stdlib.h>
#include <string.h>

#include "first.h"
#include "rule.h"
#include "voidtable.h"

const char *VoidSymbol = "$";  // "ε"

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
    Rule *pHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(RULE_ENTRY));

    //
    // 输出文法
    //
    PrintRule(pHead);

    //
    // 调用 VoidTable 函数求文法的空表
    //
    
    VoidTable VoidTable;
    GenVoidTable(pHead, &VoidTable);
    PrintVoidTable(&VoidTable);

    //
    // 调用 First 函数求文法的 First 集合
    //
    SetList FirstSet;
    FirstSet.nSetCount = 0;
    First(pHead, &VoidTable, &FirstSet);

    //
    // 输出First集合
    //
    PrintFirstSet(&FirstSet);

    return 0;
}

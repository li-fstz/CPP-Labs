#include <stdio.h>

#include "rule.h"
#include "voidtable.h"

/**
 * @brief 
 * S -> AB | bC
 * A -> $ | b
 * B -> $ | aD
 * C -> AD | b
 * D -> aS | c
 */
const struct RULE_ENTRY rule_table[] = {
    {"S", {{{0, "A"}, {0, "B"}}, {{1, "b"}, {0, "C"}}}},
    {"A", {{{1, "$"}}, {{1, "b"}}}},
    {"B", {{{1, "$"}}, {{1, "a"}, {0, "D"}}}},
    {"C", {{{0, "A"}, {0, "D"}}, {{1, "b"}}}},
    {"D", {{{1, "a"}, {0, "S"}}, {{1, "c"}}}}};

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
    //
    // 输出空表
    //
    printf("\nThe Void Table:\n");
    PrintVoidTable(&VoidTable);
    return 0;
}

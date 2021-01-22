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
const struct RULE_ENTRY ruleTable[] = {
    {"S", {{{0, "A"}, {0, "B"}}, {{1, "b"}, {0, "C"}}}},
    {"A", {{{1, "$"}}, {{1, "b"}}}},
    {"B", {{{1, "$"}}, {{1, "a"}, {0, "D"}}}},
    {"C", {{{0, "A"}, {0, "D"}}, {{1, "b"}}}},
    {"D", {{{1, "a"}, {0, "S"}}, {{1, "c"}}}}};

int main(int argc, char *argv[]) {

    /**
     * @brief 调用 InitRules 函数初始化文法
     */
    Rule *ruleHead =
        InitRules(ruleTable, sizeof(ruleTable) / sizeof(struct RULE_ENTRY));

    /**
     * @brief 输出文法
     */
    PrintRule(ruleHead);

    /**
     * @brief 调用 GenVoidTable 函数求文法的空表
     */
    VoidTable *voidTable = GenVoidTable(ruleHead);

    /**
     * @brief 输出空表
     */
    printf("\nThe Void Table:\n");
    PrintVoidTable(voidTable);
    return 0;
}

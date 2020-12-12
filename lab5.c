#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pickupleftfactor.h"
#include "rule.h"

/**
 * @brief
 * A -> abC | abcD | abcE
 */
const struct RULE_ENTRY rule_table[] = {
    {"A",
     {{{1, "a"}, {1, "b"}, {1, "C"}},
      {{1, "a"}, {1, "b"}, {1, "c"}, {1, "D"}},
      {{1, "a"}, {1, "b"}, {1, "c"}, {1, "E"}}}}};

int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));

    //
    // 输出提取左因子之前的文法
    //
    printf("Before Pickup Left Factor:\n");
    PrintRule(pRuleHead);

    //
    // 调用 PickupLeftFactor 函数对文法提取左因子
    //
    PickupLeftFactor(pRuleHead);

    //
    // 输出提取左因子之后的文法
    //
    printf("\nAfter Pickup Left Factor:\n");
    PrintRule(pRuleHead);

    return 0;
}

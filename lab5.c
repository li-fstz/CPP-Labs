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
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));
    printf("Before Pickup Left Factor:\n");
    PrintRule(pRuleHead);

    /**
     * @brief 调用 PickupLeftFactor 函数对文法提取左因子
     */
    PickupLeftFactor(pRuleHead);
    printf("\nAfter Pickup Left Factor:\n");
    PrintRule(pRuleHead);
    return 0;
}

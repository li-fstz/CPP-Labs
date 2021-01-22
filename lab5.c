#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pickupleftfactor.h"
#include "rule.h"

/**
 * @brief
 * A -> abC | abcD | abcE
 */
const struct RULE_ENTRY ruleTable[] = {
    {"A",
     {{{1, "a"}, {1, "b"}, {1, "C"}},
      {{1, "a"}, {1, "b"}, {1, "c"}, {1, "D"}},
      {{1, "a"}, {1, "b"}, {1, "c"}, {1, "E"}}}}};

int main(int argc, char *argv[]) {
    Rule *ruleHead =
        InitRules(ruleTable, sizeof(ruleTable) / sizeof(struct RULE_ENTRY));
    printf("Before Pickup Left Factor:\n");
    PrintRule(ruleHead);

    /**
     * @brief 调用 PickupLeftFactor 函数对文法提取左因子
     */
    PickupLeftFactor(ruleHead);
    printf("\nAfter Pickup Left Factor:\n");
    PrintRule(ruleHead);
    return 0;
}

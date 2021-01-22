#include <stdlib.h>
#include <string.h>

#include "removeleftrecursion2.h"

/**
 * @brief
 * A -> Ba | Aa | c
 * B -> Bb | Ab | d
 */
const struct RULE_ENTRY ruleTable[] = {
    {"A", {{{0, "B"}, {1, "a"}}, {{0, "A"}, {1, "a"}}, {{1, "c"}}}},
    {"B", {{{0, "B"}, {1, "b"}}, {{0, "A"}, {1, "b"}}, {{1, "d"}}}}};
int main(int argc, char *argv[]) {
    Rule *ruleHead =
        InitRules(ruleTable, sizeof(ruleTable) / sizeof(struct RULE_ENTRY));
    printf("Before Remove Left Recursion:\n");
    PrintRule(ruleHead);

    /**
     * @brief 调用 RemoveLeftRecursion 函数对文法消除左递归
     */
    RemoveLeftRecursion(ruleHead);
    printf("\nAfter Remove Left Recursion:\n");
    PrintRule(ruleHead);
    return 0;
}

#include <stdlib.h>
#include <string.h>

#include "removeleftrecursion2.h"

/**
 * @brief
 * A -> Ba | Aa | c
 * B -> Bb | Ab | d
 */
const struct RULE_ENTRY rule_table[] = {
    {"A", {{{0, "B"}, {1, "a"}}, {{0, "A"}, {1, "a"}}, {{1, "c"}}}},
    {"B", {{{0, "B"}, {1, "b"}}, {{0, "A"}, {1, "b"}}, {{1, "d"}}}}};
int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));
    ;

    //
    // 输出消除左递归之前的文法
    //
    printf("Before Remove Left Recursion:\n");
    PrintRule(pRuleHead);

    //
    // 调用 RemoveLeftRecursion 函数消除文法中的左递归
    //
    RemoveLeftRecursion(pRuleHead);

    //
    // 输出消除左递归之后的文法
    //
    printf("\nAfter Remove Left Recursion:\n");
    PrintRule(pRuleHead);

    return 0;
}

#include <stdlib.h>
#include <string.h>

#include "removeleftrecursion1.h"

/**
 * @brief
 * A -> Aa | bA | c | Ad
 */
const struct RULE_ENTRY rule_table[] = {{"A",
                                         {{{0, "A"}, {1, "a"}},
                                          {{1, "b"}, {0, "A"}},
                                          {{1, "c"}},
                                          {{0, "A"}, {1, "d"}}}}};

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
    // 调用 RemoveLeftRecursion 函数对文法消除左递归
    //
    RemoveLeftRecursion(pRuleHead);

    //
    // 输出消除左递归之后的文法
    //
    printf("\nAfter Remove Left Recursion:\n");
    PrintRule(pRuleHead);

    return 0;
}

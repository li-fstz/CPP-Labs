#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pickupleftfactor.h"
#include "rule.h"

const char *VoidSymbol = "$";  // "ε"
const char *Postfix = "'";

/* A -> abC | abcD | abcE */
const RULE_ENTRY rule_table[] = {{"A",
                                  {{{1, "a"}, {1, "b"}, {1, "C"}},
                                   {{1, "a"}, {1, "b"}, {1, "c"}, {1, "D"}},
                                   {{1, "a"}, {1, "b"}, {1, "c"}, {1, "E"}}}}};

int main(int argc, char *argv[]) {
    //
    // 调用 InitRules 函数初始化文法
    //
    Rule *pHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(RULE_ENTRY));

    //
    // 输出提取左因子之前的文法
    //
    printf("Before Pickup Left Factor:\n");
    PrintRule(pHead);

    //
    // 调用 PickupLeftFactor 函数对文法提取左因子
    //
    PickupLeftFactor(pHead);

    //
    // 输出提取左因子之后的文法
    //
    printf("\nAfter Pickup Left Factor:\n");
    PrintRule(pHead);

    return 0;
}


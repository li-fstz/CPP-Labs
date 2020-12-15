#include <stdlib.h>
#include <string.h>

#include "follow.h"
#include "parsingtable.h"

/**
 * @brief
 * E -> TE'
 * E' -> +TE' | $
 * T -> FT'
 * T' -> *FT' | $
 * F -> i | (E)
 */
const struct RULE_ENTRY rule_table[] = {
    {"E", {{{0, "T"}, {0, "E'"}}}},
    {"E'", {{{1, "+"}, {0, "T"}, {0, "E'"}}, {{1, "$"}}}},
    {"T", {{{0, "F"}, {0, "T'"}}}},
    {"T'", {{{1, "*"}, {0, "F"}, {0, "T'"}}, {{1, "$"}}}},
    {"F", {{{1, "i"}}, {{1, "("}, {0, "E"}, {1, ")"}}}}};

int main(int argc, char *argv[]) {
    Rule *pRuleHead =
        InitRules(rule_table, sizeof(rule_table) / sizeof(struct RULE_ENTRY));
    PrintRule(pRuleHead);
    VoidTable *pVoidTable = GenVoidTable(pRuleHead);
    PrintVoidTable(pVoidTable);
    SetList *pFirstSetList = GenFirstSetList(pRuleHead, pVoidTable),
            *pFollowSetList =
                GenFollowSetList(pRuleHead, pVoidTable, pFirstSetList);
    PrintFirstSetList(pFirstSetList);
    PrintFollowSetList(pFollowSetList);

    /**
     * @brief 调用 GenSelectSetList 求出文法的 Select 集
     */
    SelectSetList *pSelectSetList =
        GenSelectSetList(pRuleHead, pVoidTable, pFirstSetList, pFollowSetList);

    /**
     * @brief 输出 Select 集
     */
    PrintSelectSetList(pSelectSetList);

    /**
     * @brief 调用 GenParsingTable 求出文法的预测分析表
     */
    ParsingTable *pParsingTable = GenParsingTable(pRuleHead, pSelectSetList);

    /**
     * @brief 输出预测分析表
     */
    PrintParsingTable(pParsingTable);
    return 0;
}

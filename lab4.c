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
const struct RULE_ENTRY ruleTable[] = {
    {"E", {{{0, "T"}, {0, "E'"}}}},
    {"E'", {{{1, "+"}, {0, "T"}, {0, "E'"}}, {{1, "$"}}}},
    {"T", {{{0, "F"}, {0, "T'"}}}},
    {"T'", {{{1, "*"}, {0, "F"}, {0, "T'"}}, {{1, "$"}}}},
    {"F", {{{1, "i"}}, {{1, "("}, {0, "E"}, {1, ")"}}}}};

int main(int argc, char *argv[]) {
    Rule *ruleHead =
        InitRules(ruleTable, sizeof(ruleTable) / sizeof(struct RULE_ENTRY));
    PrintRule(ruleHead);
    VoidTable *voidTable = GenVoidTable(ruleHead);
    PrintVoidTable(voidTable);
    FirstSetList *firstSetList = GenFirstSetList(ruleHead, voidTable);
    PrintFirstSetList(firstSetList);
    FollowSetList *followSetList =
        GenFollowSetList(ruleHead, voidTable, firstSetList);
    PrintFollowSetList(followSetList);

    /**
     * @brief 调用 GenSelectSetList 求出文法的 Select 集
     */
    SelectSetList *selectSetList =
        GenSelectSetList(ruleHead, voidTable, firstSetList, followSetList);

    /**
     * @brief 输出 Select 集
     */
    PrintSelectSetList(selectSetList);

    /**
     * @brief 调用 GenParsingTable 求出文法的预测分析表
     */
    ParsingTable *parsingTable = GenParsingTable(ruleHead, selectSetList);

    /**
     * @brief 输出预测分析表
     */
    PrintParsingTable(parsingTable);
    return 0;
}

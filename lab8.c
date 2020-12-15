#include <stdlib.h>
#include <string.h>

#include "follow.h"
#include "parser.h"
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
    SelectSetList *pSelectSetList =
        GenSelectSetList(pRuleHead, pVoidTable, pFirstSetList, pFollowSetList);
    PrintSelectSetList(pSelectSetList);
    ParsingTable *pParsingTable = GenParsingTable(pRuleHead, pSelectSetList);
    PrintParsingTable(pParsingTable);

    /**
     * @brief 调用 Parse 函数对文本进行自顶向下语法分析
     */
    Parse(pRuleHead, pParsingTable, "i+i*i#");
    return 0;
}

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
    SelectSetList *selectSetList =
        GenSelectSetList(ruleHead, voidTable, firstSetList, followSetList);
    PrintSelectSetList(selectSetList);
    ParsingTable *parsingTable = GenParsingTable(ruleHead, selectSetList);
    PrintParsingTable(parsingTable);

    /**
     * @brief 调用 Parse 函数对文本进行自顶向下语法分析
     */
    Parse(ruleHead, parsingTable, "i+i*i#");
    return 0;
}

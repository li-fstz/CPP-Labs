#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief 将符号压入栈中
 *
 * @param stack 栈指针
 * @param symbol 符号指针
 */
void PushSymbol(ParsingStack *stack, const Symbol *symbol) {
    assert(stack && IS_SYMBOL(symbol));
    stack->symbols =
        realloc(stack->symbols, (stack->symbolCount + 1) * sizeof(Symbol *));
    stack->symbols[stack->symbolCount++] = symbol;
}

/**
 * @brief PushProduction 的辅助函数
 *
 * @param stack 栈指针
 * @param symbol 符号指针
 */
void PushProductionSymbol(ParsingStack *stack, const Symbol *symbol) {
    assert(stack && (IS_SYMBOL(symbol) || !symbol));
    if (symbol == NULL) {
        return;
    }
    PushProductionSymbol(stack, symbol->next);
    if (strcmp(SYMBOL_NAME(symbol), VOID_SYMBOL)) {
        PushSymbol(stack, symbol);
    }
}

/**
 * @brief 将产生式逆向压入栈中，在压栈时忽略 ε
 *
 * @param stack 栈指针
 * @param production 产生式指针
 */
void PushProduction(ParsingStack *stack, const Production *production) {
    assert(stack && IS_PRODUCTION(production));
    PushProductionSymbol(stack, SYMBOL_HEAD(production));
}

/**
 * @brief 将符号从栈中弹出
 *
 * @param stack 栈指针
 * @return Symbol* 符号指针
 */
const Symbol *PopSymbol(ParsingStack *stack) {
    assert(stack);
    if (stack->symbolCount) {
        const Symbol *r = stack->symbols[--stack->symbolCount];
        stack->symbols =
            realloc(stack->symbols, stack->symbolCount * sizeof(Symbol *));
        return r;
    } else {
        assert(0);
        return NULL;
    }
}

/**
 * @brief 输出栈
 *
 * @param stack 栈指针
 */
void FormatParsingStack(char *tmpStr, const ParsingStack *stack) {
    assert(tmpStr && stack);
    tmpStr[0] = '\0';
    for (int i = 0; i < stack->symbolCount; i++) {
        strcat(tmpStr, SYMBOL_NAME(stack->symbols[i]));
    }
    int len = strlen(tmpStr);
    for (int i = len - 1; i < 10; i ++) {
        tmpStr[i] = ' ';
    }
    tmpStr[10] = '\0';
}

/**
 * @brief 输出产生式
 *
 * @param production 产生式指针
 */
void FormatProduction(char *tmpStr, const Production *production) {
    assert(tmpStr && IS_PRODUCTION(production));
    tmpStr[0] = '\0';
    for (Symbol *symbol = SYMBOL_HEAD(production); symbol != NULL;
         symbol = symbol->next) {
        strcat(tmpStr, (SYMBOL_NAME(symbol)));
    }
}

/**
 * @brief 执行自顶向下语法分析
 *
 * @param ruleHead 文法链表头指针
 * @param parsingTable 预测分析表指针
 * @param string 待解析文本
 */
void Parse(const Rule *ruleHead, const ParsingTable *parsingTable,
           const char *string) {
    assert(IS_RULE(ruleHead) && IS_PARSING_TABLE(parsingTable) && string);
    /**
     * @brief 初始化分析栈
     */
    ParsingStack *stack = calloc(1, sizeof(ParsingStack));

    /**
     * @brief 初始化开始符号和结束符号，
     * 并压入栈中
     */
    Symbol *end = NewSymbol(END_SYMBOL),
           *start = NewSymbol(RULE_NAME(ruleHead));
    PushSymbol(stack, end);
    RULE(start) = ruleHead;
    PushSymbol(stack, start);

    int i = 0, parsingCorrect = 1;
    while (stack->symbolCount && parsingCorrect) {
        char procedureStr[200] = {'\0'}, tmpStr[100];
        sprintf(tmpStr, "%-4d", ++i);
        strcat(procedureStr, tmpStr);
        FormatParsingStack(tmpStr, stack);
        strcat(procedureStr, tmpStr);
        sprintf(tmpStr, "%-10s", string);
        strcat(procedureStr, tmpStr);

        /**
         * @brief 从分析栈中弹出符号
         * 如果是终结符则进行匹配，
         * 如果是非终结符则从预测分析表中
         * 找到产生式逆向压入栈中。
         */
        const Symbol *topSymbol = PopSymbol(stack);
        if (IS_TOKEN(topSymbol)) {
            if (stack->symbolCount) {
                if (*SYMBOL_NAME(topSymbol) == *string) {
                    sprintf(tmpStr, "“%c”匹配  ", *string);
                } else {
                    sprintf(tmpStr, "“%c”不匹配", *string);
                    parsingCorrect = 0;
                }
                strcat(procedureStr, tmpStr);
                string++;
            } else {
                if (strcmp(string, END_SYMBOL) == 0) {
                    strcat(procedureStr, "接受  ");
                } else {
                    strcat(procedureStr, "不接受");
                    parsingCorrect = 0;
                }
            }
        } else {
            char ch[] = {*string, '\0'};
            const Production **foundProduction = 
                FindProduction(parsingTable, RULE(topSymbol), ch);
            sprintf(tmpStr, "%s->", SYMBOL_NAME(topSymbol));
            strcat(procedureStr, tmpStr);
            FormatProduction(tmpStr, *foundProduction);
            strcat(procedureStr, tmpStr);
            PushProduction(stack, *foundProduction);
        }
        printf("%s\n", procedureStr);
    }
}

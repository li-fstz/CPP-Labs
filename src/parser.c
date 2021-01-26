#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief 将符号压入栈中
 *
 * @param stack 栈的指针
 * @param symbol 符号指针
 */
void PushSymbol(ParsingStack *stack, const Symbol *symbol) {
    stack->symbols = realloc(stack->symbols, (stack->symbolCount + 1) * sizeof (Symbol *));
    stack->symbols[stack->symbolCount++] = symbol;
}

void PushProductionSymbol(ParsingStack *stack, const Symbol *symbol) {
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
 * @param stack 栈的指针
 * @param production 产生式指针
 */
void PushProduction(ParsingStack *stack, const Production *production) {
    PushProductionSymbol(stack, SYMBOL_HEAD(production));
}

/**
 * @brief 将符号从栈中弹出
 *
 * @param stack 栈的指针
 * @return Symbol* 符号的指针
 */
const Symbol *PopSymbol(ParsingStack *stack) {
    if (stack->symbolCount) {
        Symbol *r = stack->symbols[--stack->symbolCount];
        stack->symbols = realloc(stack->symbols, stack->symbolCount * sizeof (Symbol *));
        return r;
    } else {
        assert(0);
        return NULL;
    }
}

/**
 * @brief 输出栈
 *
 * @param stack 栈的指针
 */
void PrintParsingStack(const ParsingStack *stack) {
    for (int i = 0; i < stack->symbolCount; i++) {
        printf(SYMBOL_NAME(stack->symbols[i]));
    }
}

/**
 * @brief 执行自顶向下语法分析
 *
 * @param ruleHead 文法链表的头指针
 * @param parsingTable 预测分析表的指针
 * @param string 待解析文本
 */
void Parse(const Rule *ruleHead, const ParsingTable *parsingTable,
           const char *string) {
    /**
     * @brief 初始化分析栈
     */
    ParsingStack *stack = calloc(1, sizeof(ParsingStack));

    /**
     * @brief 初始化开始符号和结束符号，
     * 并压入栈中
     */
    Symbol *end = NewSymbol(END_SYMBOL), *start = NewSymbol(RULE_NAME(ruleHead));
    PushSymbol(stack, end);
    RULE(start) = ruleHead;
    PushSymbol(stack, start);

    int i = 0;
    while (stack->symbolCount) {
        printf("%d\t", ++i);
        PrintParsingStack(stack);
        printf("\t%s\t", string);

        /**
         * @brief 从分析栈中弹出符号
         * 如果是终结符则进行匹配，
         * 如果是非终结符则从预测分析表中
         * 找到产生式逆向压入栈中。
         */
        Symbol *topSymbol = PopSymbol(stack);
        if (IS_TOKEN(topSymbol)) {
            if (stack->symbolCount) {
                if (*SYMBOL_NAME(topSymbol) == *string) {
                    printf("“%c”匹配", *string);
                } else {
                    printf("“%c”不匹配", *string);
                    return;
                }
                string++;
            } else {
                if (strcmp(string, END_SYMBOL) == 0) {
                    printf("接受");
                } else {
                    printf("不接受");
                }
            }
        } else {
            char ch[] = {*string, '\0'};
            Production *production =
                *FindProduction(parsingTable, RULE(topSymbol), ch);
            printf("%s->", SYMBOL_NAME(topSymbol));
            PrintProduction(production);
            PushProduction(stack, production);
        }
        putchar('\n');
    }
}

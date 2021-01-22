#include "rule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 初始化文法链表
 *
 * @param ruleTable 原始文法实体的指针
 * @param ruleCount 文法的数量
 * @return Rule* 文法链表的头指针
 */
Rule *InitRules(const struct RULE_ENTRY *ruleTable, int ruleCount) {
    Rule *ruleHead = NULL;
    for (int i = 0; i < ruleCount; i ++) {
        ruleHead = NewRule(ruleTable[i].ruleName);
    }
    Rule *rule = ruleHead;
    for (int i = 0; i < ruleCount; i ++) {
        for (int j = 0; ruleTable[i].productions[j][0].symbolName[0]; j ++) {
            Production *production = NewProduction();
            PRODUCTIONHEAD(rule) = Append(PRODUCTIONHEAD(rule), production);
            for (int k = 0; ruleTable[i].productions[j][k].symbolName[0]; k ++) {
                Symbol *symbol = NewSymbol(ruleTable[i].productions[j][k].symbolName);
                if (ruleTable[i].productions[j][k].isToken) {
                    RULE(symbol) = FindRule(ruleHead, SYMBOLNAME(symbol));
                    if (RULE(symbol) == NULL) {
                        printf("Init rules error, miss rule \"%s\"\n", SYMBOLNAME(symbol));
                        exit(1);
                    }
                }
                SYMBOLHEAD(production) = Append(SYMBOLHEAD(production), symbol);
            }
        }
        rule = rule->next;
    }
    return ruleHead;
}

Rule *NewRule(const char *ruleName) {
    Rule *r = NewNode();
    RULENAME(r) = strdup (ruleName);
    return r;
}
Symbol *NewSymbol(const char *symbolName) {
    Symbol *r = NewNode();
    SYMBOLNAME(r) = strdup (symbolName);
    return r;
}
Production *NewProduction() {
    return NewNode();
}

/**
 * @brief 从文法链表中使用名字查找文法
 *
 * @param ruleHead 文法链表的头指针
 * @param ruleName 文法的名字
 * @return Rule* 如果存在该文法则返回文法的指针，否则返回 NULL
 */
const Rule *FindRule(const Rule *ruleHead, const char *ruleName) {
    for (; ruleHead != NULL; ruleHead = ruleHead->next) {
        if (strcmp(RULENAME(ruleHead), ruleName) == 0) {
            return ruleHead;
        }
    }
    return NULL;

}

/**
 * @brief 输出文法
 *
 * @param ruleHead 文法链表的头指针
 */
void PrintRule(const Rule *ruleHead) {
    if (ruleHead) {
        printf("%s->", RULENAME(ruleHead));
        for (Production *production = PRODUCTIONHEAD(ruleHead);
             production != NULL; production = production->next) {
            for (Symbol *symbol = SYMBOLHEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                printf("%s", SYMBOLNAME(symbol));
            }
            putchar(production->next ? '|' : '\n');
        }
        PrintRule(ruleHead->next);
    }
}

Rule *CopyRules(const Rule *ruleHeadTemplate) {
    if (ruleHeadTemplate == NULL) {
        return NULL;
    }
    Rule *rule = CopyRule(ruleHeadTemplate);
    rule->next = CopyRules(ruleHeadTemplate->next);
    return rule;
}

/**
 * @brief 复制整个文法链表
 *
 * @param ruleHead 文法链表的头指针
 * @return Rule* 新文法链表的头指针
 */
Rule *CopyRule(const Rule *ruleTemplate) {
    if (ruleTemplate == NULL) {
        return NULL;
    }
    Rule *rule = NewRule(RULENAME(ruleTemplate));
    for (Production *production = PRODUCTIONHEAD(ruleTemplate); production != NULL; production = production->next) {
        PRODUCTIONHEAD(rule) = Append (PRODUCTIONHEAD(rule), CopyProduction(production));
    }
    return rule;
}

/**
 * @brief 复制一个符号
 *
 * @param symbolTemplate 被复制的符号的指针
 * @return Symbol* 新符号的指针
 */
Symbol *CopySymbol(const Symbol *symbolTemplate) {
    if (symbolTemplate == NULL) {
        return NULL;
    }
    Symbol *symbol = NowSymbol(SYMBOLNAME(symbolTemplate));
    RULE(symbol) = RULE(symbolTemplate);
    return symbol;
}

/**
 * @brief 复制一个产生式
 *
 * @param productionTemplate 被产生式的产生式指针
 * @return Symbol* 新产生式的指针
 */
Production *CopyProduction(const Production *productionTemplate) {
    if (productionTemplate == NULL) {
        return NULL;
    }
    Production *production = NewProduction();
    for(Symbol *symbol = SYMBOLHEAD(productionTemplate); symbol != NULL; symbol = symbol->next) {
        SYMBOLHEAD(production) = Append(SYMBOLHEAD(production), CopySymbol(symbol));
    }
    return production;
}

LinkedNode *Delete(LinkedNode *head, LinkedNode *node) {
    if (head == node) {
        //free (node);
        return head->next;
    }
    for (LinkedNode *p = head; p->next != NULL; p = p->next) {
        if (p->next == node) {
            p->next = node->next;
            //free (node);
            break;
        }
    }
    return head;
}
LinkedNode *Append(LinkedNode *head, const LinkedNode *node) {
    if (head == NULL) {
        return node;
    }
    LinkedNode *p;
    for (p = head; p->next != NULL; p = p->next);
    p->next = node;
    return head;
}
LinkedNode *NewNode() {
    return calloc(1, sizeof (LinkedNode));
}
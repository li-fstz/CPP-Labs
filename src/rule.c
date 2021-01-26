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
    assert(ruleTable && ruleCount);
    Rule *ruleHead = NULL;
    for (int i = 0; i < ruleCount; i ++) {
        ruleHead = AppendNode(ruleHead, NewRule(ruleTable[i].ruleName));
    }
    Rule *rule = ruleHead;
    for (int i = 0; i < ruleCount; i ++) {
        for (int j = 0; ruleTable[i].productions[j][0].symbolName[0]; j ++) {
            Production *production = NewProduction();
            PRODUCTION_HEAD(rule) = AppendNode(PRODUCTION_HEAD(rule), production);
            for (int k = 0; ruleTable[i].productions[j][k].symbolName[0]; k ++) {
                Symbol *symbol = NewSymbol(ruleTable[i].productions[j][k].symbolName);
                if (!ruleTable[i].productions[j][k].isToken) {
                    RULE(symbol) = FindRule(ruleHead, SYMBOL_NAME(symbol));
                    if (RULE(symbol) == NULL) {
                        printf("Init rules error, miss rule \"%s\"\n", SYMBOL_NAME(symbol));
                        exit(1);
                    }
                }
                SYMBOL_HEAD(production) = AppendNode(SYMBOL_HEAD(production), symbol);
            }
        }
        rule = rule->next;
    }
    return ruleHead;
}

Rule *NewRule(const char *ruleName) {
    assert(ruleName);
    Rule *r = NewNode(RuleNode);
    r->value = calloc (1, sizeof (struct Rule));
    RULE_NAME(r) = strdup (ruleName);
    return r;
}
Symbol *NewSymbol(const char *symbolName) {
    assert(symbolName);
    Symbol *r = NewNode(SymbolNode);
    r->value = calloc (1, sizeof (struct Symbol));
    SYMBOL_NAME(r) = strdup (symbolName);
    return r;
}
Production *NewProduction() {
    Production *r = NewNode(ProductionNode);
    r->value = calloc (1, sizeof (struct Production));
    return r;
}

/**
 * @brief 从文法链表中使用名字查找文法
 *
 * @param ruleHead 文法链表的头指针
 * @param ruleName 文法的名字
 * @return Rule* 如果存在该文法则返回文法的指针，否则返回 NULL
 */
const Rule *FindRule(const Rule *ruleHead, const char *ruleName) {
    assert(IS_RULE(ruleHead) && ruleName);
    for (; ruleHead != NULL; ruleHead = ruleHead->next) {
        if (strcmp(RULE_NAME(ruleHead), ruleName) == 0) {
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
    assert(!ruleHead || IS_RULE(ruleHead));
    if (ruleHead) {
        printf("%s->", RULE_NAME(ruleHead));
        for (Production *production = PRODUCTION_HEAD(ruleHead);
             production != NULL; production = production->next) {
            for (Symbol *symbol = SYMBOL_HEAD(production); symbol != NULL;
                 symbol = symbol->next) {
                printf("%s", SYMBOL_NAME(symbol));
            }
            putchar(production->next ? '|' : '\n');
        }
        PrintRule(ruleHead->next);
    }
}

Rule *CopyRules(const Rule *ruleHeadTemplate) {
    assert(!ruleHeadTemplate || IS_RULE(ruleHeadTemplate));
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
    assert(IS_RULE(ruleTemplate));
    Rule *rule = NewRule(RULE_NAME(ruleTemplate));
    PRODUCTION_HEAD(rule) = CopyProductions(PRODUCTION_HEAD(ruleTemplate));
    return rule;
}

/**
 * @brief 复制一个符号
 *
 * @param symbolTemplate 被复制的符号的指针
 * @return Symbol* 新符号的指针
 */
Symbol *CopySymbol(const Symbol *symbolTemplate) {
    assert(IS_SYMBOL(symbolTemplate));
    Symbol *symbol = NewSymbol(SYMBOL_NAME(symbolTemplate));
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
    assert(IS_PRODUCTION(productionTemplate));
    Production *production = NewProduction();
    for(Symbol *symbol = SYMBOL_HEAD(productionTemplate); symbol != NULL; symbol = symbol->next) {
        SYMBOL_HEAD(production) = AppendNode(SYMBOL_HEAD(production), CopySymbol(symbol));
    }
    return production;
}

Production *CopyProductions(const Production *productionTemplate) {
    if (productionTemplate == NULL) {
        return NULL;
    }
    assert(IS_PRODUCTION(productionTemplate));
    Production *production = CopyProduction(productionTemplate);
    production->next = CopyProductions(productionTemplate->next);
    return production;
}

LinkedNode *DeleteNode(LinkedNode *head, LinkedNode *node) {
    assert(node);
    if (head == node) {
        //free (node);
        return head->next;
    }
    assert(head->type == node->type);
    for (LinkedNode *p = head; p->next != NULL; p = p->next) {
        if (p->next == node) {
            p->next = node->next;
            //free (node);
            break;
        }
    }
    return head;
}
LinkedNode *AppendNode(LinkedNode *head, const LinkedNode *node) {
    if (head == NULL || node == NULL) {
        return node;
    }
    assert(head->type == node->type);
    LinkedNode *p;
    for (p = head; p->next != NULL; p = p->next);
    p->next = node;
    return head;
}
LinkedNode *InsertNode(LinkedNode *head, const LinkedNode *destNode, LinkedNode *node) {
    assert(head && node);
    assert(head->type == node->type);
    if (head == destNode) {
        node->next = head;
        return node;
    }
    if (destNode == NULL) {
        return AppendNode(head, node);
    }
    for (LinkedNode *p = head; p->next; p = p->next) {
        if (p->next == destNode) {
            AppendNode(node, destNode);
            p->next = node;
            return head;
        }
    }
    assert(0);
    return head;
}
LinkedNode *NewNode(NodeType type) {
    LinkedNode *r = calloc(1, sizeof (LinkedNode));
    r->type = type;
    return r;
}
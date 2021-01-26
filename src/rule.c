#include "rule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 初始化文法链表
 *
 * @param ruleTable 原始文法实体指针
 * @param ruleCount 文法的数量
 * @return Rule* 文法链表头指针
 */
Rule *InitRules(const struct RULE_ENTRY *ruleTable, int ruleCount) {
    assert(ruleTable && ruleCount);
    Rule *ruleHead = NULL;
    for (int i = 0; i < ruleCount; i++) {
        ruleHead = AppendNode(ruleHead, NewRule(ruleTable[i].ruleName));
    }
    Rule *rule = ruleHead;
    for (int i = 0; i < ruleCount; i++) {
        for (int j = 0; ruleTable[i].productions[j][0].symbolName[0]; j++) {
            Production *production = NewProduction();
            PRODUCTION_HEAD(rule) =
                AppendNode(PRODUCTION_HEAD(rule), production);
            for (int k = 0; ruleTable[i].productions[j][k].symbolName[0]; k++) {
                Symbol *symbol =
                    NewSymbol(ruleTable[i].productions[j][k].symbolName);
                if (!ruleTable[i].productions[j][k].isToken) {
                    RULE(symbol) = FindRule(ruleHead, SYMBOL_NAME(symbol));
                    if (RULE(symbol) == NULL) {
                        printf("Init rules error, miss rule \"%s\"\n",
                               SYMBOL_NAME(symbol));
                        exit(1);
                    }
                }
                SYMBOL_HEAD(production) =
                    AppendNode(SYMBOL_HEAD(production), symbol);
            }
        }
        rule = rule->next;
    }
    return ruleHead;
}

/**
 * @brief 生成一个文法节点
 * 
 * @param ruleName 文法名称
 * @return Rule* 文法指针
 */
Rule *NewRule(const char *ruleName) {
    assert(ruleName);
    Rule *r = NewNode(RuleNode);
    r->value = calloc(1, sizeof(struct Rule));
    RULE_NAME(r) = strdup(ruleName);
    return r;
}

/**
 * @brief 生成一个符号节点
 * 
 * @param symbolName 符号名称
 * @return Symbol* 符号指针
 */
Symbol *NewSymbol(const char *symbolName) {
    assert(symbolName);
    Symbol *r = NewNode(SymbolNode);
    r->value = calloc(1, sizeof(struct Symbol));
    SYMBOL_NAME(r) = strdup(symbolName);
    return r;
}

/**
 * @brief 生成一个产生式节点
 * 
 * @return Production* 产生式指针
 */
Production *NewProduction() {
    Production *r = NewNode(ProductionNode);
    r->value = calloc(1, sizeof(struct Production));
    return r;
}

/**
 * @brief 从文法链表中使用名字查找文法
 *
 * @param ruleHead 文法链表头指针
 * @param ruleName 文法的名字
 * @return Rule* 如果存在该文法则返回文法指针，否则返回 NULL
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
 * @param ruleHead 文法链表头指针
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

/**
 * @brief 复制文法链表
 * 
 * @param ruleHeadTemplate 
 * @return Rule* 文法链表头指针
 */
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
 * @brief 复制文法
 *
 * @param ruleHead 文法指针
 * @return Rule* 新文法指针
 */
Rule *CopyRule(const Rule *ruleTemplate) {
    assert(IS_RULE(ruleTemplate));
    Rule *rule = NewRule(RULE_NAME(ruleTemplate));
    PRODUCTION_HEAD(rule) = CopyProductions(PRODUCTION_HEAD(ruleTemplate));
    return rule;
}

/**
 * @brief 复制符号
 *
 * @param symbolTemplate 被复制的符号指针
 * @return Symbol* 新符号指针
 */
Symbol *CopySymbol(const Symbol *symbolTemplate) {
    assert(IS_SYMBOL(symbolTemplate));
    Symbol *symbol = NewSymbol(SYMBOL_NAME(symbolTemplate));
    RULE(symbol) = RULE(symbolTemplate);
    return symbol;
}

/**
 * @brief 复制产生式
 *
 * @param productionTemplate 被产生式的产生式指针
 * @return Symbol* 新产生式指针
 */
Production *CopyProduction(const Production *productionTemplate) {
    assert(IS_PRODUCTION(productionTemplate));
    Production *production = NewProduction();
    for (Symbol *symbol = SYMBOL_HEAD(productionTemplate); symbol != NULL;
         symbol = symbol->next) {
        SYMBOL_HEAD(production) =
            AppendNode(SYMBOL_HEAD(production), CopySymbol(symbol));
    }
    return production;
}

/**
 * @brief 复制产生式链表
 * 
 * @param productionTemplate 产生式链表头指针
 * @return Production* 新产生式链表头指针
 */
Production *CopyProductions(const Production *productionTemplate) {
    assert(!productionTemplate || IS_PRODUCTION(productionTemplate));
    if (productionTemplate == NULL) {
        return NULL;
    }
    Production *production = CopyProduction(productionTemplate);
    production->next = CopyProductions(productionTemplate->next);
    return production;
}

/**
 * @brief 从链表中删除一个节点
 * 
 * @param head 链表头指针
 * @param node 节点指针
 * @return LinkedNode* 删除节点后链表头指针
 */
LinkedNode *DeleteNode(LinkedNode *head, LinkedNode *node) {
    assert(head && node && head->type == node->type);
    if (head == node) {
        // free (node);
        return head->next;
    }
    for (LinkedNode *p = head; p->next != NULL; p = p->next) {
        if (p->next == node) {
            p->next = node->next;
            // free (node);
            break;
        }
    }
    return head;
}

/**
 * @brief 从链表中追加节点
 * 
 * @param head 链表头指针
 * @param node 节点指针
 * @return LinkedNode* 追加节点后链表头指针
 */
LinkedNode *AppendNode(LinkedNode *head, const LinkedNode *node) {
    if (head == NULL) {
        return node;
    }
    assert(head && node && head->type == node->type);
    LinkedNode *p;
    for (p = head; p->next != NULL; p = p->next)
        ;
    p->next = node;
    return head;
}

/**
 * @brief 从链表中插入节点
 * 
 * @param head 链表头指针
 * @param destNode 插入位置指针
 * @param node 插入节点指针
 * @return LinkedNode* 插入节点后链表头指针
 */
LinkedNode *InsertNode(LinkedNode *head, const LinkedNode *destNode,
                       LinkedNode *node) {
    assert(head && node && head->type == node->type);
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

/**
 * @brief 生成链表节点
 * 
 * @param type 值类型
 * @return LinkedNode* 节点指针
 */
LinkedNode *NewNode(NodeType type) {
    LinkedNode *r = calloc(1, sizeof(LinkedNode));
    r->type = type;
    return r;
}
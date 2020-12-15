#include "voidtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 输出空表
 *
 * @param pVoidTable 空表的指针
 */
void PrintVoidTable(const VoidTable *pVoidTable) {
    for (int i = 0; i < pVoidTable->ColCount; i++) {
        printf("%s%c", pVoidTable->pTableHead[i],
               i == pVoidTable->ColCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; i < pVoidTable->ColCount; i++) {
        printf("%d%c", pVoidTable->TableRows[0].hasVoid[i],
               i == pVoidTable->ColCount - 1 ? '\n' : '\t');
    }
}

/**
 * @brief 在空表中标记一定能或不能推出 ε 的文法，
 * 并在复制过得文法链表中删去这些文法，
 * 同时还会删去一定不能推出 ε 的产生式。
 *
 * @param pCopiedRulePrePtr 复制后的文法链表的指针的指针
 * @param pVoidTable 空表的指针
 */
void MarkVoidAndNotVoid(Rule **pCopiedRulePrePtr, VoidTable *pVoidTable) {
    Rule *pRule = *pCopiedRulePrePtr, **pRulePrePtr = pCopiedRulePrePtr;
    Production *pProduction, **pProductionPrePtr;
    while (pRule != NULL) {
        pProduction = pRule->pFirstProduction;
        pProductionPrePtr = &pRule->pFirstProduction;

        /**
         * @brief 标记当前文法是否包含 ε 产生式
         */
        int hasVoidProduction = 0;
        while (pProduction != NULL) {

            /**
             * @brief 如果该产生式只有一个符号且是 ε
             * 则标记文法包含 ε 产生式
             */
            if (pProduction->pNextSymbol == NULL &&
                strcmp(pProduction->SymbolName, VOID_SYMBOL) == 0) {
                hasVoidProduction = 1;
                break;
            }

            /**
             * @brief 标记该产生式是否需要被删除
             * 如果该产生式一定不能推出 ε 则该产生式需要被删除
             */
            int deleteThisProduction = 0;
            for (Symbol *pSymble = pProduction; pSymble != NULL;
                 pSymble = pSymble->pNextSymbol) {
                if (pSymble->isToken &&
                    strcmp(pSymble->SymbolName, VOID_SYMBOL) != 0) {
                    deleteThisProduction = 1;
                    break;
                }
            }
            if (deleteThisProduction) {
                *pProductionPrePtr = pProduction->pNextProduction;
                // FreeProduction(pProduction);
                pProduction = *pProductionPrePtr;
            } else {
                pProductionPrePtr = &pProduction->pNextProduction;
                pProduction = pProduction->pNextProduction;
            }
        }

        /**
         * @brief 如果该文法包含 ε 产生式，
         * 则标记该文法可以推出 ε，并删除该文法；
         * 如果该文法指向的产生式链表为空，
         * 则标记该文法不可以推出 ε，并删除该文法。
         */
        if (hasVoidProduction || pRule->pFirstProduction == NULL) {
            *FindHasVoid(pVoidTable, pRule->RuleName) = hasVoidProduction;
            *pRulePrePtr = pRule->pNextRule;
            // FreeRule(pRule);
            pRule = *pRulePrePtr;
        } else {
            pRulePrePtr = &pRule->pNextRule;
            pRule = pRule->pNextRule;
        }
    }
}

/**
 * @brief 生成文法的空表
 *
 * @param pRuleHead 文法链表的头指针
 * @return VoidTable* 生成的空表的指针
 */
VoidTable *GenVoidTable(const Rule *pRuleHead) {
    VoidTable *pVoidTable = malloc(sizeof(VoidTable));
    pVoidTable->pTableHead = GetNonTerminals(pRuleHead);
    for (pVoidTable->ColCount = 0; pVoidTable->pTableHead[pVoidTable->ColCount];
         pVoidTable->ColCount++) {
        pVoidTable->TableRows[0].hasVoid[pVoidTable->ColCount] = -1;
    }
    Rule *pCopiedRule = CopyRule(pRuleHead), *pRule, **pRulePrePtr;

    /**
     * @brief 首先扫描并标记文法链表中一定能或不能推出 ε 的文法，
     * 并在文法链表中删去这戏文法，
     * 同时还会删去一定不能推出 ε 的产生式。
     */
    MarkVoidAndNotVoid(&pCopiedRule, pVoidTable);
    int isChange;
    do {
        isChange = 0;
        pRule = pCopiedRule;
        pRulePrePtr = &pCopiedRule;
        while (pRule != NULL) {

            /**
             * @brief 标记该文法剩余的所有产生式都不能推出 ε
             */
            int allProductionNotVoid = 1;

            /**
             * @brief 标记该文法含有一条可以推出 ε 的产生式
             */
            int hasVoidProduction;
            for (Production *pProduction = pRule->pFirstProduction;
                 pProduction != NULL;
                 pProduction = pProduction->pNextProduction) {
                Symbol *pSymble = pProduction,
                       **pSymblePrePtr = &pRule->pFirstProduction;
                hasVoidProduction = 0;
                while (pSymble != NULL) {
                    switch (*FindHasVoid(pVoidTable, pSymble->SymbolName)) {
                    case 1:

                        /**
                         * @brief 这个符号对应的文法可以推出 ε
                         * 则判断该产生式是否只剩此符号
                         * 如果只剩此符号说明该文法包含可以推出 ε 的产生式
                         * 否则在此产生式中删去该符号
                         */
                        if (pSymble->pNextSymbol == NULL &&
                            pSymblePrePtr == &pRule->pFirstProduction) {
                            hasVoidProduction = 1;
                        } else {
                            *pSymblePrePtr = pSymble->pNextSymbol;
                            if (pSymble->pNextSymbol) {
                                pSymble->pNextSymbol->pNextProduction =
                                    pSymble->pNextProduction;
                            }
                            // FreeSymble(pSymble);
                            pSymble = *pSymblePrePtr;
                        }
                        break;
                    case -1:
                        /**
                         * @brief 不知道这个符号对应的文法是否可以推出 ε
                         * 则要取消对该文法剩余的所有产生式都不能推出 ε 的标记
                         */
                        allProductionNotVoid = 0;
                    case 0:
                        pSymblePrePtr = &pSymble->pNextSymbol;
                        pSymble = pSymble->pNextSymbol;
                        break;
                    }
                    if (hasVoidProduction) {
                        break;
                    }
                }
                if (hasVoidProduction) {
                    break;
                }
            }

            /**
             * @brief 如果这个文法包含可以推出 ε
             * 的产生式或者所有产生式都不能推出 ε
             * 则在空表中标记该产生式，并且在文法链表中删去该文法
             */
            if (allProductionNotVoid || hasVoidProduction) {
                isChange = 1;
                *FindHasVoid(pVoidTable, pRule->RuleName) = hasVoidProduction;
                *pRulePrePtr = pRule->pNextRule;
                // FreeRule(pRule);
                pRule = *pRulePrePtr;
            } else {
                pRulePrePtr = &pRule->pNextRule;
                pRule = pRule->pNextRule;
            }
        }
    } while (isChange);
}

/**
 * @brief 在空表中根据文法名字找到值的位置
 *
 * @param pTable 空表的指针
 * @param RuleName 文法的名字
 * @return int* 值在空表中的位置
 */
int *FindHasVoid(VoidTable *pTable, const char *RuleName) {
    for (int i = 0; i < pTable->ColCount; i++) {
        if (strcmp(pTable->pTableHead[i], RuleName) == 0) {
            return pTable->TableRows[0].hasVoid + i;
        }
    }
    return 0;
}

/**
 * @brief 从文法中提取所有的非终结符
 *
 * @param pRule 文法链表的头指针
 * @return char** 非终结符数组
 */
char **GetNonTerminals(const Rule *pRuleHead) {
    char **pNonTerminals = calloc(32, sizeof(char *));
    for (int i = 0; pRuleHead != NULL; pRuleHead = pRuleHead->pNextRule, i++) {
        pNonTerminals[i] = malloc(MAX_STR_LENGTH);
        strcpy(pNonTerminals[i], pRuleHead->RuleName);
    }
    return pNonTerminals;
}

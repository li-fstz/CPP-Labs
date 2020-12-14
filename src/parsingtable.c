#include "parsingtable.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"

/**
 * @brief 生成 Select 集
 *
 * @param pRuleHead 文法链表的头指针
 * @param pVoidTable 空表的指针
 * @param pFirstSetList First 集的指针
 * @param pFollowSetList Follow 集的指针
 * @param pSelectSetList Select 集的指针
 */
void GenSelectSetList(const Rule *pRuleHead, const VoidTable *pVoidTable,
                      const SetList *pFirstSetList,
                      const SetList *pFollowSetList,
                      SelectSetList *pSelectSetList) {
    for (; pRuleHead != NULL; pRuleHead = pRuleHead->pNextRule) {
        for (Production *pProduction = pRuleHead->pFirstProduction;
             pProduction != NULL; pProduction = pProduction->pNextProduction) {

            /**
             * @brief Select 集中的每个子集对应文法中的每一条产生式
             * 首先将这条产生式对应的 First 集中的符号加入到 Select 子集中
             */
            AddOneSelectSet(pSelectSetList, pRuleHead, pProduction);
            SelectSet *pSelectSet =
                pSelectSetList->Sets + pSelectSetList->nSetCount - 1;
            for (Symbol *pSymbol = pProduction; pSymbol != NULL;
                 pSymbol = pSymbol->pNextSymbol) {
                if (pSymbol->isToken) {
                    AddTerminalToSelectSet(pSelectSet, pSymbol->SymbolName);
                    if (strcmp(pSymbol->SymbolName, VOID_SYMBOL) != 0) {
                        break;
                    }
                } else {
                    AddSetToSelectSet(
                        pSelectSet, GetSet(pFirstSetList, pSymbol->SymbolName));
                    if (!*FindHasVoid(pVoidTable, pSymbol->SymbolName)) {
                        break;
                    }
                }
            }

            /**
             * @brief 如果该产生式对应的 First 子集中包含 ε
             * 则去除 ε，并且向其中加入文法对应的 Follow 子集中的符号
             */
            if (RemoveVoidFromSelectSet(pSelectSet)) {
                AddSetToSelectSet(pSelectSet,
                                  GetSet(pFollowSetList, pRuleHead->RuleName));
            }
        }
    }
}

/**
 * @brief 生成预测分析表
 *
 * @param pRuleHead 文法链表的头指针
 * @param pSelectSetList Select 集的指针
 * @param pParsingTable 预测分析标的指针
 */
void GenParsingTable(const Rule *pRuleHead, const SelectSetList *pSelectSetList,
                     ParsingTable *pParsingTable) {
    memset(pParsingTable, 0, sizeof(ParsingTable));
    pParsingTable->pTableHead = GetTerminals(pRuleHead);
    for (pParsingTable->ColCount = 0;
         pParsingTable->pTableHead[++pParsingTable->ColCount];)
        ;
    for (int i = 0; pRuleHead != NULL; pRuleHead = pRuleHead->pNextRule, i++) {
        pParsingTable->TableRows[i].pRule = pRuleHead;
    }
    for (int i = 0; i < pSelectSetList->nSetCount; i++) {
        for (int j = 0; j < pSelectSetList->Sets[i].nTerminalCount; j++) {
            Production **pFindProduction =
                FindProduction(pParsingTable, pSelectSetList->Sets[i].pRule,
                               pSelectSetList->Sets[i].Terminals[j]);
            if (*pFindProduction) {
                puts("该文法不是 LL(1) 文法！");
            } else {
                *pFindProduction = pSelectSetList->Sets[i].pProduction;
            }
        }
    }
}

/**
 * @brief 输出 Select 集
 *
 * @param pSelectSetList Select 集的指针
 */
void PrintSelectSetList(const SelectSetList *pSelectSetList) {
    printf("\nThe Select Set:\n");
    for (int i = 0; i < pSelectSetList->nSetCount; i++) {
        printf("Select(%s->", pSelectSetList->Sets[i].pRule->RuleName);
        PrintProduction(pSelectSetList->Sets[i].pProduction);
        printf(") = { ");
        for (int j = 0; j < pSelectSetList->Sets[i].nTerminalCount; j++) {
            printf("%s%s", pSelectSetList->Sets[i].Terminals[j],
                   j == pSelectSetList->Sets[i].nTerminalCount - 1 ? " }\n"
                                                                   : ", ");
        }
    }
}

/**
 * @brief 输出预测分析表
 *
 * @param pParsingTable 预测分析表的指针
 */
void PrintParsingTable(const ParsingTable *pParsingTable) {
    putchar('\t');
    for (int i = 0; i < pParsingTable->ColCount; i++) {
        printf("%s%c", pParsingTable->pTableHead[i],
               i == pParsingTable->ColCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; pParsingTable->TableRows[i].pRule; i++) {
        printf("%s\t", pParsingTable->TableRows[i].pRule->RuleName);
        for (int j = 0; j < pParsingTable->ColCount; j++) {
            if (pParsingTable->TableRows[i].Productions[j]) {
                PrintProduction(pParsingTable->TableRows[i].Productions[j]);
            }
            putchar(j == pParsingTable->ColCount - 1 ? '\n' : '\t');
        }
    }
}

/**
 * @brief 从 Select 子集中删除 $
 *
 * @param pSelectSet Select 子集指针
 * @return int 对子集是否有修改
 */
int RemoveVoidFromSelectSet(SelectSet *pSelectSet) {
    for (int i = 0; i < pSelectSet->nTerminalCount; i++) {
        if (strcmp(pSelectSet->Terminals[i], VOID_SYMBOL) == 0) {
            if (i != pSelectSet->nTerminalCount - 1) {
                memmove(pSelectSet->Terminals + i,
                        pSelectSet->Terminals + i + 1,
                        sizeof(pSelectSet->Terminals[0]) *
                            (pSelectSet->nTerminalCount - i - 1));
            }
            pSelectSet->nTerminalCount--;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 在 Select 集中查找一个 Select 子集
 *
 * @param pSetList Select 集指针
 * @param pProduction 产生式的指针
 * @return SelectSet* 如果找到则返回子集的指针，否则返回 NULL
 */
SelectSet *GetSelectSet(const SelectSetList *pSetList,
                        const Production *pProduction) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (pSetList->Sets[i].pProduction == pProduction) {
            return pSetList->Sets + i;
        }
    }
    return NULL;
}

/**
 * @brief 在 Select 子集中添加一个产生式子集
 *
 * @param pSetList Select 子集的指针
 * @param pRule 指向产生式的文法的指针
 * @param pProduction 产生式的指针
 */
void AddOneSelectSet(SelectSetList *pSetList, const Rule *pRule,
                     const Production *pProduction) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (pSetList->Sets[i].pProduction == pProduction) {
            return;
        }
    }
    pSetList->Sets[pSetList->nSetCount].pRule = pRule;
    pSetList->Sets[pSetList->nSetCount++].pProduction = pProduction;
}

/**
 * @brief 添加一个终结符到 Select 子集
 *
 * @param pSet Select 子集的指针
 * @param Terminal 终结符
 * @return int 是否对子集有修改
 */
int AddTerminalToSelectSet(SelectSet *pSet, const char *Terminal) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminals[i], Terminal) == 0) {
            return 0;
        }
    }
    strcpy(pSet->Terminals[pSet->nTerminalCount++], Terminal);
    return 1;
}

/**
 * @brief 将源 First 子集或 Follow 子集中的所有终结符添加至目标 Select 子集中
 *
 * @param pDesSet 目标 Select 子集
 * @param pSrcSet 源 First 子集或 Follow 子集
 * @return int 是否对目标 Select 子集有修改
 */
int AddSetToSelectSet(SelectSet *pDesSet, const Set *pSrcSet) {
    int flag = 0;
    for (int i = 0; i < pSrcSet->nTerminalCount; i++) {
        if (AddTerminalToSelectSet(pDesSet, pSrcSet->Terminals[i])) {
            flag = 1;
        }
    }
    return flag;
}

/**
 * @brief 从文法中提取所有的终结符
 *
 * @param pRuleHead 文法链表的头指针
 * @return char** 终结符数组
 */
char **GetTerminals(const Rule *pRuleHead) {
    char **pTerminals = calloc(32, sizeof(char *));
    int t;
    for (; pRuleHead != NULL; pRuleHead = pRuleHead->pNextRule) {
        for (Production *pProduction = pRuleHead->pFirstProduction;
             pProduction != NULL; pProduction = pProduction->pNextProduction) {
            for (Symbol *pSymbol = pProduction; pSymbol != NULL;
                 pSymbol = pSymbol->pNextSymbol) {
                if (!pSymbol->isToken) {
                    continue;
                }
                for (t = 0; pTerminals[t] != NULL; t++) {
                    if (strcmp(pTerminals[t], pSymbol->SymbolName) == 0) {
                        break;
                    }
                }
                if (pTerminals[t] == NULL) {
                    pTerminals[t] = malloc(MAX_STR_LENGTH);
                    strcpy(pTerminals[t], pSymbol->SymbolName);
                }
            }
        }
    }
    pTerminals[t + 1] = END_SYMBOL;
    return pTerminals;
}

/**
 * @brief 从预测分析表中根据文法和终结符查找产生式的位置
 *
 * @param pParsingTable 预测分析表的指针
 * @param pRule 文法的指针
 * @param Terminal 终结符
 * @return Symbol** 如果存在则返回该产生式在预测分析表中的位置，否则返回 NULL
 */
Production **FindProduction(const ParsingTable *pParsingTable,
                            const Rule *pRule, const char *Terminal) {
    int i;
    for (i = 0; i < pParsingTable->ColCount; i++) {
        if (strcmp(pParsingTable->pTableHead[i], Terminal) == 0) {
            break;
        }
    }
    for (int j = 0; pParsingTable->TableRows[j].pRule; j++) {
        if (pParsingTable->TableRows[j].pRule == pRule) {
            return pParsingTable->TableRows[j].Productions + i;
        }
    }
    return NULL;
}

/**
 * @brief 输出产生式
 *
 * @param pProduction 产生式的指针
 */
void PrintProduction(const Production *pProduction) {
    for (; pProduction != NULL; pProduction = pProduction->pNextSymbol) {
        printf(pProduction->SymbolName);
    }
}

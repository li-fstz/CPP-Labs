#include "parsingtable.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"
void Select(Rule *pRule, VoidTable *pVoidTable, SetList *pFirstSetList,
            SetList *pFollowSetList, SelectSetList *pSelectSetList) {
    for (; pRule != NULL; pRule = pRule->pNextRule) {
        for (RuleSymbol *pSelect = pRule->pFirstSymbol; pSelect != NULL;
             pSelect = pSelect->pOther) {
            AddOneSelectSet(pSelectSetList, pRule, pSelect);
            SelectSet *pSelectSet = GetSelectSet(pSelectSetList, pSelect);
            for (RuleSymbol *pSymbol = pSelect; pSymbol != NULL;
                 pSymbol = pSymbol->pNextSymbol) {
                if (pSymbol->isToken) {
                    AddTerminalToSelectSet(pSelectSet, pSymbol->SymbolName);
                    break;
                } else {
                    AddSetToSelectSet(
                        pSelectSet, GetSet(pFirstSetList, pSymbol->SymbolName));
                    if (!*FindHasVoid(pVoidTable, pSymbol->SymbolName)) {
                        break;
                    }
                }
            }
            if (RemoveVoidFromSelectSet(pSelectSet)) {
                AddSetToSelectSet(pSelectSet,
                                  GetSet(pFollowSetList, pRule->RuleName));
            }
        }
    }
}

void GenParsingTable(Rule *pRule, ParsingTable *pParsingTable,
                     SelectSetList *pSelectSetList) {
    memset(pParsingTable, 0, sizeof(ParsingTable));
    pParsingTable->pTableHead = GetTerminals(pRule);
    for (pParsingTable->ColCount = 0;
         pParsingTable->pTableHead[++pParsingTable->ColCount];)
        ;
    for (int i = 0; pRule != NULL; pRule = pRule->pNextRule, i++) {
        pParsingTable->TableRows[i].pRule = pRule;
    }
    for (int i = 0; i < pSelectSetList->nSetCount; i++) {
        for (int j = 0; j < pSelectSetList->Sets[i].nTerminalCount; j++) {
            RuleSymbol **pFindSelect =
                FindSelect(pParsingTable, pSelectSetList->Sets[i].pRule,
                           pSelectSetList->Sets[i].Terminal[j]);
            if (*pFindSelect) {
                puts("该文法不是 LL(1) 文法！");
            } else {
                *pFindSelect = pSelectSetList->Sets[i].pSelect;
            }
        }
    }
}

void PrintSelectSet(SelectSetList *pSelectSetList) {
    printf("\nThe Select Set:\n");
    for (int i = 0; i < pSelectSetList->nSetCount; i++) {
        printf("Select(%s->", pSelectSetList->Sets[i].pRule->RuleName);
        PrintSelect(pSelectSetList->Sets[i].pSelect);
        printf(") = { ");
        for (int j = 0; j < pSelectSetList->Sets[i].nTerminalCount; j++) {
            printf("%s%s", pSelectSetList->Sets[i].Terminal[j],
                   j == pSelectSetList->Sets[i].nTerminalCount - 1 ? " }\n"
                                                                   : ", ");
        }
    }
}

void PrintParsingTable(ParsingTable *pParsingTable) {
    putchar('\t');
    for (int i = 0; i < pParsingTable->ColCount; i++) {
        printf("%s%c", pParsingTable->pTableHead[i],
               i == pParsingTable->ColCount - 1 ? '\n' : '\t');
    }
    for (int i = 0; pParsingTable->TableRows[i].pRule; i++) {
        printf("%s\t", pParsingTable->TableRows[i].pRule->RuleName);
        for (int j = 0; j < pParsingTable->ColCount; j++) {
            if (pParsingTable->TableRows[i].Select[j]) {
                PrintSelect(pParsingTable->TableRows[i].Select[j]);
            }
            putchar(j == pParsingTable->ColCount - 1 ? '\n' : '\t');
        }
    }
}
int RemoveVoidFromSelectSet(SelectSet *pSet) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminal[i], VoidSymbol) == 0) {
            if (i != pSet->nTerminalCount - 1) {
                memmove(
                    pSet->Terminal + i, pSet->Terminal + i + 1,
                    sizeof(pSet->Terminal[0]) * (pSet->nTerminalCount - i - 1));
            }
            pSet->nTerminalCount--;
            return 1;
        }
    }
    return 0;
}
SelectSet *GetSelectSet(SelectSetList *pSetList, const RuleSymbol *pSelect) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (pSetList->Sets[i].pSelect == pSelect) {
            return pSetList->Sets + i;
        }
    }
    return NULL;
}
void AddOneSelectSet(SelectSetList *pSetList, const Rule *pRule,
                     const RuleSymbol *pSelect) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (pSetList->Sets[i].pSelect == pSelect) {
            return;
        }
    }
    pSetList->Sets[pSetList->nSetCount].pRule = pRule;
    pSetList->Sets[pSetList->nSetCount++].pSelect = pSelect;
}
int AddTerminalToSelectSet(SelectSet *pSet, const char *pTerminal) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminal[i], pTerminal) == 0) {
            return 0;
        }
    }
    strcpy(pSet->Terminal[pSet->nTerminalCount++], pTerminal);
    return 1;
}
int AddSetToSelectSet(SelectSet *pDesSet, const Set *pSrcSet) {
    int flag = 0;
    for (int i = 0; i < pSrcSet->nTerminalCount; i++) {
        if (AddTerminalToSelectSet(pDesSet, pSrcSet->Terminal[i])) {
            flag = 1;
        }
    }
    return flag;
}
char **GetTerminals(Rule *pRule) {
    char **pTerminals = calloc(32, sizeof(char *));
    int t;
    for (; pRule != NULL; pRule = pRule->pNextRule) {
        for (RuleSymbol *pSelect = pRule->pFirstSymbol; pSelect != NULL;
             pSelect = pSelect->pOther) {
            for (RuleSymbol *pSymbol = pSelect; pSymbol != NULL;
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
    pTerminals[t + 1] = EndSymbol;
    return pTerminals;
}
RuleSymbol **FindSelect(ParsingTable *pParsingTable, Rule *pRule,
                        const char *Terminal) {
    int i;
    for (i = 0; i < pParsingTable->ColCount; i++) {
        if (strcmp(pParsingTable->pTableHead[i], Terminal) == 0) {
            break;
        }
    }
    for (int j = 0; pParsingTable->TableRows[j].pRule; j++) {
        if (pParsingTable->TableRows[j].pRule == pRule) {
            return pParsingTable->TableRows[j].Select + i;
        }
    }
    return NULL;
}
void PrintSelect(RuleSymbol *pSelect) {
    printf("->");
    for (; pSelect != NULL; pSelect = pSelect->pNextSymbol) {
        printf(pSelect->SymbolName);
    }
}

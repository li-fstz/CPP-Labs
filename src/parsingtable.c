#include "parsingtable.h"

#include <stdlib.h>
#include <string.h>

#include "follow.h"
void GenSelectSet(Rule *pRule, VoidTable *pVoidTable, SetList *pFirstSetList,
                  SetList *pFollowSetList, SelectSetList *pSelectSetList) {
    for (; pRule != NULL; pRule = pRule->pNextRule) {
        for (Production *pProduction = pRule->pFirstProduction;
             pProduction != NULL; pProduction = pProduction->pNextProduction) {
            AddOneSelectSet(pSelectSetList, pRule, pProduction);
            SelectSet *pSelectSet = GetSelectSet(pSelectSetList, pProduction);
            for (Symbol *pSymbol = pProduction; pSymbol != NULL;
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
            Symbol **pFindProduction =
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

void PrintSelectSet(SelectSetList *pSelectSetList) {
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

void PrintParsingTable(ParsingTable *pParsingTable) {
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
int RemoveVoidFromSelectSet(SelectSet *pSet) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminals[i], VOID_SYMBOL) == 0) {
            if (i != pSet->nTerminalCount - 1) {
                memmove(pSet->Terminals + i, pSet->Terminals + i + 1,
                        sizeof(pSet->Terminals[0]) *
                            (pSet->nTerminalCount - i - 1));
            }
            pSet->nTerminalCount--;
            return 1;
        }
    }
    return 0;
}
SelectSet *GetSelectSet(SelectSetList *pSetList, const Symbol *pProduction) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (pSetList->Sets[i].pProduction == pProduction) {
            return pSetList->Sets + i;
        }
    }
    return NULL;
}
void AddOneSelectSet(SelectSetList *pSetList, const Rule *pRule,
                     const Symbol *pProduction) {
    for (int i = 0; i < pSetList->nSetCount; i++) {
        if (pSetList->Sets[i].pProduction == pProduction) {
            return;
        }
    }
    pSetList->Sets[pSetList->nSetCount].pRule = pRule;
    pSetList->Sets[pSetList->nSetCount++].pProduction = pProduction;
}
int AddTerminalToSelectSet(SelectSet *pSet, const char *pTerminal) {
    for (int i = 0; i < pSet->nTerminalCount; i++) {
        if (strcmp(pSet->Terminals[i], pTerminal) == 0) {
            return 0;
        }
    }
    strcpy(pSet->Terminals[pSet->nTerminalCount++], pTerminal);
    return 1;
}
int AddSetToSelectSet(SelectSet *pDesSet, const Set *pSrcSet) {
    int flag = 0;
    for (int i = 0; i < pSrcSet->nTerminalCount; i++) {
        if (AddTerminalToSelectSet(pDesSet, pSrcSet->Terminals[i])) {
            flag = 1;
        }
    }
    return flag;
}
char **GetTerminals(Rule *pRule) {
    char **pTerminals = calloc(32, sizeof(char *));
    int t;
    for (; pRule != NULL; pRule = pRule->pNextRule) {
        for (Production *pProduction = pRule->pFirstProduction;
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
Symbol **FindProduction(ParsingTable *pParsingTable, Rule *pRule,
                        const char *Terminal) {
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
void PrintProduction(Symbol *pProduction) {
    for (; pProduction != NULL; pProduction = pProduction->pNextSymbol) {
        printf(pProduction->SymbolName);
    }
}

#include "pickupleftfactor.h"

#include <stdio.h>
#include <string.h>

#include "rule.h"

/*
功能：
        提取左因子。

参数：
        pHead -- 文法的头指针。
*/
void PickupLeftFactor(Rule *pHead) {
    Rule *pRule;                  // Rule 游标
    int isChange;                 // Rule 是否被提取左因子的标志
    RuleSymbol *pSelectTemplate;  // Select 游标
    Rule *pNewRule;               // Rule 指针
    RuleSymbol *pSelect;          // Select 游标

    do {
        isChange = 0;

        for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
            // 取 Rule 中的一个 Select 作为模板，调用 LeftFactorMaxLength
            // 函数确定左因子的最大长度
            int Count = 0;
            for (pSelectTemplate = pRule->pFirstSymbol; pSelectTemplate != NULL;
                 pSelectTemplate = pSelectTemplate->pOther) {
                if ((Count = LeftFactorMaxLength(pSelectTemplate)) > 0) break;
            }

            // 忽略没用左因子的 Rule
            if (Count == 0) continue;

            pNewRule = CreateRule(pRule->RuleName);  // 创建新 Rule
            GetUniqueRuleName(pRule, pNewRule->RuleName);
            isChange = 1;  // 设置标志

            // 调用 AddSelectToRule 函数把模板左因子之后的部分加到新 Rule 的末尾
            // 将模板左因子之后的部分替换为指向新 Rule 的非终结符
            AddSelectToRule(pNewRule, GetSymbol(pSelectTemplate, Count));
            RuleSymbol *pTmp = CreateSymbol();
            pTmp->isToken = 0;
            pTmp->pRule = pNewRule;
            GetSymbol(pSelectTemplate, Count - 1)->pNextSymbol = pTmp;

            // 从模板之后的位置循环查找包含左因子的 Select，并提取左因子
            pSelect = pSelectTemplate->pOther;
            RuleSymbol **pSelectPtr = &pSelectTemplate->pOther;
            while (pSelect != NULL) {
                if (NeedPickup(pSelectTemplate, Count,
                               pSelect))  // Select 包含左因子
                {
                    // 调用 AddSelectToRule 函数把左因子之后的部分加到新 Rule
                    // 的末尾 将该 Select 从 Rule 中移除，释放内存，并移动游标
                    AddSelectToRule(pNewRule, GetSymbol(pSelect, Count));
                    *pSelectPtr = pSelect->pOther;
                    GetSymbol(pSelect, Count - 1)->pNextSymbol = NULL;
                    FreeSelect(pSelect);
                    pSelect = *pSelectPtr;
                } else  // Select 不包含左因子
                {
                    // 移动游标
                    pSelectPtr = &pSelect->pOther;
                    pSelect = pSelect->pOther;
                }
            }

            // 将新 Rule 加入到文法链表
            while (pRule->pNextRule) {
                pRule = pRule->pNextRule;
            }
            pRule->pNextRule = pNewRule;
        }

    } while (isChange == 1);
}

/*
功能：
        根据下标找到 Select 中的一个 Symbol。

参数：
        pSelect -- Select 指针。
        index -- 下标。

返回值：
        如果存在，返回找到的 Symbol 指针，否则返回 NULL。
*/
RuleSymbol *GetSymbol(RuleSymbol *pSelect, int index) {
    int i = 0;
    RuleSymbol *pRuleSymbol;
    for (pRuleSymbol = pSelect, i = 0; pRuleSymbol != NULL;
         pRuleSymbol = pRuleSymbol->pNextSymbol, i++) {
        if (i == index) {
            return pRuleSymbol;
        }
    }

    return NULL;
}

/*
功能：
        以 SelectTemplate 为模板，确定左因子的最大长度。

参数：
        pSelectTemplate -- 作为模板的 Select 指针。

返回值：
        左因子的最大长度，如果返回 0 说明不存在左因子。
*/
int LeftFactorMaxLength(RuleSymbol *pSelectTemplate) {
    int maxLength = 0;
    RuleSymbol *pCmpSelectTemplate = pSelectTemplate->pOther;
    while (pCmpSelectTemplate) {
        int length = 0;
        for (int i = 0;; i++) {
            RuleSymbol *pA = GetSymbol(pSelectTemplate, i),
                       *pB = GetSymbol(pCmpSelectTemplate, i);
            if (!pA || !pB || !SymbolCmp(pA, pB)) {
                length = i;
                break;
            }
        }
        maxLength = length > maxLength ? length : maxLength;
        pCmpSelectTemplate = pCmpSelectTemplate->pOther;
    }
    return maxLength;
}

/*
功能：
        比较两个相同类型(同为终结符或同为非终结符)的 Symbol 是否具有相同的名字。

参数：
        pSymbol1 -- Symbol 指针。
        pSymbol2 -- Symbol 指针。

返回值：
        相同返回 1，不同返回 0。
*/
int SymbolCmp(RuleSymbol *pSymbol1, RuleSymbol *pSymbol2) {
    if (pSymbol1->isToken == pSymbol2->isToken) {
        if (pSymbol1->isToken) {
            return strcmp(pSymbol1->SymbolName, pSymbol2->SymbolName) == 0;
        } else {
            return pSymbol1->pRule == pSymbol2->pRule;
        }
    } else {
        return 0;
    }
}

/*
功能：
        取文法中的一个 Select 与 SelectTemplate 进行比较，判断该 Select
是否需要提取左因子。

参数：
        pSelectTemplate -- 作为模板的 Select 指针。
        Count -- SelectTemplate 中已确定的左因子的数量。
        pSelect -- Select 指针。

返回值：
        如果 Select 包含左因子返回 1，否则返回 0。
*/
int NeedPickup(RuleSymbol *pSelectTemplate, int Count, RuleSymbol *pSelect) {
    for (int i = 0; i < Count; i++) {
        RuleSymbol *pA = GetSymbol(pSelectTemplate, i),
                   *pB = GetSymbol(pSelect, i);
        if (!pA || !pB || !SymbolCmp(pA, pB)) {
            return 0;
        }
    }
    return 1;
}

/*
功能：
        将一个 Select 加入到文法末尾，当 Select 为 NULL
时就将一个ε终结符加入到文法末尾。

参数：
        pRule -- 文法指针。
        pNewSelect -- Select 指针。
*/
void AddSelectToRule(Rule *pRule, RuleSymbol *pNewSelect) {
    if (!pNewSelect) {
        pNewSelect = CreateSymbol();
        pNewSelect->isToken = 1;
        strcpy(pNewSelect->SymbolName, VoidSymbol);
    }
    if (pRule->pFirstSymbol) {
        RuleSymbol *pTmp = pRule->pFirstSymbol;
        while (pTmp->pOther) {
            pTmp = pTmp->pOther;
        }
        pTmp->pOther = pNewSelect;
    } else {
        pRule->pFirstSymbol = pNewSelect;
    }
}

/*
功能：
        将 pRuleName 与文法中的其他 RuleName 比较, 如果相同就增加一个后缀。

参数：
        pHead -- Rule 链表的头指针。
        pRuleName -- Rule 的名字。
*/
void GetUniqueRuleName(Rule *pHead, char *pRuleName) {
    Rule *pRuleCursor = pHead;
    for (; pRuleCursor != NULL;) {
        if (0 == strcmp(pRuleCursor->RuleName, pRuleName)) {
            strcat(pRuleName, Postfix);
            pRuleCursor = pHead;
            continue;
        }
        pRuleCursor = pRuleCursor->pNextRule;
    }
}

/*
功能：
        释放一个 Select 的内存。

参数：
        pSelect -- 需要释放的 Select 的指针。
*/
void FreeSelect(RuleSymbol *pSelect) {
    if (pSelect->pNextSymbol) {
        FreeSelect(pSelect->pNextSymbol);
    } else {
        free(pSelect);
    }
}

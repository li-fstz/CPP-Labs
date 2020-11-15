#include "removeleftrecursion.h"
/*
功能：
        判断当前 Rule 中的一个 Symbol 是否需要被替换。
        如果 Symbol 是一个非终结符，且 Symbol 对应的
        Rule 在当前 Rule 之前，就需要被替换。

参数：
        pCurRule -- 当前 Rule 的指针。
        pSymbol -- Symbol 指针。

返回值：
        需要替换返回 1。
        不需要替换返回 0。
*/
int SymbolNeedReplace(const Rule* pCurRule, const RuleSymbol* pSymbol) {
    if (pSymbol->isToken) {
        return 0;
    } else {
        Rule* pTmp = pSymbol->pRule->pNextRule;
        while (pTmp) {
            if (pCurRule == pTmp) {
                return 1;
            } else {
                pTmp = pTmp->pNextRule;
            }
        }
        return 0;
    }
}

/*
功能：
        替换一个 Select 的第一个 Symbol。

参数：
        pSelectTemplate -- 需要被替换的 Select 指针。

返回值：
        替换后获得的新 Select 的指针。
        注意，替换后可能会有一个新的 Select，
        也可能会有多个 Select 链接在一起。
*/
RuleSymbol* ReplaceSelect(const RuleSymbol* pSelectTemplate) {
    RuleSymbol *pSelectsOfFirstSymble =
                   CopySelect(pSelectTemplate->pRule->pFirstSymbol),
               *pTmpSelect = pSelectsOfFirstSymble;
    while (pTmpSelect) {
        RuleSymbol* pTmpSymble = pTmpSelect;
        while (pTmpSymble->pNextSymbol) {
            pTmpSymble = pTmpSymble->pNextSymbol;
        }
        pTmpSymble->pNextSymbol = CopySelect(pSelectTemplate->pNextSymbol);
        if (pTmpSelect->pOther) {
            pTmpSelect->pOther = CopySelect(pTmpSelect->pOther);
        }
        pTmpSelect = pTmpSelect->pOther;
    }
    return pSelectsOfFirstSymble;
}

/*
功能：
        释放一个 Select 的内存。

参数：
        pSelect -- 需要释放的 Select 的指针。
*/
void FreeSelect(RuleSymbol* pSelect) {
    if (pSelect->pNextSymbol) {
        FreeSelect(pSelect->pNextSymbol);
    } else {
        free(pSelect);
    }
}

/*
功能：
        判断一条 Rule 是否存在左递归。

参数：
        prRule -- Rule 指针。

返回值：
        存在返回 1。
        不存在返回 0。
*/
int SelectHasLeftRecursion(RuleSymbol* pSelect, Rule* pRule) {
    if (pSelect->isToken) {
        return 0;
    } else {
        return pSelect->pRule == pRule;
    }
}
int RuleHasLeftRecursion(Rule* pRule) {
    RuleSymbol* pTmp = pRule->pFirstSymbol;
    while (pTmp) {
        if (SelectHasLeftRecursion(pTmp, pRule)) {
            return 1;
        }
        pTmp = pTmp->pOther;
    }
    return 0;
}

/*
功能：
        将一个 Symbol 添加到 Select 的末尾。

参数：
        pSelect -- Select 指针。
        pNewSymbol -- Symbol 指针。
*/
void AddSymbolToSelect(RuleSymbol* pSelect, RuleSymbol* pNewSymbol) {
    while (pSelect->pNextSymbol) {
        pSelect = pSelect->pNextSymbol;
    }
    pSelect->pNextSymbol = pNewSymbol;
}

/*
功能：
        将一个 Select 加入到文法末尾，当 Select 为 NULL
时就将一个ε终结符加入到文法末尾。

参数：
        pRule -- 文法指针。
        pNewSelect -- Select 指针。
*/
void AddSelectToRule(Rule* pRule, RuleSymbol* pNewSelect) {
    if (!pNewSelect) {
        pNewSelect = CreateSymbol();
        pNewSelect->isToken = 1;
        strcpy(pNewSelect->SymbolName, VoidSymbol);
    }
    if (pRule->pFirstSymbol) {
        RuleSymbol* pTmp = pRule->pFirstSymbol;
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
        消除左递归。

参数：
        pHead -- 文法链表的头指针。
*/
void RemoveLeftRecursion(Rule* pHead) {
    Rule* pRule;                 // Rule 游标
    RuleSymbol* pSelect;         // Select 游标
    Rule* pNewRule;              // Rule 指针
    int isChange;                // Rule 是否被替换的标记
    RuleSymbol** pSelectPrePtr;  // Symbol 指针的指针

    for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule) {
        //
        // 替换
        //
        do {
            isChange = 0;

            // 在 Rule 的所有 Select 中查找是否需要替换
            for (pSelect = pRule->pFirstSymbol,
                pSelectPrePtr = &pRule->pFirstSymbol;
                 pSelect != NULL;
                 pSelectPrePtr = &pSelect->pOther, pSelect = pSelect->pOther) {
                if (SymbolNeedReplace(
                        pRule,
                        pSelect))  // 判断 Select 的第一个 Symbol 是否需要替换
                {
                    isChange = 1;

                    // 调用 ReplaceSelect 函数，替换 Select 的第一个 Symbol
                    // 后得到新的 Selects
                    RuleSymbol* pNewSelects = ReplaceSelect(pSelect);

                    // 使用新的 Selects 替换原有的 Select，并调用 FreeSelect
                    // 函数释放原有的 Select 内存
                    *pSelectPrePtr = pNewSelects;
                    while (pNewSelects->pOther) {
                        pNewSelects = pNewSelects->pOther;
                    }
                    pNewSelects->pOther = pSelect->pOther;
                    FreeSelect(pSelect);
                    break;
                }

                if (isChange) break;
            }
        } while (isChange);

        // 忽略没有左递归的 Rule;
        if (!RuleHasLeftRecursion(pRule)) continue;

        //
        // 消除左递归
        //
        pNewRule = CreateRule(pRule->RuleName);  // 创建新 Rule
        strcat(pNewRule->RuleName, Postfix);

        pSelect = pRule->pFirstSymbol;  // 初始化 Select 游标
        pSelectPrePtr = &pRule->pFirstSymbol;
        while (pSelect != NULL)  // 循环处理所有的 Select
        {
            RuleSymbol* pTmp = CreateSymbol();
            pTmp->isToken = 0;
            pTmp->pRule = pNewRule;
            strcpy(pTmp->SymbolName, pNewRule->RuleName);
            if (0 == pSelect->isToken &&
                pSelect->pRule == pRule)  // Select 存在左递归
            {
                // 移除包含左递归的 Select，将其转换为右递归后添加到新 Rule
                // 的末尾，并移动游标
                *pSelectPrePtr = (*pSelectPrePtr)->pOther;
                pSelect = pSelect->pNextSymbol;
                AddSymbolToSelect(pSelect, pTmp);
                AddSelectToRule(pNewRule, pSelect);
                pSelect = *pSelectPrePtr;
            } else  // Select 不存在左递归
            {
                // 在没有左递归的 Select 末尾添加指向新 Rule
                // 的非终结符，并移动游标
                AddSymbolToSelect(pSelect, pTmp);
                pSelectPrePtr = &(*pSelectPrePtr)->pOther;
                pSelect = pSelect->pOther;
            }
        }

        // 在新 Rule 的最后加入ε(用 '$' 代替)
        // 将新 Rule 插入文法链表
        AddSelectToRule(pNewRule, NULL);
        pNewRule->pNextRule = pRule->pNextRule;
        pRule->pNextRule = pNewRule;
        pRule = pNewRule;
    }
}

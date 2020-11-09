#include "PickupLeftFactor.h"
#include <string.h>
#include <stdlib.h>

const char *VoidSymbol = "$"; // "ε"
const char *Postfix = "'";

char rule_table_ci[20][256];
char ruleNameArr[20][64];

int main(int argc, char *argv[])
{
	//
	// 调用 InitRules 函数初始化文法
	//
	Rule *pHead = InitRules();

	//
	// 输出提取左因子之前的文法
	//
	printf("Before Pickup Left Factor:\n");
	PrintRule(pHead);

	//
	// 调用 PickupLeftFactor 函数对文法提取左因子
	//
	PickupLeftFactor(pHead);

	//
	// 输出提取左因子之后的文法
	//
	printf("\nAfter Pickup Left Factor:\n");
	PrintRule(pHead);

	return 0;
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
RuleSymbol *GetSymbol(RuleSymbol *pSelect, int index)
{
	int i = 0;
	RuleSymbol *pRuleSymbol;
	for (pRuleSymbol = pSelect, i = 0; pRuleSymbol != NULL;
		 pRuleSymbol = pRuleSymbol->pNextSymbol, i++)
	{
		if (i == index)
		{
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
int LeftFactorMaxLength(RuleSymbol *pSelectTemplate)
{

	//
	// TODO: 在此添加代码
	//
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
int SymbolCmp(RuleSymbol *pSymbol1, RuleSymbol *pSymbol2)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	取文法中的一个 Select 与 SelectTemplate 进行比较，判断该 Select 是否需要提取左因子。

参数：
	pSelectTemplate -- 作为模板的 Select 指针。
	Count -- SelectTemplate 中已确定的左因子的数量。
	pSelect -- Select 指针。
	  
返回值：
	如果 Select 包含左因子返回 1，否则返回 0。
*/
int NeedPickup(RuleSymbol *pSelectTemplate, int Count, RuleSymbol *pSelect)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	将一个 Select 加入到文法末尾，当 Select 为 NULL 时就将一个ε终结符加入到文法末尾。

参数：
	pRule -- 文法指针。
	pNewSelect -- Select 指针。
*/
void AddSelectToRule(Rule *pRule, RuleSymbol *pNewSelect)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	将 pRuleName 与文法中的其他 RuleName 比较, 如果相同就增加一个后缀。

参数：
	pHead -- Rule 链表的头指针。
	pRuleName -- Rule 的名字。
*/
void GetUniqueRuleName(Rule *pHead, char *pRuleName)
{
	Rule *pRuleCursor = pHead;
	for (; pRuleCursor != NULL;)
	{
		if (0 == strcmp(pRuleCursor->RuleName, pRuleName))
		{
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
void FreeSelect(RuleSymbol *pSelect)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	提取左因子。

参数：
	pHead -- 文法的头指针。
*/
void PickupLeftFactor(Rule *pHead)
{
	Rule *pRule;				 // Rule 游标
	int isChange;				 // Rule 是否被提取左因子的标志
	RuleSymbol *pSelectTemplate; // Select 游标
	Rule *pNewRule;				 // Rule 指针
	RuleSymbol *pSelect;		 // Select 游标

	do
	{
		isChange = 0;

		for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
		{
			// 取 Rule 中的一个 Select 作为模板，调用 LeftFactorMaxLength 函数确定左因子的最大长度
			int Count = 0;
			for (pSelectTemplate = pRule->pFirstSymbol; pSelectTemplate != NULL; pSelectTemplate = pSelectTemplate->pOther)
			{
				if ((Count = LeftFactorMaxLength(pSelectTemplate)) > 0)
					break;
			}

			// 忽略没用左因子的 Rule
			if (Count == 0)
				continue;

			pNewRule = CreateRule(pRule->RuleName); // 创建新 Rule
			GetUniqueRuleName(pRule, pNewRule->RuleName);
			isChange = 1; // 设置标志

			// 调用 AddSelectToRule 函数把模板左因子之后的部分加到新 Rule 的末尾
			// 将模板左因子之后的部分替换为指向新 Rule 的非终结符

			//
			// TODO: 在此添加代码
			//

			// 从模板之后的位置循环查找包含左因子的 Select，并提取左因子
			pSelect = pSelectTemplate->pOther;
			RuleSymbol **pSelectPtr = &pSelectTemplate->pOther;
			while (pSelect != NULL)
			{
				if (NeedPickup(pSelectTemplate, Count, pSelect)) // Select 包含左因子
				{
					// 调用 AddSelectToRule 函数把左因子之后的部分加到新 Rule 的末尾
					// 将该 Select 从 Rule 中移除，释放内存，并移动游标

					//
					// TODO: 在此添加代码
					//
				}
				else // Select 不包含左因子
				{
					// 移动游标

					//
					// TODO: 在此添加代码
					//
				}
			}

			// 将新 Rule 加入到文法链表

			//
			// TODO: 在此添加代码
			//
		}

	} while (isChange == 1);
}

/*
功能：
	使用给定的数据初始化文法链表

返回值：
	文法的头指针
*/
typedef struct _SYMBOL
{
	int isToken;
	char Name[MAX_STR_LENGTH];
} SYMBOL;

typedef struct _RULE_ENTRY
{
	char RuleName[MAX_STR_LENGTH];
	SYMBOL Selects[64][64];
} RULE_ENTRY;

/* A -> abC | abcD | abcE */
static const RULE_ENTRY rule_table[] =
	{{"A", {{{1, "a"}, {1, "b"}, {1, "C"}}, {{1, "a"}, {1, "b"}, {1, "c"}, {1, "D"}}, {{1, "a"}, {1, "b"}, {1, "c"}, {1, "E"}}}}};

/*
功能：
	初始化文法链表。
	
返回值：
	文法的头指针。
*/
Rule *InitRules()
{
	Rule *pHead, *pRule;
	RuleSymbol **pSymbolPtr1, **pSymbolPtr2;
	int nRuleCount = sizeof(rule_table) / sizeof(rule_table[0]);
	int i, j, k;

	Rule **pRulePtr = &pHead;
	for (i = 0; i < nRuleCount; i++)
	{
		*pRulePtr = CreateRule(rule_table[i].RuleName);
		pRulePtr = &(*pRulePtr)->pNextRule;
	}

	pRule = pHead;
	for (i = 0; i < nRuleCount; i++)
	{
		pSymbolPtr1 = &pRule->pFirstSymbol;
		for (j = 0; rule_table[i].Selects[j][0].Name[0] != '\0'; j++)
		{
			pSymbolPtr2 = pSymbolPtr1;
			for (k = 0; rule_table[i].Selects[j][k].Name[0] != '\0'; k++)
			{
				const SYMBOL *pSymbol = &rule_table[i].Selects[j][k];

				*pSymbolPtr2 = CreateSymbol();
				(*pSymbolPtr2)->isToken = pSymbol->isToken;
				if (1 == pSymbol->isToken)
				{
					strcpy((*pSymbolPtr2)->TokenName, pSymbol->Name);
				}
				else
				{
					(*pSymbolPtr2)->pRule = FindRule(pHead, pSymbol->Name);
					if (NULL == (*pSymbolPtr2)->pRule)
					{
						printf("Init rules error, miss rule \"%s\"\n", pSymbol->Name);
						exit(1);
					}
				}
				pSymbolPtr2 = &(*pSymbolPtr2)->pNextSymbol;
			}

			pSymbolPtr1 = &(*pSymbolPtr1)->pOther;
		}

		pRule = pRule->pNextRule;
	}

	return pHead;
}

/*
功能：
	创建一个新的 Rule。

参数：
	pRuleName -- 文法的名字。
	
返回值：
	Rule 指针	
*/
Rule *CreateRule(const char *pRuleName)
{
	Rule *pRule = (Rule *)malloc(sizeof(Rule));

	strcpy(pRule->RuleName, pRuleName);
	pRule->pFirstSymbol = NULL;
	pRule->pNextRule = NULL;

	return pRule;
}

/*
功能：
	创建一个新的 Symbol。
	
返回值：
	RuleSymbol 指针	
*/
RuleSymbol *CreateSymbol()
{
	RuleSymbol *pSymbol = (RuleSymbol *)malloc(sizeof(RuleSymbol));

	pSymbol->pNextSymbol = NULL;
	pSymbol->pOther = NULL;
	pSymbol->isToken = -1;
	pSymbol->TokenName[0] = '\0';
	pSymbol->pRule = NULL;

	return pSymbol;
}

/*
功能：
	根据 RuleName 在文法链表中查找名字相同的文法。

参数：
	pHead -- 文法的头指针。
	RuleName -- 文法的名字。
	
返回值：
	Rule 指针	
*/
Rule *FindRule(Rule *pHead, const char *RuleName)
{
	Rule *pRule;
	for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		if (0 == strcmp(pRule->RuleName, RuleName))
		{
			break;
		}
	}

	return pRule;
}

/*
功能：
	输出文法。

参数：
	pHead -- 文法的头指针。
*/
void PrintRule(Rule *pHead)
{

	//
	// TODO: 在此添加代码
	//
}

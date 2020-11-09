#include "Follow.h"
#include <string.h>
#include <stdlib.h>

const char *VoidSymbol = "$"; // "ε"
const char *EndSymbol = "#";

char rule_table_ci[20][256];
char ruleNameArr[20][64];

int main(int argc, char *argv[])
{
	//
	// 调用 InitRules 函数初始化文法
	//
	Rule *pHead = InitRules();

	//
	// 初始化 First 集合、Follow 集合
	//
	SetList FirstSetList, FollowSetList;
	FirstSetList.nSetCount = 0;
	FollowSetList.nSetCount = 0;

	//
	// 调用 Follow 函数求文法的 First 集合、Follow 集合
	//
	Follow(pHead, &FollowSetList, &FirstSetList);

	//
	// 输出文法
	//
	PrintRule(pHead);

	//
	// 输出Follow集合
	//
	PrintFollowSet(&FollowSetList);

	return 0;
}

/*
功能：
	添加一个 Set 到 SetList。

参数：
	pSetList -- SetList 指针。
	pName -- 集合名称字符串指针。 
*/
void AddOneSet(SetList *pSetList, const char *pName)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	根据名称在 SetList 中查找。

参数：
	pSetList -- SetList 指针。
	pName -- 集合名称字符串指针。
	  
返回值：
	如果找到返回 Set 指针，否则返回 NULL。
*/
Set *GetSet(SetList *pSetList, const char *pName)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	添加一个终结符到 Set。

参数：
	pSet -- Set 指针。
	pTerminal -- 终结符名称指针。
	  
返回值：
	添加成功返回 1，否则返回 0。
*/
int AddTerminalToSet(Set *pSet, const char *pTerminal)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	将源 Set 添加到目标 Set 中，忽略ε。

参数：
	pDesSet -- 目标 Set 指针。
	pSrcSet -- 源 Set 指针。
	  
返回值：
	添加成功返回 1，否则返回 0。
*/
int AddSetToSet(Set *pDesSet, const Set *pSrcSet)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	判断 Set 中是否含有ε。

参数：
	pSet -- Set 指针。
	  
返回值：
	存在返回 1。
	不存在返回 0。
*/
int SetHasVoid(const Set *pSet)
{

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	求文法的 First 集合。

参数：
	pHead -- 文法的头指针。
	pFirstSetList -- First 集合指针。
*/
void First(const Rule *pHead, SetList *pFirstSetList)
{
	const Rule *pRule;	 // Rule 指针
	int isChange;		 // 集合是否被修改的标志
	RuleSymbol *pSymbol; // Symbol 游标

	//
	// TODO: 在此添加代码
	//
}

/*
功能：
	求文法的 Follow 集合。

参数：
	pHead -- 文法的头指针。
	pFollowSetList -- Follow 集合指针。
	pFirstSetList -- First 集合指针。
*/
void Follow(const Rule *pHead, SetList *pFollowSetList, SetList *pFirstSetList)
{
	const Rule *pRule;	 // Rule 指针
	int isChange;		 // 集合是否被修改的标志
	RuleSymbol *pSymbol; // Symbol 游标

	// 调用 First 函数求文法的 First 集合
	First(pHead, pFirstSetList);

	// 使用文法链表初始化 Follow 集合
	for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		AddOneSet(pFollowSetList, pRule->RuleName);
	}

	// 在开始符号的 Follow 集合中加入结束符 #
	AddTerminalToSet(&pFollowSetList->Sets[0], EndSymbol);

	do
	{
		isChange = 0; // 设置集合修改标志

		for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
		{
			for (pSymbol = pRule->pFirstSymbol; pSymbol != NULL; pSymbol = pSymbol->pNextSymbol)
			{
				// 忽略终结符
				if (pSymbol->isToken)
					continue;

				// 初始化一个 First 集合 TempFirstSet，作为临时变量
				Set TempFirstSet;
				TempFirstSet.nTerminalCount = 0;

				if (NULL != pSymbol->pNextSymbol)
				{
					if (pSymbol->pNextSymbol->isToken) // 终结符
					{
						// 调用 AddTerminalToSet 函数将该终结符加入到 TempFirstSet
						AddTerminalToSet(&TempFirstSet, pSymbol->pNextSymbol->SymbolName);
					}
					else // 非终结符
					{
						// 调用 GetSet 函数在 pFirstSetList 中查找 Set
						Set *pFirstSetXn = GetSet(pFirstSetList, pSymbol->pNextSymbol->SymbolName);

						// 将找到的 Set 加入 TempFirstSet 中
						AddSetToSet(&TempFirstSet, pFirstSetXn);
					}
				}
				else
				{
					// 调用 AddTerminalToSet 函数将 EndSymbol 加入到 TempFirstSet 中
					AddTerminalToSet(&TempFirstSet, EndSymbol);
				}

				// 调用 GetSet 函数在 pFollowSetList 中查找 Set
				Set *pFollowSetXi = GetSet(pFollowSetList, pSymbol->SymbolName);

				// 将集合 TempFirstSet 加入 pFollowSetXi
				if (AddSetToSet(pFollowSetXi, &TempFirstSet))
					isChange = 1; // 设置集合修改标志

				if (SetHasVoid(&TempFirstSet)) // 判断集合 TempFirstSet 是否含有ε
				{
					// 调用 GetSet 函数得到当前文法名称对应的 Follow 集合
					Set *pFollowSetA = GetSet(pFollowSetList, pRule->RuleName);

					// 将集合 pFollowSetA 加入 pFollowSetXi
					if (AddSetToSet(pFollowSetXi, pFollowSetA))
						isChange = 1; // 设置集合修改标志
				}
			}
		}

	} while (isChange);
}

typedef struct _SYMBOL
{
	int isToken;
	char SymbolName[MAX_STR_LENGTH];
} SYMBOL;

typedef struct _RULE_ENTRY
{
	char RuleName[MAX_STR_LENGTH];
	SYMBOL Symbols[64];
} RULE_ENTRY;

/* exp -> exp addop term| term 
   addop -> + | - 
   term -> term mulop factor | factor 
   mulop -> * 
   factor -> (exp) | number */
static const RULE_ENTRY rule_table[] =
	{{"exp", {{0, "exp"}, {0, "addop"}, {0, "term"}}},
	 {"exp", {{0, "term"}}},
	 {"addop", {{1, "+"}}},
	 {"addop", {{1, "-"}}},
	 {"term", {{0, "term"}, {0, "mulop"}, {0, "factor"}}},
	 {"term", {{0, "factor"}}},
	 {"mulop", {{1, "*"}}},
	 {"factor", {{1, "("}, {0, "exp"}, {1, ")"}}},
	 {"factor", {{1, "number"}}}};

/*
功能：
	初始化文法链表。
	
返回值：
	文法的头指针。
*/
Rule *InitRules()
{
	Rule *pHead, *pRule;
	RuleSymbol **pSymbolPtr, *pNewSymbol;
	int nRuleCount = sizeof(rule_table) / sizeof(rule_table[0]);
	int i, j;

	Rule **pRulePtr = &pHead;
	for (i = 0; i < nRuleCount; i++)
	{
		*pRulePtr = CreateRule(rule_table[i].RuleName);
		pRulePtr = &(*pRulePtr)->pNextRule;
	}

	pRule = pHead;
	for (i = 0; i < nRuleCount; i++)
	{
		pSymbolPtr = &pRule->pFirstSymbol;
		for (j = 0; rule_table[i].Symbols[j].SymbolName[0] != '\0'; j++)
		{
			const SYMBOL *pSymbol = &rule_table[i].Symbols[j];

			pNewSymbol = CreateSymbol();
			pNewSymbol->isToken = pSymbol->isToken;
			strcpy(pNewSymbol->SymbolName, pSymbol->SymbolName);
			*pSymbolPtr = pNewSymbol;

			pSymbolPtr = &pNewSymbol->pNextSymbol;
		}

		pRule = pRule->pNextRule;
	}

	return pHead;
}

/*
功能：
	创建一个新的文法。
	
参数：
	pRuleName -- 文法的名字。	
	
返回值：
	文法的指针。
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
	创建一个新的符号。
	
返回值：
	符号的指针。
*/
RuleSymbol *CreateSymbol()
{
	RuleSymbol *pSymbol = (RuleSymbol *)malloc(sizeof(RuleSymbol));

	pSymbol->pNextSymbol = NULL;
	pSymbol->isToken = -1;
	pSymbol->SymbolName[0] = '\0';

	return pSymbol;
}

/*
功能：
	输出文法。
	
参数：
	pHead -- 文法的头指针。
*/
void PrintRule(const Rule *pHead)
{
	const Rule *pRule;
	for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		printf("%s ->", pRule->RuleName);

		RuleSymbol *pRuleSymbol;
		for (pRuleSymbol = pRule->pFirstSymbol; pRuleSymbol != NULL;
			 pRuleSymbol = pRuleSymbol->pNextSymbol)
		{
			printf(" %s", pRuleSymbol->SymbolName);
		}
		printf("\n");
	}
}

/*
功能：
	输出 First 集合。

参数：
	pFirstSetList -- First 集合指针。
*/
void PrintFollowSet(SetList *pFollowSetList)
{
	printf("\nThe Follow Set:\n");
	for (int i = 0; i < pFollowSetList->nSetCount; i++)
	{
		printf("Follow(%s) = { ", pFollowSetList->Sets[i].Name);
		for (int j = 0; j < pFollowSetList->Sets[i].nTerminalCount; j++)
		{
			if (j == pFollowSetList->Sets[i].nTerminalCount - 1)
			{
				printf("%s ", pFollowSetList->Sets[i].Terminal[j]);
			}
			else
			{
				printf("%s, ", pFollowSetList->Sets[i].Terminal[j]);
			}
		}
		printf("}\n");
	}
}

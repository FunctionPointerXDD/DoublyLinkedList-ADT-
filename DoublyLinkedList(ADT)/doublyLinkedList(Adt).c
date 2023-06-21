#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int InsertAtHead(void* pParam, const char* (*pfParam)(void*));
int InsertAtTail(void* pParam, const char* (*pfParam)(void*));
int InsertAt(void* pParam, int idx, const char* (*pfParam)(void*));

//데이터형식(언제든 변경가능하게 구현)
typedef struct USERDATA {
	char szName[32];	//key로 설정
	char szPhone[32];
} USERDATA;

//데이터멤버 중 설정한 key값에 접근해서 (char*)형식으로 반환해주는 함수
const char* GetKeyFromUserData(USERDATA* pUser)
{
	return pUser->szName;
}


void CreateUserData(const char* pszName, const char* pszPhone)
{
	USERDATA* pNewData = (USERDATA*)malloc(sizeof(USERDATA));
	memset(pNewData, 0, sizeof(USERDATA));
	strncpy(pNewData->szName, pszName, sizeof(pNewData->szName));
	strncpy(pNewData->szPhone, pszPhone, sizeof(pNewData->szPhone));
	InsertAtTail(pNewData, GetKeyFromUserData);
}


typedef struct NODE {
	//관리대상 자료(모든 데이터형식에 접근한다. 반드시 USERDATA의 주소로만 접근할 필요가 없다.)
	void* pData;

	//멤버 함수 포인터 : 자료를 식별하기 위한 key값을 부여하는 함수포인터
	const char* (*pfGetKey)(void*); //매개변수를 void 포인터로 설정하면 모든 데이터 형식을 받을 수 있다. 위에서 선언한 USERDATA외에도 다른 형식도 적용이 가능하다.

	//자료구조
	struct NODE* pNext;
	struct NODE* pPrev;
} NODE;

NODE* g_pHead;
NODE* g_pTail;
int g_nSize;

void InitList()
{
	//더미노드 생성
	g_pHead = (NODE*)malloc(sizeof(NODE));
	g_pTail = (NODE*)malloc(sizeof(NODE));

	memset(g_pHead, 0, sizeof(NODE));
	memset(g_pTail, 0, sizeof(NODE));
	g_nSize = 0;

	g_pHead->pNext = g_pTail;
	g_pTail->pPrev = g_pHead;
}

void PrintList()
{
	int i = 0;
	NODE* pTmp = g_pHead;
	printf("<DUMMY_HEAD>: [%p] || <DUMMY_TAIL>: [%p] Size: %d\n", g_pHead, g_pTail, g_nSize);
	while (pTmp != 0)
	{
		if (pTmp == g_pHead || pTmp == g_pTail)
			printf("-----<DUMMY>-----\n");
		else
		{
			printf("Index: %d [%p]: %s\n",
				i, pTmp, pTmp->pfGetKey(pTmp->pData));
			++i;
		}
		pTmp = pTmp->pNext;
	}
	
}

void ReleaseList()
{
	NODE* pTmp = g_pHead;

	printf("ReleaseList()\n");
	while (pTmp != 0)
	{
		NODE* pDelete = pTmp;
		pTmp = pTmp->pNext;
		printf("Delete [%p]\n", pDelete);
		free(pDelete->pData);
		free(pDelete);
	}
	g_nSize = 0;
	g_pHead = 0;
	g_pTail = 0;
}

//호출자(메인함수)가 "동적할당"해서 여기에 주소값 넘겨주어서 연결하는 방식으로 설계
int InsertAtHead(void* pParam, const char* (*pfParam)(void*))
{
	NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
	memset(pNewNode, 0, sizeof(NODE));
	//data 붙이기
	pNewNode->pData = pParam;
	//멤버 함수포인터 초기화(여기서는 GetkeyFromUserData함수 주소로 초기화)
	pNewNode->pfGetKey = pfParam;

	//교통정리
	pNewNode->pNext = g_pHead->pNext;
	pNewNode->pPrev = g_pHead;

	g_pHead->pNext = pNewNode;
	pNewNode->pNext->pPrev = pNewNode;

	return g_nSize++;
}

int InsertAtTail(void* pParam, const char* (*pfParam)(void*))
{
	NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
	memset(pNewNode, 0, sizeof(NODE));

	pNewNode->pData = pParam;
	pNewNode->pfGetKey = pfParam;

	pNewNode->pNext = g_pTail;
	pNewNode->pPrev = g_pTail->pPrev;

	g_pTail->pPrev->pNext = pNewNode;
	g_pTail->pPrev = pNewNode;

	return g_nSize++;
}
//szName이라는 자료의존성 아직 존재함 (나중에 수정)
NODE* FindNode(const char* pszKey)
{
	NODE* pTmp = g_pHead->pNext;
	while (pTmp != g_pTail)
	{
		//구조체 멤버중 함수포인터에 접근해서 매개변수로 데이타형식(주소)를 받는다. 여기서 함수포인터가 GetKeyFromUserData함수를 호출하게 하면, 그 함수에서 특정데이터에 접근하여 반환하도록 만든다. 
		if (strcmp(pTmp->pfGetKey(pTmp->pData), pszKey) == 0)
			return pTmp;
		pTmp = pTmp->pNext;
	}
	return 0;
}

int DeleteNode(const char* pszKey)
{
	NODE* pNode = FindNode(pszKey);

	if (pNode != 0)
	{
		printf("DeleteNode: [%p] %s\n", pNode, pNode->pfGetKey(pNode->pData));
		//교통정리
		pNode->pPrev->pNext = pNode->pNext;
		pNode->pNext->pPrev = pNode->pPrev;
		free(pNode->pData);
		free(pNode);
	}

	return g_nSize--;
}
//index는 0부터 시작하는 걸로 가정
NODE* GetAt(int idx)
{
	unsigned int i = 0;
	NODE* pTmp = g_pHead->pNext;

	if (g_nSize > 0 && idx >= g_nSize)
	{
		printf("%d is not valid value!\n", idx);
		return 0;
	}
	else
		while (pTmp != g_pTail)
		{
			if (i == idx)
				return pTmp;
			pTmp = pTmp->pNext;
			++i;
		}
	//최초 노드 삽입시
	return g_pHead;
}

int InsertAt(void* pParam, int idx, const char* (*pfParam)(void*))
{
	if (GetAt(idx) == 0) return 0;
	NODE* pNode = GetAt(idx);

	//최초노드 삽입시(맨앞에 그냥 넣기)
	if (pNode == g_pHead)
		InsertAtHead(pParam, pfParam);
	else
	{
		//찾은 인덱스 노드 앞에다 넣는 방식으로
		NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
		memset(pNewNode, 0, sizeof(NODE));

		pNewNode->pData = pParam;
		pNewNode->pfGetKey = pfParam;

		pNewNode->pNext = pNode;
		pNewNode->pPrev = pNode->pPrev;

		pNode->pPrev->pNext = pNewNode;
		pNode->pPrev = pNewNode;
		g_nSize++;
	}

	return 1;
}

int main()
{
	InitList();

	CreateUserData("jeong-chan-soo", "010-2345-3463");
	CreateUserData("cziffra", "010-2225-7763");
	CreateUserData("hamelin pp", "010-8845-3400");
	
	PrintList();
	ReleaseList();
	return 0;
}
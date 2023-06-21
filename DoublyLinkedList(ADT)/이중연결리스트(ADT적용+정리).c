#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>

//함수 원형 선언
int InsertAtHead(void* pParam);
int InsertAtTail(void* pParam);
int InsertAt(void* pParam, int idx);

//데이터형식(언제든 변경가능하게 구현)
typedef struct USERDATA {
	const char* (*pfGetKey)(void*);//첫번째 멤버를 함수포인터로 선언
	char szName[32];	//key로 설정
	char szPhone[32];
} USERDATA;

//데이터멤버 중 설정한 key값에 접근해서 (char*)형식으로 반환해주는 함수
const char* GetKeyFromUserData(USERDATA* pUser)
{
	return pUser->szName;
}

USERDATA* CreateUserData(const char* pszName, const char* pszPhone)
{
	USERDATA* pNewData = (USERDATA*)malloc(sizeof(USERDATA));
	memset(pNewData, 0, sizeof(USERDATA));
	strncpy(pNewData->szName, pszName, sizeof(pNewData->szName));
	strncpy(pNewData->szPhone, pszPhone, sizeof(pNewData->szPhone));
	
	//구조체 멤버 함수포인터 초기화(여기서는 GetKeyFromUserData함수 주소로 초기화)
	pNewData->pfGetKey = GetKeyFromUserData;

	return pNewData;
}


typedef struct NODE {
	//관리대상 자료(모든 데이터형식에 접근한다. 반드시 USERDATA의 주소로만 접근할 필요는 없다.)
	void* pData;

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
	//초기화 할 것들
	int i = 0;
	NODE* pTmp = g_pHead;
	
	printf("<DUMMY_HEAD>: [%p] || <DUMMY_TAIL>: [%p] Size: %d\n", g_pHead, g_pTail, g_nSize);
	while (pTmp != 0)
	{
		if (pTmp == g_pHead || pTmp == g_pTail)
			printf("-----<DUMMY>-----\n");
		else
		{
			//NODE구조체 멤버중 pData는 보이드형 포인터이므로 데이터형식을 식별할 수 없다.
			//따라서 출력할 데이터형식을 선언해주어야 한다.(출력함수는 데이터에 의존적일 수 밖에 없다.)
			USERDATA* pUser = pTmp->pData;
			//key값(szName)이 잘 반환되는지 출력해본다. 물론 바로 pUser->szName으로 접근도 가능하다.
			printf("Index: %d [%p]: %s, %s\n",
				i, pTmp, pUser->pfGetKey(pUser), pUser->szPhone);
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
int InsertAtHead(void* pParam)
{
	NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
	memset(pNewNode, 0, sizeof(NODE));
	//관리대상 data 붙이기(여기서는 USERDATA의 주소가 넘겨진다!!)
	pNewNode->pData = pParam;

	//자료구조 교통정리
	pNewNode->pNext = g_pHead->pNext;
	pNewNode->pPrev = g_pHead;

	g_pHead->pNext = pNewNode;
	pNewNode->pNext->pPrev = pNewNode;

	return g_nSize++;
}

int InsertAtTail(void* pParam)
{
	NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
	memset(pNewNode, 0, sizeof(NODE));

	pNewNode->pData = pParam;

	pNewNode->pNext = g_pTail;
	pNewNode->pPrev = g_pTail->pPrev;

	g_pTail->pPrev->pNext = pNewNode;
	g_pTail->pPrev = pNewNode;

	return g_nSize++;
}

NODE* FindNode(const char* pszKey)
{
	NODE* pTmp = g_pHead->pNext;
	const char* (*pfGetKey)(void*) = 0;
	while (pTmp != g_pTail)
	{ 
		//임시로 선언한 함수포인터에 pData주소를 저장한다. 그런데 pData주소는 데이터생성시 USERDATA주소를 가리키고 있고,
		//첫번째 멤버가 GetKeyFromUserData함수를 호출하는 함수포인터이다.
		//구조체 주소와 그 구조체 첫번째 멤버의 주소는 항상 같다는 점을 응용한 것이다.
		pfGetKey = pTmp->pData;
		if (strcmp(pfGetKey(pTmp->pData), pszKey) == 0)
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
		printf("DeleteNode: [%p]\n", pNode);
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

int InsertAt(void* pParam, int idx)
{
	if (GetAt(idx) == 0) return 0;
	NODE* pNode = GetAt(idx);

	//최초노드 삽입시(맨앞에 그냥 넣기)
	if (pNode == g_pHead)
		InsertAtHead(pParam);
	else
	{
		//찾은 인덱스 노드 앞에다 넣는 방식으로
		NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
		memset(pNewNode, 0, sizeof(NODE));

		pNewNode->pData = pParam;

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
	USERDATA* pNewData = 0;

	pNewData = CreateUserData("jeong-chan-soo", "010-2345-3463");
	InsertAt(pNewData, 0);
	pNewData = CreateUserData("hamelin", "010-3456-7777");
	InsertAtTail(pNewData);
	pNewData = CreateUserData("정형돈", "010-2342-3463");
	InsertAtTail(pNewData);
	pNewData = CreateUserData("박명수", "010-1116-7727");
	InsertAtHead(pNewData);

	PrintList();
	ReleaseList();
	return 0;
}
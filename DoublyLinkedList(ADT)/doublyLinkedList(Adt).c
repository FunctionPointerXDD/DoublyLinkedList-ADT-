#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int InsertAtHead(void* pParam, const char* (*pfParam)(void*));
int InsertAtTail(void* pParam, const char* (*pfParam)(void*));
int InsertAt(void* pParam, int idx, const char* (*pfParam)(void*));

//����������(������ ���氡���ϰ� ����)
typedef struct USERDATA {
	char szName[32];	//key�� ����
	char szPhone[32];
} USERDATA;

//�����͸�� �� ������ key���� �����ؼ� (char*)�������� ��ȯ���ִ� �Լ�
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
	//������� �ڷ�(��� ���������Ŀ� �����Ѵ�. �ݵ�� USERDATA�� �ּҷθ� ������ �ʿ䰡 ����.)
	void* pData;

	//��� �Լ� ������ : �ڷḦ �ĺ��ϱ� ���� key���� �ο��ϴ� �Լ�������
	const char* (*pfGetKey)(void*); //�Ű������� void �����ͷ� �����ϸ� ��� ������ ������ ���� �� �ִ�. ������ ������ USERDATA�ܿ��� �ٸ� ���ĵ� ������ �����ϴ�.

	//�ڷᱸ��
	struct NODE* pNext;
	struct NODE* pPrev;
} NODE;

NODE* g_pHead;
NODE* g_pTail;
int g_nSize;

void InitList()
{
	//���̳�� ����
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

//ȣ����(�����Լ�)�� "�����Ҵ�"�ؼ� ���⿡ �ּҰ� �Ѱ��־ �����ϴ� ������� ����
int InsertAtHead(void* pParam, const char* (*pfParam)(void*))
{
	NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
	memset(pNewNode, 0, sizeof(NODE));
	//data ���̱�
	pNewNode->pData = pParam;
	//��� �Լ������� �ʱ�ȭ(���⼭�� GetkeyFromUserData�Լ� �ּҷ� �ʱ�ȭ)
	pNewNode->pfGetKey = pfParam;

	//��������
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
//szName�̶�� �ڷ������� ���� ������ (���߿� ����)
NODE* FindNode(const char* pszKey)
{
	NODE* pTmp = g_pHead->pNext;
	while (pTmp != g_pTail)
	{
		//����ü ����� �Լ������Ϳ� �����ؼ� �Ű������� ����Ÿ����(�ּ�)�� �޴´�. ���⼭ �Լ������Ͱ� GetKeyFromUserData�Լ��� ȣ���ϰ� �ϸ�, �� �Լ����� Ư�������Ϳ� �����Ͽ� ��ȯ�ϵ��� �����. 
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
		//��������
		pNode->pPrev->pNext = pNode->pNext;
		pNode->pNext->pPrev = pNode->pPrev;
		free(pNode->pData);
		free(pNode);
	}

	return g_nSize--;
}
//index�� 0���� �����ϴ� �ɷ� ����
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
	//���� ��� ���Խ�
	return g_pHead;
}

int InsertAt(void* pParam, int idx, const char* (*pfParam)(void*))
{
	if (GetAt(idx) == 0) return 0;
	NODE* pNode = GetAt(idx);

	//���ʳ�� ���Խ�(�Ǿտ� �׳� �ֱ�)
	if (pNode == g_pHead)
		InsertAtHead(pParam, pfParam);
	else
	{
		//ã�� �ε��� ��� �տ��� �ִ� �������
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
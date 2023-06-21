#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>

//�Լ� ���� ����
int InsertAtHead(void* pParam);
int InsertAtTail(void* pParam);
int InsertAt(void* pParam, int idx);

//����������(������ ���氡���ϰ� ����)
typedef struct USERDATA {
	const char* (*pfGetKey)(void*);//ù��° ����� �Լ������ͷ� ����
	char szName[32];	//key�� ����
	char szPhone[32];
} USERDATA;

//�����͸�� �� ������ key���� �����ؼ� (char*)�������� ��ȯ���ִ� �Լ�
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
	
	//����ü ��� �Լ������� �ʱ�ȭ(���⼭�� GetKeyFromUserData�Լ� �ּҷ� �ʱ�ȭ)
	pNewData->pfGetKey = GetKeyFromUserData;

	return pNewData;
}


typedef struct NODE {
	//������� �ڷ�(��� ���������Ŀ� �����Ѵ�. �ݵ�� USERDATA�� �ּҷθ� ������ �ʿ�� ����.)
	void* pData;

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
	//�ʱ�ȭ �� �͵�
	int i = 0;
	NODE* pTmp = g_pHead;
	
	printf("<DUMMY_HEAD>: [%p] || <DUMMY_TAIL>: [%p] Size: %d\n", g_pHead, g_pTail, g_nSize);
	while (pTmp != 0)
	{
		if (pTmp == g_pHead || pTmp == g_pTail)
			printf("-----<DUMMY>-----\n");
		else
		{
			//NODE����ü ����� pData�� ���̵��� �������̹Ƿ� ������������ �ĺ��� �� ����.
			//���� ����� ������������ �������־�� �Ѵ�.(����Լ��� �����Ϳ� �������� �� �ۿ� ����.)
			USERDATA* pUser = pTmp->pData;
			//key��(szName)�� �� ��ȯ�Ǵ��� ����غ���. ���� �ٷ� pUser->szName���� ���ٵ� �����ϴ�.
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

//ȣ����(�����Լ�)�� "�����Ҵ�"�ؼ� ���⿡ �ּҰ� �Ѱ��־ �����ϴ� ������� ����
int InsertAtHead(void* pParam)
{
	NODE* pNewNode = (NODE*)malloc(sizeof(NODE));
	memset(pNewNode, 0, sizeof(NODE));
	//������� data ���̱�(���⼭�� USERDATA�� �ּҰ� �Ѱ�����!!)
	pNewNode->pData = pParam;

	//�ڷᱸ�� ��������
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
		//�ӽ÷� ������ �Լ������Ϳ� pData�ּҸ� �����Ѵ�. �׷��� pData�ּҴ� �����ͻ����� USERDATA�ּҸ� ����Ű�� �ְ�,
		//ù��° ����� GetKeyFromUserData�Լ��� ȣ���ϴ� �Լ��������̴�.
		//����ü �ּҿ� �� ����ü ù��° ����� �ּҴ� �׻� ���ٴ� ���� ������ ���̴�.
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

int InsertAt(void* pParam, int idx)
{
	if (GetAt(idx) == 0) return 0;
	NODE* pNode = GetAt(idx);

	//���ʳ�� ���Խ�(�Ǿտ� �׳� �ֱ�)
	if (pNode == g_pHead)
		InsertAtHead(pParam);
	else
	{
		//ã�� �ε��� ��� �տ��� �ִ� �������
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
	pNewData = CreateUserData("������", "010-2342-3463");
	InsertAtTail(pNewData);
	pNewData = CreateUserData("�ڸ��", "010-1116-7727");
	InsertAtHead(pNewData);

	PrintList();
	ReleaseList();
	return 0;
}
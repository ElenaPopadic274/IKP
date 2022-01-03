#define _CRT_SECURE_NO_WARNINGS

#include "ReplicatorList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define GUID_FORMAT "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

CRITICAL_SECTION csReplicator;
bool Contains(NODE_REPLICATOR** head, PROCESS process);

void InitReplicatorList(NODE_REPLICATOR** head)
{
	//InitializeCriticalSection(&csReplicator);
	InitializeCriticalSectionAndSpinCount(&csReplicator, 0x80000400);
	EnterCriticalSection(&csReplicator);
	*head = NULL;
	LeaveCriticalSection(&csReplicator);
}

bool PushBack(NODE_REPLICATOR** head, PROCESS process)
{
	if (Contains(head, process))
		return false;

	NODE_REPLICATOR* tempNode = *head;
	NODE_REPLICATOR* newNode = (NODE_REPLICATOR*)malloc(sizeof(NODE_REPLICATOR));
	newNode->process = process;
	newNode->next = NULL;

	if (tempNode == NULL) // dodajemo prvi element
	{
		EnterCriticalSection(&csReplicator);
		*head = newNode;
		LeaveCriticalSection(&csReplicator);
		return true;
	}
	while (tempNode->next != NULL)	// dodajemo na kraj
	{
		tempNode = tempNode->next;
	}
	EnterCriticalSection(&csReplicator);
	tempNode->next = newNode;
	LeaveCriticalSection(&csReplicator);

	return true;
}

void PrintAllProcesses(NODE_REPLICATOR** head)
{
	NODE_REPLICATOR* tempNode = *head;

	printf("\nAll processes:\n");
	while (tempNode != NULL)
	{
		printf("ID: {" GUID_FORMAT "}\n", GUID_ARG(tempNode->process.processId));

		EnterCriticalSection(&csReplicator);
		tempNode = tempNode->next;
		LeaveCriticalSection(&csReplicator);
	}
	printf("\n");
}

bool Contains(NODE_REPLICATOR** head, PROCESS process)
{
	NODE_REPLICATOR* tempNode = *head;

	while (tempNode != NULL)
	{
		if (tempNode->process.processId == process.processId)
			return true;

		EnterCriticalSection(&csReplicator);
		tempNode = tempNode->next;
		LeaveCriticalSection(&csReplicator);
	}
	return false;
}

bool AddSocketToID(NODE_REPLICATOR** head, PROCESS** process)
{
	NODE_REPLICATOR* tempNode = *head;
	PROCESS* tempProcess = *process;

	while (tempNode != NULL)
	{
		if (tempNode->process.acceptedSocket == NULL)
		{
			EnterCriticalSection(&csReplicator);
			tempNode->process.acceptedSocket = tempProcess->acceptedSocket;
			tempProcess->processId = tempNode->process.processId;
			LeaveCriticalSection(&csReplicator);
			return true;
		}
		EnterCriticalSection(&csReplicator);
		tempNode = tempNode->next;
		LeaveCriticalSection(&csReplicator);
	}
	return false;
}

bool IsSocketNull(NODE_REPLICATOR** head)
{
	NODE_REPLICATOR* tempNode = *head;

	while (tempNode != NULL)
	{
		if (tempNode->process.acceptedSocket == NULL)
			return true;

		EnterCriticalSection(&csReplicator);
		tempNode = tempNode->next;
		LeaveCriticalSection(&csReplicator);
	}
	return false;
}

bool FindProcess(NODE_REPLICATOR** head, PROCESS** process, GUID guid)
{
	NODE_REPLICATOR* tempNode = *head;
	PROCESS* tempProcess = *process;

	while (tempNode != NULL)
	{
		if (tempNode->process.processId == guid)
		{
			EnterCriticalSection(&csReplicator);
			tempProcess->acceptedSocket = tempNode->process.acceptedSocket;
			tempProcess->processId = tempNode->process.processId;
			LeaveCriticalSection(&csReplicator);
			return true;
		}
		EnterCriticalSection(&csReplicator);
		tempNode = tempNode->next;
		LeaveCriticalSection(&csReplicator);
	}
	return false;
}

PROCESS InitProcess(GUID processId, SOCKET acceptedSocket)
{
	PROCESS p;
	p.processId = processId;
	p.acceptedSocket = acceptedSocket;

	return p;
}
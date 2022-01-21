#define _CRT_SECURE_NO_WARNINGS

#include "ProcessList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

CRITICAL_SECTION csProcess;

/*
	FUNKCIJA: InitProcessList
	FUNKCIONALNOST: Inicijalizacija liste
	POVRATNA VREDNOST: nema, void je
*/
void InitProcessList(NODE_PROCESS**head)
{
	InitializeCriticalSectionAndSpinCount(&csProcess, 0x80000400);
	//InitializeCriticalSection(&csProcess);
	EnterCriticalSection(&csProcess);
	*head = NULL;
	LeaveCriticalSection(&csProcess);
}

/*
	FUNKCIJA: PushProcess
	FUNKCIONALNOST: Dodavanje procesa u listu
	POVRATNA VREDNOST: nema, void je
*/
void PushProcess(NODE_PROCESS** head, DATA data)
{
	NODE_PROCESS* tempNode = *head;
	NODE_PROCESS* newNode = (NODE_PROCESS*)malloc(sizeof(NODE_PROCESS));
	newNode->data = data;
	newNode->next = NULL;

	if (tempNode == NULL) // dodajemo prvi element
	{
		EnterCriticalSection(&csProcess);
		*head = newNode;
		LeaveCriticalSection(&csProcess);
		return;
	}
	while (tempNode->next != NULL)	// dodajemo na kraj
	{
		tempNode = tempNode->next;
	}
	EnterCriticalSection(&csProcess);
	tempNode->next = newNode;
	LeaveCriticalSection(&csProcess);
}

/*
	FUNKCIJA: PopFront
	FUNKCIONALNOST: Skidanje sa liste
	POVRATNA VREDNOST: Data
*/
DATA PopFront(NODE_PROCESS** head)
{
	NODE_PROCESS* tempNode = *head;
	DATA returnData = { {0} };

	if (tempNode == NULL) {
		printf("\nList is empty!\n");
		return returnData;
	}
	returnData = InitData(tempNode->data.data);

	EnterCriticalSection(&csProcess);
	*head = tempNode->next;
	free(tempNode);
	LeaveCriticalSection(&csProcess);

	return returnData;
}

/*
	FUNKCIJA: PrintAllData
	FUNKCIONALNOST: Ispisuje sve podatke
	POVRATNA VREDNOST: nema, void je
*/
void PrintAllData(NODE_PROCESS** head)
{
	NODE_PROCESS* tempNode = *head;

	printf("\nAll data:\n");
	while (tempNode != NULL)
	{
		printf("%s\n", tempNode->data);

		tempNode = tempNode->next;
	}
	printf("\n");
}

/*
	FUNKCIJA: InitData
	FUNKCIONALNOST: Inicijalizuje podatke
	POVRATNA VREDNOST: Data
*/
DATA InitData(char* data)
{
	DATA d;
	strcpy(d.data, data);

	return d;
}
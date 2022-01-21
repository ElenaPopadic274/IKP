#define WIN32_LEAN_AND_MEAN

#include <combaseapi.h>
#include <winsock.h>

//lista procesa
typedef struct process_st
{
	GUID processId;
	SOCKET acceptedSocket;
}PROCESS;

typedef struct node_st_replicator
{
	PROCESS process;
	struct node_st_replicator* next;
}NODE_REPLICATOR;

void InitReplicatorList(NODE_REPLICATOR** head);

bool PushBack(NODE_REPLICATOR** head, PROCESS process);

void PrintAllProcesses(NODE_REPLICATOR** head);

bool Contains(NODE_REPLICATOR** head, PROCESS process);

bool AddSocketToID(NODE_REPLICATOR** head, PROCESS** process);

bool IsSocketNull(NODE_REPLICATOR** head);

bool FindProcess(NODE_REPLICATOR** head, PROCESS** process, GUID guid);

PROCESS InitProcess(GUID processId, SOCKET acceptedSocket);
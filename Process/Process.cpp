#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <combaseapi.h>
#include "..\Common\ProcessList.h"
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT_R1 27016
#define DEFAULT_PORT_R2 27017

bool InitializeWindowsSockets();
void RegisterProcess(SOCKET connectSocket, int i);
void SendData(SOCKET connectSocket, char* i);

char* guidToString(const GUID* id, char* out);
GUID stringToGUID(const std::string& guid);

DWORD WINAPI handleIncomingData(LPVOID lpParam);

NODE_PROCESS* headProcess;

int main(int argc, char* argv[])
{
	InitProcessList(&headProcess);
	int serverPort = DEFAULT_PORT_R1;

	/*
		Proces bira sa kojim replikatorom zeli da se poveze
	*/
	if (argc > 1)
	{
		char* arg = (char*)argv[1];
		if (strcmp("27017", arg) == 0)
		{
			printf("Connected to the Replicator2!\n");
			serverPort = DEFAULT_PORT_R2;
		}
		else
		{
			printf("Connected to the Replicator1!\n");
			serverPort = DEFAULT_PORT_R1;
		}
	}
	else
	{
		char line[256];
		int i = 0;
		bool selected = false;
		while (!selected) {
			printf("Choose replicator(1 or 2): ");
			if (fgets(line, sizeof(line), stdin))
			{
				if (1 == sscanf(line, "%d", &i))
				{
					/* Now 'i' can be safely used */
					if (i == 1)
					{
						printf("Connected to the Replicator1!\n");
						serverPort = DEFAULT_PORT_R1;
						selected = true;
					}
					else if (i == 2)
					{
						printf("Connected to the Replicator2!\n");
						serverPort = DEFAULT_PORT_R2;
						selected = true;
					}
					else
					{
						printf("Invalid input.\n");
					}
				}
				else
				{
					printf("Invalid input.\n");
				}
			}
		}
	}

	char messageBuffer[DEFAULT_BUFLEN];
	char message[DEFAULT_BUFLEN];

	NODE_PROCESS* head;
	InitProcessList(&head);
	bool dataSent = false;

#pragma region connectRegion
	// socket used to communicate with server
	SOCKET connectSocket = INVALID_SOCKET;
	// variable used to store function return value
	int iResult;
	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	// create a socket
	connectSocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddress.sin_port = htons(serverPort);
	// connect to server specified in serverAddress and socket connectSocket
	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
	}

#pragma endregion 

	unsigned long mode = 1; //non-blocking mode
	iResult = ioctlsocket(connectSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);

	fd_set readfds;
	FD_ZERO(&readfds);

	DWORD funId;
	HANDLE handle;

	handle = CreateThread(NULL, 0, &handleIncomingData, &connectSocket, 0, &funId);

	while (true)
	{
		if (!dataSent) {
			puts("__________________________________________________________________________________");
			puts("MAIN MENU: ");
			puts("0. Exit.");
			puts("1. Register process.");
			puts("2. Send data.");
		}

		char line[256];
		int i;
		if (fgets(line, sizeof(line), stdin))
		{
			if (1 == sscanf(line, "%d", &i))
			{
				/* Now 'i' can be safely used */
				if (i == 0)
				{
					puts("Client is shuting down...");
					//CloseHandle(handle);
					break;
				}
				else if (i == 1)
				{
					RegisterProcess(connectSocket, i);
				}
				else if (i == 2)
				{
					message[0] = '2';
					printf("Please input text: ");
					scanf("%s", &message[1]);
					SendData(connectSocket, message);
					dataSent = true;
				}
				else
				{
					printf("Invalid input.\n");
				}
			}
			else
			{
				if (dataSent)
				{
					dataSent = false;
				}
				else
				{
					printf("Invalid input.\n");
				}
			}
		}
	}

	// cleanup
	closesocket(connectSocket);
	WSACleanup();

	return 0;
}

/*
	FUNKCIJA: InitializeWindowsSockets
	FUNKCIONALNOST: Inicijalizuje WsaData podatke
	POVRATNA VREDNOST: Uspesnost akcije
*/
bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

/*
	FUNKCIJA: RegisterProcess
	FUNKCIONALNOST: Registruje proces
	POVRATNA VREDNOST: nema, void je
*/
void RegisterProcess(SOCKET connectSocket, int i)
{
	char messageBuffer[DEFAULT_BUFLEN];

	// Send an prepared message with null terminator included
	int iResult = send(connectSocket, (char*)&i, sizeof(i), 0);

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return;
	}

}

/*
	FUNKCIJA: SendData
	FUNKCIONALNOST: Salje podatke na izabrani proces
	POVRATNA VREDNOST: nema, void je
*/
void SendData(SOCKET connectSocket, char* i)
{
	char messageBuffer[DEFAULT_BUFLEN];

	// Send an prepared message with null terminator included
	int iResult = send(connectSocket, i, (int)strlen(i), 0);

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return;
	}
}

/*
	FUNKCIJA: guidToString
	FUNKCIONALNOST: Pretvara globalni identifikator u string
	POVRATNA VREDNOST: string
*/
char* guidToString(const GUID* id, char* out) {
	int i;
	char* ret = out;
	out += sprintf(out, "%.8lX-%.4hX-%.4hX-", id->Data1, id->Data2, id->Data3);
	for (i = 0; i < sizeof(id->Data4); ++i) {
		out += sprintf(out, "%.2hhX", id->Data4[i]);
		if (i == 1) *(out++) = '-';
	}
	return ret;
}

/*
	FUNKCIJA: stringToGUID
	FUNKCIONALNOST: Pretvara string u globalni identifikator
	POVRATNA VREDNOST: GUID
*/
GUID stringToGUID(const std::string& guid) {
	GUID output;
	const auto ret = sscanf(guid.c_str(), "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", &output.Data1, &output.Data2, &output.Data3, &output.Data4[0], &output.Data4[1], &output.Data4[2], &output.Data4[3], &output.Data4[4], &output.Data4[5], &output.Data4[6], &output.Data4[7]);
	if (ret != 11)
		throw std::logic_error("Unvalid GUID, format should be {00000000-0000-0000-0000-000000000000}");
	return output;
}

/*
	FUNKCIJA: handleIncomingData
	FUNKCIONALNOST: Upravlja sa sadrzajem koji pristize, u zavisnosti od odabira funkcije moze da registruje proces i da cuva podatke
	POVRATNA VREDNOST: Uspesnost akcije
*/
DWORD WINAPI handleIncomingData(LPVOID lpParam)
{
	SOCKET* connectSocket = (SOCKET*)lpParam;

	int iResult;
	char messageBuffer[DEFAULT_BUFLEN];

	while (true)
	{
		fd_set readfds;
		FD_ZERO(&readfds);

		FD_SET(*connectSocket, &readfds);
		timeval timeVal;
		timeVal.tv_sec = 2;
		timeVal.tv_usec = 0;
		int result = select(0, &readfds, NULL, NULL, &timeVal);

		if (result == 0)
		{
			// vreme za cekanje je isteklo
		}
		else if (result == SOCKET_ERROR)
		{
			//desila se greska prilikom poziva funkcije
		}
		else if (FD_ISSET(*connectSocket, &readfds))
		{
			// rezultat je jednak broju soketa koji su zadovoljili uslov
			iResult = recv(*connectSocket, messageBuffer, DEFAULT_BUFLEN, 0);
			if (iResult > 0)
			{
				if (messageBuffer[0] == '0')
					printf("This process is registered already.\n");
				else if (messageBuffer[0] == '1')
					printf("Registered successfully.\n");
				else if (messageBuffer[0] == '2')
					printf("Data wasn't saved successfully.\n");
				else if (messageBuffer[0] == '3')
					printf("Data saved successfully.\n");
				else if (messageBuffer[0] == '4')
				{
					DATA data = InitData(&messageBuffer[1]);
					PushProcess(&headProcess, data);

					PrintAllData(&headProcess);
				}
				else if (messageBuffer[0] == '5') {
					printf("Your copy has stopped working. Please stop sending messages and close connection.\n");
					//*connectSocket = NULL;
					break;
				}
			}
			else if (iResult == 0)
			{
				// connection was closed gracefully
				printf("Connection with Replicator closed.\n");
				
				closesocket(*connectSocket);
			}
			else
			{
				// there was an error during recv
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(*connectSocket);
			}
		}
		FD_CLR(*connectSocket, &readfds);
	}
	
		return 0;
}
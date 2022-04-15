# IKP - INDUSTRIJSKI KOMUNIKACIONI PROTOKOLI | INDUSTRIAL COMMUNICATION PROTOCOLS
Potrebno je implementirati servis za replikaciju podataka. Servis se sasatoji iz dva replikatora, i po N parova procesa, na svakom replikatoru po jedan proces 
iz svakog para sa istim imenom. 

Servis implementira interface: 
* RegisterProcess(int ProcessID) – koristi se za registraciju procesa na replikator i eventualno kreiranje procesa na drugom replikatoru 
* SendData(int ProcessID, void* data, int dataSize) – proces šalje podatke replikatoru na čuvanje, tj. slanje odgovarajućem procesu na drugi replikator 

Svi procesi na oba servisa mogu da zahtevaju registraciju i čuvanje podataka. Svako slanje poruke zahteva potvrdu da je poruka primljena. 

![image](https://user-images.githubusercontent.com/80057962/163589374-2cd1e29f-3c5f-4cca-bcf6-35f8612e6b94.png)

----------------------------------------------------------------------------------------------------------------------------------------------------------------------
A data replication service needs to be implemented. The service consists of two replicators, and N pairs of processes, on each replicator one process from each 
pair with the same name. 

The service implements an interface: 
* RegisterProcess (int ProcessID) - used to register a process on a replicator and possibly create a process on another replicator 
* SendData (int ProcessID, void * data, int dataSize) - the process sends data to the replicator for storage, ie. sending to the appropriate process on another replicator 

All processes on both services may require registration and data storage. Each message you send requires confirmation that the message has been received.

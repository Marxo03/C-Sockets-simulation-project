/*
 ----------------------------------------------------------------------------
 Name        : client-TCP_simulation.c
 Author      : Nemo
 Description : TCP Simulation in C - Client
 ----------------------------------------------------------------------------
*/

//Inclusione delle librerie a seconda del sistema operativo
#if defined WIN32
#include <winsock.h>	//include la libreria winsock per i compilatori con sistemi Windows
#else
#define closesocket close
#include <sys/socket.h>		//include le librerie per i compilatori con sistemi Unix/Mac
#include <arpa/inet.h>
#include <unistd.h>
#endif
//Inclusione di altre librerie standard
#include <stdio.h>			//Libreria standard di input/output
#include <stdlib.h>			//Libreria standard per funzioni di utilità
#include <string.h>			//Libreria per la gestione delle stringhe
#include <ctype.h>			//Libreria per la gestione dei caratteri

#define BUFFERSIZE 512		//Dimensione Buffer del Client
#define PROTOPORT 27015		//Numero di porta Default del Client


// Funzione che stampa un messaggio di errore (input: stringa )
void ErrorHandler(char *errorMessage)
{
	printf("%s",errorMessage);
}

//funzione che rimuove i dati creati dalla winsock
void ClearWinSock()
{
	#if defined WIN32
	WSACleanup();
	#endif
}

//MAIN
int main()//main start
{
	//DICHIARAZIONE VARIABILI
	int Csocket;	//dichiaro la Socket del Client
	struct sockaddr_in sad; //dichiaro un oggetto di tipo sockaddr_in (IP + port)
	char buf[BUFFERSIZE]; 	// buffer for data from the server
	int bytesRcvd;			//numero di byte ricevuti
	int num;				//intero che verrà aquisito da tastiera
	char num1[15];		//first integer
	char num2[15];		//second integer

	puts(" ______________");
	puts("| Client Start |");		//messaggio di inizio del Client
	puts(" --------------\n");


	#if defined WIN32
		WSADATA wsaData;	//Struttura per Winsock

		//Inizializzazione della libreria di socket e specifica della versione
		int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);

		//Verifica se l'inizializzazione e' avvenuta con successo
		if (iResult != 0)
		{
			// In caso di errore, stampa un messaggio e termina il programma
			printf ("error at WSASturtup\n");
			return -1;
		}

			puts("WSA CHECK"); //Winsock inizializzato correttamente
	#endif


	// CREAZIONE DELLA SOCKET

	//creazione di una socket IPv4 (Protocol Family) di tipo stream
	//che utilizza il protocollo TCP
	Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//se qualcosa e' andato storto restituisce -1
	if (Csocket < 0)
	{
		//se fallisce restituisce il messaggio di errore, chiude la socket,
		//pulisce Winsock e termina il programma
		ErrorHandler("socket creation failed.\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}
		puts("Socket CHECK");	//Socket creata correttamente

	//COSTRUZIONE DELL’INDIRIZZO DEL SERVER

	memset(&sad, 0, sizeof(sad));	//Inizializzazione dei campi della struttura sad
	sad.sin_family = AF_INET;						//protocol family (IPv4)
	sad.sin_addr.s_addr = inet_addr("127.0.0.1"); 	// IP del server
	sad.sin_port = htons(5193); 					// Server port


	//CONNESSIONE AL SERVER

	//Invia una richiesta di connessione al server
	if (connect(Csocket, (struct sockaddr *)&sad, sizeof(sad)) < 0)
	{
		//se qualcosa e' andato storto stampa il messaggio di errore, chiude la socket
		//pulisce winsock e termina il programma
		ErrorHandler("Failed to connect.\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

		puts("Connection to Server CHECK");



	//RICEVERE DATI DAL SERVER

	//Alloco per Cstring 2 byte in memoria
	char* CString = (char*)malloc(2 * sizeof(char));


	//CONFERMA DI CONNESSIONE AVVENUTA
	printf("Received: "); // Setup to print the echoed string


	if ((bytesRcvd = recv(Csocket, buf, BUFFERSIZE - 1, 0)) <= 0)
	{
		ErrorHandler("recv() failed or connection closed prematurely\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}



	buf[bytesRcvd] = '\0'; // Add \0 so printf knows where to stop
	printf("%s\n", buf); // Print the echo buffer

	//Pulisco il buffer
	memset(buf, 0, sizeof(buf));



	//AQUISIZIONE DI STRINGHE DA TASTIERA

	//STRINGA DA TASTIERA
	printf("Inserisci una lettera tra queste (A/S/M/D): ");

	// Acquisisce un carattere da tastiera e lo converte tutto in maiuscolo (se e' una lettera)
	CString[0] = toupper(getchar());
	fflush(stdin);	//Pulisce il buffer in output (per getchar())


	//INVIA IL CARATTERE AL SERVER
	if (send(Csocket, CString, strlen(CString), 0)!= strlen(CString))
	{
		ErrorHandler("send() sent a different number of bytes than expected\n");
		free(CString);
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	memset(CString, 0, strlen(CString));	//Azzera la variabile
	memset(buf, 0, sizeof(buf));				//Azzera il buffer



	CString = (char*)malloc((BUFFERSIZE -1) * sizeof(char));

	memset(CString, 0, strlen(CString));	//azzero la variabile


	//Restituisce la STRINGA
	if ((bytesRcvd = recv(Csocket, buf, BUFFERSIZE -1, 0)) <= 0)
	{
		ErrorHandler("recv() failed or connection closed prematurely");
		free(CString);
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	buf[bytesRcvd]='\0';	// Aggiunge il terminatore di stringa per indicare a printf dove fermarsi


	//CONTROLLA SE DEVE ESEGUIRE UN OPERAZIONE O TERMINARE IL PROCESSO

	if(((strcmp(buf,"ADDIZIONE"))==0)||((strcmp(buf,"SOTTRAZIONE"))==0)||((strcmp(buf,"MOLTIPLICAZIONE"))==0)||((strcmp(buf,"DIVISIONE"))==0))
	{
		printf("OPERATION: %s\n", buf);	//stampa operazione + stringa
	}
	else
	{
		printf("%s\n", buf);	//stampa termine processo del client
		free(CString);

		puts("Socket CLOSE");
		closesocket(Csocket);	//chiude la socket
		ClearWinSock();
		puts("Client END"); // Print a final linefeed
		system("pause");
		return 0;
	}

	//AQUISIRE INTERI DA TASTIERA

	printf("Enter two integers for %s:\n", buf);

	printf("First Integer: ");

	//finchè il numero non e' corretto (intero) continuo ad aquisire da tastiera
	while(scanf("%d",&num)!=1)
	{
		fflush(stdin);
		printf("\nIncorrect value\nPlease enter first integer: ");
	}
	fflush(stdin);	//svuoto il buffer
	sprintf(num1,"%d",num);	//conversione intero in stringa assegnazione a variabile num1


	printf("Second Integer: ");

	//finchè il numero non e' corretto (intero) continuo ad aquisire da tastiera
	while(scanf("%d",&num)!=1)
	{
		fflush(stdin);
		printf("\nIncorrect value\nPlease enter second integer: ");
	}
	fflush(stdin);

	sprintf(num2,"%d",num); //conversione intero in stringa assegnazione avariabile num2

	num=0;	//azzero la variabile num


//INVIARE GLI INTERI AL SERVER

	//INVIA IL PRIMO INTERO
	if (send(Csocket, num1, strlen(num1), 0) <= 0)
	{
		ErrorHandler("send() sent a different number of bytes than expected\n");
		free(CString);
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	//INVIA IL SECONDO INTERO
	if (send(Csocket, num2, strlen(num2), 0) <= 0)
	{
		ErrorHandler("send() sent a different number of bytes than expected\n");
		free(CString);
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}


	if ((bytesRcvd = recv(Csocket, buf, BUFFERSIZE -1, 0)) <= 0)
	{
		ErrorHandler("recv() failed or connection closed prematurely");
		free(CString);
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	buf[bytesRcvd]='\0';	// Aggiunge il terminatore di stringa per indicare a printf dove fermarsi

	printf("RESULT: %s\n\n",buf);	//stampa il risultato dell'operazione


	// CHIUSURA DELLA CONNESSIONE

	free(CString);		//dealloco l'area di memoria occupata da CString
	puts("Socket CLOSE");
	closesocket(Csocket);	//chiudo la socket
	ClearWinSock();			//termino winsock

	puts("Client END"); //Stampa chiusura del client

	system("pause");

	return(0);
}


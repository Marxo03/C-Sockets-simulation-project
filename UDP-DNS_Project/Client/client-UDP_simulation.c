/*----------------------------------------------------------------------------
 Name        : client-UDP_simulation.c
 Author      : Nemo
 Description : UDP Simulation in C - Client
 ----------------------------------------------------------------------------*/
#if defined WIN32
#include <winsock.h>        //include la libreria winsock per i compilatori con sistemi Windows
#else
#define closesocket close
#include <sys/socket.h>        //include le librerie per i compilatori con sistemi Unix/Mac
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>            //Libreria standard di input/output
#include <string.h>            //Libreria per la gestione delle stringhe
#define ECHOMAX 255            //Dimensione Buffer del Server
#define PORT 56432            //Numero di porta Default del Server

//funzione che stampa un messaggio di errore
void ErrorHandler(char *errorMessage) {
    printf("%s", errorMessage);
}

//funzione che rimuove i dati creati dalla winsock
void ClearWinSock() {
    #if defined WIN32
        WSACleanup();
    #endif
}

//MAIN
int main() {
	int Csock;                  	  //dichiaro la Socket del Server
	struct sockaddr_in echoServAddr;  //dichiara un oggetto di tipo sockaddr_in (IP + port) del Server
	struct sockaddr_in fromAddr;      //dichiara un oggetto di tipo sockaddr_in (IP + port) del Client
	unsigned int fromSize;           //dichiara la lunghezza dell' indirizzo del Server
	char echoString[ECHOMAX];        //Dichiarazione di un array di caratteri per le stringhe da inviare al server
	char echoBuffer[ECHOMAX];        //Dichiarazione di un array di caratteri per il buffer di ricezione
	int echoStringLen;               //dichiara la lunghezza della stringa da inviare al server
	int respStringLen;				 //dichiara la lunghezza della stringa ricevuta dal server
	char *serverName;                //dichiara la stringa che memorizzera' il nome canonico del server
	int port;                         //dichiara la porta del server
	int num;                          //dichiara un intero per acquisirlo da tastiera


    puts(" ______________");
    puts("| CLIENT START:|");
    puts(" --------------\n");
    #if defined WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
            printf("error at WSASturtup\n");
            printf("\033[0m"); //ripristina il colore
            return EXIT_FAILURE;
        }
    #endif

    puts("WSA CHECK");

    //Richiesta nome server
    printf("Please enter the Server name:");
    gets(serverName);

    //Richiesta porta server
    printf("Enter the port of Server:");
    scanf("%d", &port);
    fflush(stdin);

    //Risoluzione nome server
    struct hostent *hostName = gethostbyname(serverName);

    if (hostName == NULL) {
        /* Errore nella risoluzione del nome. */
        printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
        fprintf(stderr, "gethostbyname() failed.\n");
        printf("\033[0m"); //ripristina il colore
        exit(EXIT_FAILURE);
    } else {
        if (port != PORT) {
        	//se il numero di porta inserita è errato assegna il numero di porta di default
            printf("\033[0;34m"); //sequenza di escape ANSI per il colore rosso
            printf("Wrong value - default port: %d\n", PORT);
            port = PORT;
            printf("\033[0m"); //ripristina il colore
        }
    }

    // L'indirizzo IP è contenuto in host->h_addr_list[0]
    struct in_addr *addr = (struct in_addr *)hostName->h_addr_list[0];
    //printf("L'indirizzo IP di %s : %s \n", serverName, inet_ntoa(*addr));

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&echoServAddr, 0, sizeof(echoServAddr));  //Inizializzazione della struttura con zeri
    echoServAddr.sin_family = PF_INET;               //Impostazione della famiglia di indirizzi a IPv4
    echoServAddr.sin_port = htons(port);             //Impostazione della porta del server
    echoServAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*addr));  //Impostazione dell'indirizzo IP del server


    //creazione di una socket IPv4 (Protocol Family) di tipo datagram
    //che utilizza il protocollo UDP
    if ((Csock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    	//se fallisce restituisce il messaggio di errore, chiude la socket,
    	//pulisce Winsock e termina il programma
    	printf("\033[1;31m");
        ErrorHandler("socket() failed");
        printf("\033[0m"); //ripristina il colore
        closesocket(Csock);
        ClearWinSock();
        exit(EXIT_FAILURE);
    }

    puts("SOCKET CHECK");

    printf("\nInserisci la stringa echo da inviare al server\n");
    scanf("%s", echoString);

    if ((echoStringLen = strlen(echoString)) > ECHOMAX) {
    	//se fallisce restituisce il messaggio di errore, chiude la socket,
    	//pulisce Winsock e termina il programma
    	printf("\033[1;31m");
        ErrorHandler("echo word too long");
        printf("\033[0m"); //ripristina il colore
        closesocket(Csock);
        ClearWinSock();
        exit(EXIT_FAILURE);
    }

    puts("SEND CHECK\n");

    // INVIO DELLA STRINGA ECHO AL SERVER
    if (sendto(Csock, echoString, echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != echoStringLen) {
    	printf("\033[1;31m");
        ErrorHandler("sendto() sent different number of bytes than expected");
        closesocket(Csock);
        ClearWinSock();
        printf("\033[0m"); //ripristina il colore
    }

    // RITORNO DELLA STRINGA ECHO
    fromSize = sizeof(fromAddr);
    respStringLen = recvfrom(Csock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize);

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    	//se fallisce restituisce il messaggio di errore, chiude la socket,
    	//pulisce Winsock e termina il programma
    	printf("\033[1;31m");
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        printf("\033[0m"); //ripristina il colore
        closesocket(Csock);
        ClearWinSock();
        exit(EXIT_FAILURE);
    }

    echoBuffer[respStringLen] = '\0';		//Aggiunta del terminatore NULL
    printf("Received: %s\n", echoBuffer);	//stampo la stringa ricevuta



    //AQUISIRE DUE INTERI DA TASTIERA
    printf("Enter two integers for Sum:\n");
    printf("First Integer: ");
    //finchè il numero non e' corretto (intero) continuo ad acquisire da tastiera
    while (scanf("%d", &num) != 1) {
        printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
        fflush(stdin);
        printf("\nIncorrect value\nPlease enter first integer: ");
        printf("\033[0m"); //ripristina il colore
    }

    fflush(stdin); //svuoto il buffer
    sprintf(echoString, "%d", num); //conversione intero in stringa assegnazione a variabile num1


    //invio del primo intero al Server
    if (sendto(Csock, echoString, strlen(echoString), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != strlen(echoString)) {
    	printf("\033[1;31m");
        ErrorHandler("sendto() sent different number of bytes than expected");
        closesocket(Csock);
        ClearWinSock();
        printf("\033[0m"); //ripristina il colore
    }


    printf("Second Integer: ");

    //finchè il numero non e' corretto (intero) continuo ad acquisire da tastiera
    while (scanf("%d", &num) != 1) {
        printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
        fflush(stdin);
        printf("\nIncorrect value\nPlease enter second integer: ");
        printf("\033[0m"); //ripristina il colore
    }
    fflush(stdin);    //svuoto il buffer

    sprintf(echoString, "%d", num); //conversione intero in stringa assegnazione a variabile num2

    num = 0; //azzero la variabile num


    //invio del secondo intero al Server
    if (sendto(Csock, echoString, strlen(echoString), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != strlen(echoString)) {
    	printf("\033[1;31m");
        ErrorHandler("sendto() sent different number of bytes than expected");
        closesocket(Csock);
        ClearWinSock();
        printf("\033[0m"); //ripristina il colore
    }

    //RICEVERE IL RISULTATO
    respStringLen = recvfrom(Csock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize);

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    	//se fallisce restituisce il messaggio di errore, chiude la socket,
    	//pulisce Winsock e termina il programma
    	printf("\033[1;31m");
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        printf("\033[0m"); //ripristina il colore
        closesocket(Csock);
        ClearWinSock();
        exit(EXIT_FAILURE);
    }

    echoBuffer[respStringLen] = '\0';		//Aggiunta del terminatore NULL
    printf("Result: %s\n", echoBuffer);		//stampa del risultato

	free(serverName);		//dealloco l'area di memoria occupata da serverName
	puts("Socket CLOSE");   //Stampa chiusura della socket
	closesocket(Csock);	    //chiudo la socket
	ClearWinSock();			//termino winsock

	puts("Client END"); //Stampa chiusura del client

	system("pause");

    return EXIT_SUCCESS;
}

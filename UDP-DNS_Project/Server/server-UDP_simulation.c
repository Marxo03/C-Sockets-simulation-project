/*----------------------------------------------------------------------------
 Name        : server-UDP_simulation.c
 Author      : Nemo
 Description : UDP Simulation in C - Server
 ----------------------------------------------------------------------------*/
#if defined WIN32
#include <winsock.h>        //include la libreria winsock per i compilatori con sistemi Windows
#else
#define closesocket close
#include <sys/socket.h>        //include le librerie per i compilatori con sistemi Unix/Mac
#include <arpa/inet.h> 		   // Inclusione delle librerie di sistema per operazioni di rete su sistemi Unix/Mac.
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
    int Ssock; //dichiaro la Socket del Server
    struct sockaddr_in echoServAddr;  //dichiara un oggetto di tipo sockaddr_in (IP + port) del Server
    struct sockaddr_in echoClntAddr;  //dichiara un oggetto di tipo sockaddr_in (IP + port) del Client
    unsigned int cliAddrLen;          //dichiara la lunghezza dell' indirizzo del client
    char *echoBuffer = (char *)malloc(ECHOMAX - 1);  //dichiaro e inizializzo il buffer del server
    int recvMsgSize;                  //dichiara la lunghezza della stringa ricevuta dal client
    struct hostent *host;			  //Struttura per memorizzare le informazioni sull'host ottenute dalla risoluzione del nome
    int n1, n2;                       //interi per eseguire la somma
    int res = 0;                       //variabile risultato dove verra' memorizzata la somma
    puts(" ______________");
    puts("| SERVER START:|");
    puts(" --------------\n");

    //WINSOCK SETUP
    #if defined WIN32
        WSADATA WSAData;    //Struttura per Winsock

        //Inizializzazione della libreria di socket e specifica della versione
        int iresult = WSAStartup(MAKEWORD(2, 2), &WSAData);

        //Verifica se l'inizializzazione e' avvenuta con successo
        if (iresult != 0) {
            //Se si e' verificato un errore stampo un messaggio di errore e termino il programma
            ErrorHandler("Error at WSAStartup\n");
            return -1;
        }

        puts("WSA CHECK"); //Winsock inizializzata correttamente
    #endif

    puts("WSA CHECK");

    //creazione di una socket IPv4 (Protocol Family) di tipo datagram
    //che utilizza il protocollo UDP
    if ((Ssock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    	//se fallisce restituisce il messaggio di errore, chiude la socket,
    	//pulisce Winsock e termina il programma
        printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
        ErrorHandler("socket() failed");
        printf("\033[0m"); //ripristina il colore
        closesocket(Ssock);
        ClearWinSock();
        exit(EXIT_FAILURE);
    }

    puts("SOCKET CHECK");

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&echoServAddr, 0, sizeof(echoServAddr));   //Inizializzazione della struttura con zeri
    echoServAddr.sin_family = AF_INET;                //Impostazione della famiglia di indirizzi a IPv4
    echoServAddr.sin_port = htons(PORT);              //Impostazione della porta del server
    echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //Impostazione dell'indirizzo IP del server


    // BIND DELLA SOCKET
    if ((bind(Ssock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0) {
    	//se fallisce restituisce il messaggio di errore, chiude la socket,
    	//pulisce Winsock e termina il programma
    	printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
        ErrorHandler("bind() failed");
        printf("\033[0m"); //ripristina il colore
        closesocket(Ssock);
        ClearWinSock();
        exit(EXIT_FAILURE);
    }
    puts("BIND CHECK");

    puts("LISTEN...");

    //RICEZIONE DI STRINGHE ECHO DAL CLIENT
    while (1) {

        //RICEZIONE DELLA STRINGA

        cliAddrLen = sizeof(echoClntAddr);  //Determina la dimensione della struttura dell'indirizzo del client

        //Ricezione dei dati dalla socket Ssock in echoBuffer.
        recvMsgSize = recvfrom(Ssock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&echoClntAddr, &cliAddrLen);

        //Aggiunta del terminatore NULL per garantire che echoBuffer sia una stringa C valida.
        echoBuffer[recvMsgSize] = '\0';


        //Estrazione del nome dell'host del client utilizzando l'indirizzo IP dal quale è arrivato il messaggio.
        struct in_addr addr;
        addr.s_addr = inet_addr(inet_ntoa(echoClntAddr.sin_addr));

        //Utilizzo della funzione gethostbyaddr per ottenere informazioni sull'host utilizzando l'indirizzo IP.
        host = gethostbyaddr((char *)&addr, 4, AF_INET);

        //Memorizzazione del nome canonico dell'host nella variabile canonical_name.
        char *canonical_name = host->h_name;


        // Stampa del messaggio ricevuto dal client.
        printf("\nMSG: '%s'\n", echoBuffer);
        // Stampa del nome dell'host del client associato all'indirizzo IP ricevuto.
        printf("received from client with host name: %s\n", canonical_name);
        // Stampa dell'indirizzo IP del client da cui è arrivato il messaggio.
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        // Copia della stringa "ack" nel buffer echoBuffer.
        strcpy(echoBuffer, "ack");


        // RINVIA LA STRINGA ECHO AL CLIENT
        if (sendto(Ssock, echoBuffer, strlen(echoBuffer), 0, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) != strlen(echoBuffer)) {
        	printf("\033[1;31m"); //sequenza di escape ANSI per il colore rosso
            ErrorHandler("sendto() sent different number of bytes than expected");
            printf("\033[0m"); //ripristina il colore
            closesocket(Ssock);
            ClearWinSock();
            exit(EXIT_FAILURE);
        }

        // Stampa del messaggio di attesa per l'invio di due interi dal client.
        puts("Waiting integers from Client to Sum...");

        // Ricezione del primo intero dal client e conversione in formato numerico.
        recvMsgSize = recvfrom(Ssock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&echoClntAddr, &cliAddrLen);
        echoBuffer[recvMsgSize] = '\0';   //Aggiunta del terminatore NULL
        n1 = atoi(echoBuffer);    		  //conversione in formato numerico.

        // Ricezione del secondo intero dal client
        recvMsgSize = recvfrom(Ssock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&echoClntAddr, &cliAddrLen);
        echoBuffer[recvMsgSize] = '\0';   //Aggiunta del terminatore NULL
        n2 = atoi(echoBuffer); 			  //conversione in formato numerico.
        res = n1 + n2;   				  // Calcolo della somma degli interi ricevuti.

        // Conversione del risultato in formato stringa e memorizzazione in echoBuffer.
        sprintf(echoBuffer, "%d", res);
        printf("The sum of %d + %d = %d\n", n1, n2, res); // Stampa del risultato della somma.

        //INVIA LA STRINGA ECHO AL CLIENT CONTENENTE IL RISULTATO DELLA SOMMA
        if (sendto(Ssock, echoBuffer, strlen(echoBuffer), 0, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) != strlen(echoBuffer)) {
        	printf("\033[1;31m"); //sequenza di ecape ANSI per il colore rosso
            ErrorHandler("sendto() sent different number of bytes than expected");
            printf("\033[0m"); //ripristina il colore
            closesocket(Ssock);
            ClearWinSock();
            exit(EXIT_FAILURE);
        }

        puts("\nLISTEN...\n\n");
    }
}

/*
 ----------------------------------------------------------------------------
 Name        : server-TCP_simulation.c
 Author      : Nemo
 Description : TCP Simulation in C - Server
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

#define BUFFERSIZE 512		//Dimensione Buffer del Server
#define PROTOPORT 5193		//Numero di porta Default del Server
#define QLEN 6				//numero di richieste client

//funzione che stampa un messaggio di errore
void ErrorHandler (char *errorMessage){
	printf ("%s",errorMessage);
}

//funzione che rimuove i dati creati dalla winsock
void ClearWinSock ()
{
	#if defined WIN32
		WSACleanup();
	#endif
}

//MAIN
int main(int argc, char *argv[])//main start
{
	int port; //numero di porta
	int NewSocket;	//dichiaro la Socket di Benvenuto
	struct sockaddr_in sad;	//dichiaro un oggetto di tipo sockaddr_in (IP + port)
	struct sockaddr_in cad;	//dichiara un oggetto di tipo sockaddr_in (IP + port) del client
	int clientSocket;	//dichiara una socket per il client
	int clientLen;		//dichiara la lunghezza della stringa inviata dal client
	int byteRcvd = 0;	//numero di byte ricevuti
	char* string1;		//stringhe per ricevere/inviare
	char* string2;		//dati dal/al client
	int num1;			//interi per eseguire un operazione (ADDIZIONE, SOTTRAZIONE, MOLTIPLICAZIONE, DIVISIONE)
	int num2;

	puts(" ______________");
	puts("| Server Start |");
	puts(" --------------\n");


	if (argc > 1)
	{
		port = atoi(argv[1]);
		puts("Port CHECK");	//task porta
	}
	else port = PROTOPORT;	// numero di porta di Default

	if (port < 0)	//se la porta non e' corretta
	{
		//stampa che numero di porta e' sbagliato e termina l'esecuzione
		printf("Bad port number %s \n",argv[1]);
		return -1;
	}


	//WINSOCK SETUP

	#if defined WIN32
		WSADATA WSAData;	//Struttura per Winsock

		//Inizializzazione della libreria di socket e specifica della versione
		int iresult = WSAStartup(MAKEWORD(2,2), &WSAData);

		//Verifica se l'inizializzazione e' avvenuta con successo
		if (iresult != 0)
		{
			//Se si e' verificato un errore stampo un messaggio di errore e termino il programma
			ErrorHandler("Error at WSAStartup\n");
			return -1;
		}

			puts("WSA CHECK"); //Winsock inizializzata correttamente
	#endif


	//CREAZIONE DELLA SOCKET

	//creazione di una socket IPv4 (Protocol Family) di tipo stream
	//che utilizza il protocollo TCP
	NewSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

	if (NewSocket < 0)	//se qualcosa e' andato storto restituisce -1
	{
		//se fallisce restituisce il messaggio di errore, chiude la socket,
		//pulisce Winsock e termina il programma
		ErrorHandler("Creazione Socket fallita.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return -1;
	}

		puts("Socket CHECK");	//Socket creata correttamente


	//COSTRUZIONE DELL’INDIRIZZO DEL SERVER

	memset(&sad,0,sizeof(sad));	//inizializzo i campi della stuttura
	sad.sin_family = AF_INET;		//imposto la famiglia di protocolli (IPv4)
	sad.sin_addr.s_addr = inet_addr("127.0.0.1"); //imposto l'indirizzo ip del Server (su Local Host)
	sad.sin_port = htons(5193);	//imposto il valore della porta su cul il server si metterà in acolto

	//BINDING

	//associa la socket all'IP e al numero di porta e restituisce 0 se il bind ha avuto successo
	if (bind(NewSocket,(struct sockaddr*)&sad,sizeof(sad))<0)
	{
		//se fallisce restituisce il messaggio di errore, chiude la socket,
		//pulisce Winsock e termina il programma
		ErrorHandler("Bind failed.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return 0;
	}

		printf("Bind CHECK\n");			//socket associata all'IP e porta con successo


	//SERVER IN ASCOLTO

	if (listen(NewSocket,QLEN) < 0)	//prepara la socket all'ascolto
	{
		//se qualcosa e' andato storto restituisce il messaggio di errore, chiude la socket,
		//pulisce Winsock e termina il programma
		ErrorHandler("Listen Failed!\n");
		closesocket(NewSocket);
		ClearWinSock();
		return -1;
	}

		puts("Listen CHECK"); //la socket e' in ascolto

	//RICHIESTE DI CONNESIONE

	while(1)	//loop ascolto delle richieste dei client
	{
		puts("\nListen for Client..\n");

		clientLen = sizeof(cad); //Determina la dimensione della struttura dell'indirizzo del client

		//Accetta una nuova connessione in ingresso
		clientSocket = accept(NewSocket,(struct sockaddr*)&cad,&clientLen);
		//e restituisce un nuovo socket associato a quella connessione

		if (clientSocket < 0)
		{
			//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock ed esce dal loop
			ErrorHandler("Accept failed\n");
			closesocket(NewSocket);
			ClearWinSock();
			return-1;
		}

		puts("\nAccept successfull");	//richiesta accettata


		printf("Indirizzo client %s\n",inet_ntoa(cad.sin_addr)); //stampa l'indirizzo ip del client appena connesso

		char* buf = "Connessione Stabilita";		//dichiarazione e inizializzazione del buffer


		//INVIO E RICEZIONE DI DATI

		//invia la stringa "Connessione Stabilita" al Client
		if (send(clientSocket,buf,strlen(buf),0) != strlen(buf))
		{
			//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock, mette il sistema
			//in pausa ed esce dal loop
			ErrorHandler("Send sent different No of Bytes");
			closesocket(clientSocket);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}

			puts("Send CHECK");		//stringa inviata al Client

			string1= (char*)malloc(5 * sizeof(char));	//stringa

		//RICEVE DATI

		if(recv(clientSocket,string1,strlen(string1),0)!=strlen(string1))
		{
			ErrorHandler("send() sent a different number of bytes than expected\n");
			closesocket(clientSocket);
			ClearWinSock();
			return -1;
		}

		//VERIFICARE LA LETTERA

		if(isalpha(string1[0]))	//se la stringa e' una lettera
		{
			//contronto i caratteri e se rientra tra i casi vado avanti
			if((string1[0]=='A')||(string1[0]=='S')||(string1[0]=='M')||(string1[0]=='D'))
			{
				string1[1] = '\0';	// Aggiunge il terminatore di stringa per indicare a printf dove fermarsi
				printf ("Client msg: %s\n",string1);	//stampa la stringa ricevuta dal client

				byteRcvd = 0;	//azzera la variabile che contiene il numero di byte per poterlo riutilizzare

				//assegno una stringa corrispondente all'operazione
				if(string1[0]=='A')
				{
					string2 = (char*)malloc(strlen("ADDIZIONE") * sizeof(char)); //alloco in memoria byte quanto la dimensione della stringa a
					memset(string2,0,strlen(string2));
					string2= "ADDIZIONE";
				}
				else if(string1[0]=='S')
				{
					string2 = (char*)malloc(strlen("SOTTRAZIONE") * sizeof(char));	//alloco in memoria byte quanto la dimensione della stringa
					memset(string2,0,strlen(string2));
					string2= "SOTTRAZIONE";
				}
				else if(string1[0]=='M')
				{
					string2 = (char*)malloc(strlen("MOLTIPLICAZIONE") * sizeof(char)); //alloco in memoria byte quanto la dimensione della stringa
					memset(string2,0,strlen(string2));
					string2= "MOLTIPLICAZIONE";
				}
				else
				{
					string2 = (char*)malloc(strlen("DIVISIONE") * sizeof(char)); //alloco in memoria byte quanto la dimensione della stringa
					memset(string2,0,strlen(string2));
					string2= "DIVISIONE";
				}


				printf("String to CLIENT: %s\n", string2);	//Stampo la stringa da inviare al client


				//INVIA LA STRINGA AL CLIENT
				if(send(clientSocket,string2,strlen(string2),0)!=strlen(string2))
				{
					//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock, mette il sistema
					//in pausa ed esce dal loop
					ErrorHandler("Send sent a different No of bytes\n");
					closesocket(clientSocket);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}

				printf("check\n");

				string1 = (char*) malloc(BUFFERSIZE * sizeof(char)); //alloco in memoria BUFFERSIZE byte

				memset(string1,0,strlen(string1)); //imposto i byte in memoria di string1 a 0


				//RICEVERE DUE INTERI
				//num1
				if((byteRcvd=recv(clientSocket,string1,BUFFERSIZE - 1,0)) <= 0)
				{
					ErrorHandler("send() sent a different number of bytes than expected\n");
					closesocket(clientSocket);
					ClearWinSock();
					return -1;
				}

				string1[byteRcvd]= '\0';	// Aggiunge il terminatore di stringa per indicare a printf dove fermarsi

				num1 = atoi(string1);	//converte la stringa in intero e la assegna a num1

				memset(string1,0,BUFFERSIZE - 1);	//imposto i byte in memoria di string1 a 0

				//num2
				if((byteRcvd=recv(clientSocket,string1,BUFFERSIZE - 1,0)) <= 0)
				{
					ErrorHandler("send() sent a different number of bytes than expected\n");
					closesocket(clientSocket);
					ClearWinSock();
					return -1;
				}

				string1[byteRcvd]= '\0';	// Aggiunge il terminatore di stringa per indicare a printf dove fermarsi
				num2 = atoi(string1);	//converte la stringa in intero e la assegna a num2

				memset(string1,0,BUFFERSIZE -1);	//imposto i byte in memoria di string1 a 0
				printf("num1: %d\nnum2: %d\n", num1,num2);	//stampo i due interi inviati dal client

				//confronto le stringhe se corrispondono ed eseguo un operazione
				if(((strcmp(string2,"ADDIZIONE"))==0))
				{
					num1+=num2;
				}
				else if((strcmp(string2,"SOTTRAZIONE"))==0)
				{
					num1-=num2;
				}
				else if(((strcmp(string2,"MOLTIPLICAZIONE"))==0))
				{
					num1*=num2;
				}
				else if(((strcmp(string2,"DIVISIONE"))==0))
				{
					if(num2!=0)
					{
						num1/=num2;
					}
					else
					{
						//se il denominatore e' 0, invio la stringa indefinito al client e termino il suo processo
						strcpy(string1,"Indefinito");
						if(send(clientSocket,string1,strlen(string1),0)!=strlen(string1))
						{
							//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock, mette il sistema
							//in pausa ed esce dal loop
							ErrorHandler("Send sent a different No of bytes\n");
							closesocket(clientSocket);
							ClearWinSock();
							system("PAUSE");
							return -1;
						}
						continue;	//salto le istruzioni successive, senza uscire dal loop

					}

				}


				sprintf(string1,"%d",num1);	//

				if(send(clientSocket,string1,strlen(string1),0)!=strlen(string1))
				{
					//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock, mette il sistema
					//in pausa ed esce dal loop
					ErrorHandler("Send sent a different No of bytes\n");
					closesocket(clientSocket);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}


			}//end if
			else
			{
				ErrorHandler("La lettera non corrisponde a nessuna delle lettere indicate\n");
				//string2 = "TERMINE PROCESSO CLIENT\0";
				string1 = (char*)malloc(strlen("TERMINE PROCESSO CLIENT\0") * sizeof(char));

				memset(string1, 0, strlen(string1));
				string1 = "TERMINE PROCESSO CLIENT\0";

				printf("String: %s\n", string1);

				if(send(clientSocket,string1,strlen(string1),0)!=strlen(string1))
				{
					//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock, mette il sistema
					//in pausa ed esce dal loop
					ErrorHandler("Send sent a different No of bytes\n");
					closesocket(clientSocket);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}
			}
		}

		else	//se si tratta di un carattere qualsiasi
		{
			//stampo il messaggio di errore
			ErrorHandler("il carattere non corrisponde a nessuno dei caratteri indicati\n");

			//alloco in memoria per string1 tanti byte quanto la lunghezza della stringa indicata
			string1 = (char*)malloc(strlen("TERMINE PROCESSO CLIENT\0") * sizeof(char));

			memset(string1, 0, strlen(string1));	//impostare tutti i byte di string1 a 0

			string1 = "TERMINE PROCESSO CLIENT\0" ;	 //assegno la stringa a string1

			printf("String: %s\n", string1);	//stampo la stringa

			//INVIO LA STRINGA AL CLIENT E TERMINA IL PROCESSO del Client
			if(send(clientSocket,string1,strlen(string1),0)!=strlen(string1))
			{
				//Se fallisce, stampa l'errore, chiude la socket, pulisce winsock, mette il sistema
				//in pausa ed esce dal loop
				ErrorHandler("Send sent a different No of bytes\n");
				closesocket(clientSocket);
				ClearWinSock();
				system("PAUSE");
				return -1;
			}
		}

		puts("Connection to Client END");	// Stampa un linefeed di chiusura della connesione del client

	}//end while


	puts("END");
	return 0;
}//end main





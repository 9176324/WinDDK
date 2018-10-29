/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    socketserver.cpp

Abstract:

    A simple socket server application that listens on a  specified port and echoes back data 
    received. 
    
Environment:

    User Mode
        
--*/    

#include "internal.h"
HANDLE NetworkEvent;

DWORD 
Run(
    LPVOID lpThreadParameter
   ) 
 /*++

Routine Description:

    This routine is invoked  for each thread created for a new connection accepted by the server.
    The rcv and send to socket happen in this thread routine. 
 
 
Arguments:

    lpThreadParameter , The Thread parameter which contains socket information
     
Return Value:

 Thread Exit Code

    
--*/
{

    static int count =0;

    char buffer[DATA_LENGTH];
    
    DWORD Event;
    
    //
    //   Look at socket information from thread arg. 
    //
    
    PThreadData pThreadData = (PThreadData)lpThreadParameter;

    SOCKET sClient = pThreadData->m_Socket;

    printf("Client Start:  0x%lx \n", sClient);
    
    int actual = 0;
   
    WSANETWORKEVENTS NetworkEvents;
         
    for(;;)
    {
        if ((Event = WSAWaitForMultipleEvents(
                                                1, 
                                                &NetworkEvent, 
                                                FALSE,
                                                WSA_INFINITE, 
                                                FALSE)) == WSA_WAIT_FAILED)
        {
             printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
             return 0;
        }

        if (WSAEnumNetworkEvents(sClient ,NetworkEvent,  &NetworkEvents) == SOCKET_ERROR)
        {
             printf("WSAEnumNetworkEvents failed with error %d\n", WSAGetLastError());
             return 0;
        }

        if (NetworkEvents.lNetworkEvents & FD_READ)
        {
            if (NetworkEvents.lNetworkEvents & FD_READ && NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
            {
                printf("FD_READ failed with error %d\n", NetworkEvents.iErrorCode[FD_READ_BIT]);
            }
            else
            {
                
                actual =  recv(sClient,buffer,DATA_LENGTH*sizeof(char),0);
                 //
                //  socket connection has been reset ,so bail out .
                //
                if (actual == 0 || actual == WSAECONNRESET )
                {
                    printf(" Could not get data , Error : 0x%lx \n",WSAGetLastError());
                   
                    break; // socket shut-down
                                
                }
                send(sClient, (const char*)buffer,actual,0);                
                actual = 0;

            }
        }
        if (NetworkEvents.lNetworkEvents & FD_WRITE)
        {
            if (NetworkEvents.lNetworkEvents & FD_WRITE && NetworkEvents.iErrorCode[FD_WRITE_BIT] != 0)
            {
                printf("FD_WRITE failed with error %d\n", NetworkEvents.iErrorCode[FD_WRITE_BIT]);
            }
            else
            {
                send(sClient, (const char*)buffer,actual,0);
                actual = 0;
            }
        }
        if (NetworkEvents.lNetworkEvents & FD_CLOSE)
        {
            shutdown(sClient,FD_READ|FD_WRITE);
            closesocket(sClient);
            ExitThread(0);
        }

    }
    
     return 1;
    
}
 
CEchoServer::CEchoServer(
  SOCKET socketclient
 )
/*++

Routine Description:

 This is the constructor routine for CEchoServer class. This is called for each instance of new
 connection accepted by the  server . 
 
Arguments:

    Socket received from the accept 
     
Return Value:

  None .

--*/
{
    m_socket = socketclient;
     
    printf("socket created : 0x%lx \n", m_socket);
   
}

void 
CEchoServer::Start()
/*++

Routine Description:

 This routine is to Start the thread which will rcv and send the data recieved on this instance of socket connection. 
 
 
Arguments:

    None.
     
Return Value:

  None.
--*/
{

    PThreadData   pData;


    if(WSAEventSelect(
                        m_socket,
                        NetworkEvent,
                        FD_READ|FD_WRITE|FD_CLOSE)==  SOCKET_ERROR)
    {
        printf("Error in Event Select,Cannot start Server thread for this socket \n");
        closesocket(m_socket);
        goto Exit; 
    }
        //
        // Allocate memory for thread data 
        //
 
    pData = (PThreadData)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ThreadData));

    if( pData == NULL )
    {
        printf(" Could not allocate memory for thread context data ");
        closesocket(m_socket);
        goto Exit;
    }

    //
    // Assign socket information in thread data 
    //

    pData->m_Socket = m_socket;

    //
    // Create thread to read/write data to this socket
    // 
  
    HANDLE hRunThread = CreateThread(
                                                       NULL,                  // Default Security Attrib.
                                                       0,                     // Initial Stack Size,
                                                       Run,     // Thread Func
                                                       pData,    // Arg to Thread Func.
                                                       0,                     // Creation Flags
                                                       NULL                   // Don't need the Thread Id.
                                                       );
    if (NULL == hRunThread)
    {
        printf(" Could not create socket server run thread : 0x%lx \n", GetLastError()); 
        closesocket(m_socket);
        goto Exit;
    }

    Exit:

        return ; 
  
 }

void 
SocketServerMain(
  IN USHORT uPort
  )
/*++

Routine Description:

  This routine is the main entry for the app when the app is configured to 
  be a socket server. 
  It creates a a listening socket for incoming conenctions. 
 
 
Arguments:

    uPort - Port Number that the socket server binds to
     
Return Value:

  None.
--*/  
{


    SOCKET    ListenSocket      ;
    int            iResult           ;
   #pragma warning( suppress: 24002 ) // suppress warning for IPv6 ,currently IPv4 specific  
    sockaddr_in    service     ;
 
   // Initialize Winsock 2.2

   NetworkEvent = WSACreateEvent();

   
   WSADATA wsaData;
  
   iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
   
   if ( NO_ERROR != iResult )
   {
   
        printf("Error at WSAStartup()  \n");
     
        goto Exit;
    }

      //
      // Create a SOCKET for listening for  incoming connection requests.
      //
   
   
   
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   
    if ( INVALID_SOCKET == ListenSocket) 
    {
    
        printf("Error at socket(): %ld\n ", WSAGetLastError());
  
        goto Cleanup;
     
    }

   
       // The sockaddr_in structure specifies the address family,
       // IP address, and port for the socket that is being bound.
       
     
    service.sin_family = AF_INET;

    #pragma warning( suppress: 24001 ) // suppress warning for IPv6   
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    service.sin_port = htons(uPort);

    if (SOCKET_ERROR  == bind( 
                                           ListenSocket, 
                                          (SOCKADDR*) &service, 
                                          sizeof(service) )  ) 
    {
        printf("bind() failed. \n");

        closesocket(ListenSocket);

        goto Cleanup;

   }

   //
   // Listen for incoming connection requests 
   // on the created socket upto MAX_CONNECTIONS
   //
   
    if (  SOCKET_ERROR == listen( 
                                             ListenSocket, 
                                           MAX_CONNECTIONS ) )
    {

       printf("Error listening on socket.\n");
       
    }

   printf("Listening on socket...\n");

 //
 // Loop the server to start accepting connections from clients on this socket
 //
 
    for(;;)
   {

        CEchoServer *client = new CEchoServer(accept(ListenSocket,NULL,NULL));

        if (client)
        {

            printf("Client connected.\n");
          
            client->Start();  // Start receiving/sending  data on the socket 
        
        }
    }
   
Cleanup:   

 //
 // Invoke Winsock Cleanup 
 //
 
    WSACleanup();
   
 Exit:
    return;

}
void 
Usage()
/*++

Routine Description:

 This routine is invoked to display the usage of this application
 
Arguments:

    None.
     
Return Value:

  None .
--*/

{
    printf("\n\n Usage:  \n"); 
    printf(" ------   \n\n"); 
    printf(" socketechoapp                 Display Usage \n");
    printf(" socketechoapp -h              Display Usage\n");
    printf(" socketechoapp -p              Start the app as server listening on default port\n");
    printf(" socketechoapp -p [port#]      Start the app as server listening on this port \n");  
    


}


/* */
void __cdecl
main(
    __in int argc,
    __in_ecount(argc) char* argv[]
    )

/*++

Routine Description:


 
Arguments:

    None.
     
Return Value:

   None.
--*/
{
   unsigned short argIndex   =  1 ;
   unsigned short  uPort         = DEFAULT_PORT_ADDRESS ;

  if (argc < 2) 
  {
    Usage(); 
    goto Exit; 
  }

   //
  // look at second arg and check for either -h which indicates user asked for help in Usage 
  // of this commandline 
  //

  if (!strcmp(*(argv+argIndex),"-h"))
  {
  
    Usage();    
    goto Exit; 
       
  }

 // check if its -p and proceed with otherwise show usage 

  else if (!strcmp(*(argv+argIndex),"-p"))
  {
    // look at third arg, which should be the port# 

    if ( ++argIndex < argc )
    {
        uPort = (unsigned short)atoi(*(argv+(argIndex))); 
    }
    
    SocketServerMain(uPort);
        
  }
   else 
   {
   
      Usage();
      goto Exit; 
       
      }

 
Exit:
 return;
    
}



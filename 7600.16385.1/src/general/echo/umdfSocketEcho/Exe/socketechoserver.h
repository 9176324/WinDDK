/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    sockechoserver.h

Abstract:

    Header file for the socket  server module of the socketecho application 
    
Environment:

    User mode only

--*/

#pragma once


#define MAX_CONNECTIONS             5
#define DEFAULT_PORT_ADDRESS        6000
#define DATA_LENGTH                           1024
    //
    //  Thread Data context  type definition
    //
typedef struct _ThreadData
{
    SOCKET m_Socket; 
    
} ThreadData,*PThreadData;


void 
SocketServerMain(
        __in unsigned short uPort
        );

    //
    //  Class definition for CEchoServer Class 
    //
class CEchoServer 
{

    private :
        SOCKET m_socket;

    public :

        CEchoServer(SOCKET socketclient);
        void Start();
      
};



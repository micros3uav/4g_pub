/*====================================================================*/
/*                                                                    */
/*  Module File Name:    net4all.cpp                                  */
/*                                                                    */
/*  Package for SOCKET comunication (UDP & TCP).                      */
/*                                                                    */
/*  Copyright (C) 2002-2005. All rights reserved.                     */
/*                                                                    */
/*  Coded By:  �컪�� (nethead)                                       */
/*                                                                    */
/*====================================================================*/



#if _MSC_VER > 1300
  #include <iostream.h>
#else
  #include <iostream>
  using namespace std;
#endif

  

#include "net4all.h"

#ifdef  WINSOCK_VERSION
    #define  socklen_t  int
#endif

/** SOCKET�����͹ر�(����Winsock)
 */
int socketStartUp()
{
    static int  started = 0;
    if(started)  return  0;

    #ifdef  WINSOCK_VERSION
        WSADATA  wsaData;
        if (WSAStartup(WINSOCK_VERSION, &wsaData))  return  -1;
    #endif

    started = 1;
    return  0;
}

void socketCleanUp()
{
    #ifdef  WINSOCK_VERSION
        WSACleanup();	
    #endif
}

// ��ȡ�����ĵ�n��IP��ַ������ֵΪ�����ֽ���
unsigned long getHostAddress(int n)
{
    socketStartUp();

    unsigned long  in_s_addr;
    in_s_addr = htonl(INADDR_ANY);
    if( n < 0 )  return in_s_addr;

    char  host[40];
    gethostname(host, 39);
    struct hostent  *hp = gethostbyname(host);
    if(hp) if(hp->h_addr_list)
    {
        for(int k = 0; ; k ++)
        {
            if( hp->h_addr_list[k] == 0 )  break;
            if( k != n )  continue;
            memcpy(&in_s_addr, hp->h_addr_list[k], sizeof(in_s_addr));
            break;
        }
    }

    return in_s_addr;
}

//////  UDP socket for receive data  //////

Socket_UDP_R::Socket_UDP_R(unsigned short p, char *local)
{
    port = 0;

    s = INVALID_SOCKET;
    if(socketStartUp() == -1)  return;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return;

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
	if( local )
		stIt.sin_addr.s_addr = inet_addr(local);
	else
		stIt.sin_addr.s_addr = getHostAddress(-1);
    stIt.sin_port = htons(p);

    int  on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);

    if(bind(s, (struct sockaddr *)&stIt, sizeof(stIt)) == -1)
    {
        printf("Bind socket error (port=%d)\n", p);  return;
    }

    int  len = sizeof(stIt);
    getsockname(s, (struct sockaddr *)&stIt, (socklen_t *)&len);
    port = ntohs(stIt.sin_port);
}

Socket_UDP_R::Socket_UDP_R()
{
    int  len;

    s = INVALID_SOCKET;
    port = 0;
    if(socketStartUp() == -1)  return;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return;

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    stIt.sin_addr.s_addr=htonl(INADDR_ANY);
    stIt.sin_port = htons(0);

    bind(s, (struct sockaddr *)&stIt, sizeof(stIt));

    len = sizeof(stIt);
    getsockname(s, (struct sockaddr *)&stIt, (socklen_t *)&len);
    port = ntohs(stIt.sin_port);
}

// ���Ľ���ʵ�壬��̬����˿�
Socket_UDP_R::Socket_UDP_R(char *new_version, int n)
{
    int  len;

    s = INVALID_SOCKET;
    port = 0;
    if(socketStartUp() == -1)  return;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return;

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    stIt.sin_addr.s_addr = getHostAddress(n);
    stIt.sin_port = htons(0);

    bind(s, (struct sockaddr *)&stIt, sizeof(stIt));

    len = sizeof(stIt);
    getsockname(s, (struct sockaddr *)&stIt, (socklen_t *)&len);
    port = ntohs(stIt.sin_port);
}

Socket_UDP_R::Socket_UDP_R(unsigned short p, int n, int bc)
{
    port = 0;

    s = INVALID_SOCKET;
    if(socketStartUp() == -1)  return;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return;

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    unsigned long  r_address = getHostAddress(n);
#ifndef WINSOCK_VERSION
    r_address = ntohl(r_address);
    r_address |= 0xff;
    r_address = htonl(r_address);
#endif
    stIt.sin_addr.s_addr = r_address;
    stIt.sin_port = htons(p);
cout << "IP ADDR  : " << inet_ntoa(stIt.sin_addr) << endl;

    if(bind(s, (struct sockaddr *)&stIt, sizeof(stIt)) == -1)
    {
        printf("Bind socket error (port=%d)\n", p);  return;
    }

    int  len = sizeof(stIt);
    getsockname(s, (struct sockaddr *)&stIt, (socklen_t *)&len);
    port = ntohs(stIt.sin_port);
}

Socket_UDP_R::~Socket_UDP_R()
{
    if(s != INVALID_SOCKET)  closesocket(s);
    s = INVALID_SOCKET;
}

int Socket_UDP_R::recv_data(char *data, int length)
{
    if(s == INVALID_SOCKET)  return -1;
    if( !data  ||  length <= 0 )  return  0;

    socklen_t  len = sizeof(stIt);
    int  recvlen = recvfrom(s, data, length, 0, (struct sockaddr *)&stIt, &len);
    return  recvlen;
}

//////  UDP socket for send data  //////

Socket_UDP_S::Socket_UDP_S(unsigned short port, char *local)
{
    s = INVALID_SOCKET;
    if(socketStartUp() == -1)  return;
    if( port == 0xFFFF )  return;

    init_socket_broadcast(port, local);
}

Socket_UDP_S::Socket_UDP_S(char *host, unsigned short port)
{
    s = INVALID_SOCKET;
    if(socketStartUp() == -1)  return;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return;

    char  host_ip[16];
    if(*host >= '0'  &&  *host <= '9')
    {
        strncpy(host_ip, host, 15);
        host_ip[15] = '\0';
    }
    else
    {
        strcpy(host_ip, "127.0.0.1");

        struct hostent *hp = gethostbyname(host);
	    if(hp) if(hp->h_addr_list)
	    {
            struct in_addr in;
	        memcpy(&in.s_addr, *hp->h_addr_list, sizeof (in.s_addr));
	        strncpy(host_ip, inet_ntoa(in), 15);
            host_ip[15] = '\0';
	    }
    }

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    unsigned long  dwIPAddr = inet_addr(host_ip);
    memcpy((char *)&stIt.sin_addr, (char *)&dwIPAddr, 4);
    stIt.sin_port = htons(port);
}

Socket_UDP_S::Socket_UDP_S(char *dest, unsigned short port, char *local)
{
    s = INVALID_SOCKET;
    if(socketStartUp() == -1)  return;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return;

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    stIt.sin_addr.s_addr = inet_addr(local);
    stIt.sin_port = htons(0);

    bind(s, (struct sockaddr *)&stIt, sizeof(stIt));

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    stIt.sin_addr.s_addr = inet_addr(dest);
    stIt.sin_port = htons(port);
}

// ����Ŀ�ĵ�ַ���˿���Ϣ
void Socket_UDP_S::set_dest(u_long host_addr, u_short port)
{
    memcpy(&stIt.sin_addr, &host_addr, sizeof(u_long));
    stIt.sin_port = htons(port);
}

int Socket_UDP_S::send_data(char *data, int length)
{
    if(s == INVALID_SOCKET)  return -1;
    if(length <= 0)  return 0;

    int  sLen = sendto(s, data, length, 0, (sockaddr *)&stIt, sizeof(stIt));

    return sLen;
}

int Socket_UDP_S::init_socket_broadcast(unsigned short port, char *local)
{
    if(s != INVALID_SOCKET)  closesocket(s);

    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return -1;

    //
    // Set the broadcast option with setsockopt ().
    //
    int  on = 1;
    int  err = setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));
    if(err == SOCKET_ERROR)
    {
        cout << "Set socket options error." << endl;
        closesocket(s);  s = INVALID_SOCKET;
        return -1;
    }

    //
    // Fill an IP address structure, to send an IP broadcast.  
    // The packet will be broadcasted to the specified port.
    //
    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    unsigned long  bc_address = ntohl(getHostAddress(-1));
	if( local )  bc_address = ntohl(inet_addr(local));
    bc_address |= 0x00FF;
    stIt.sin_addr.s_addr = htonl(bc_address);
    stIt.sin_port = htons(port);

    return 0;
}

Socket_UDP_S::~Socket_UDP_S()
{
    if(s != INVALID_SOCKET)  closesocket(s);
    s = INVALID_SOCKET;
}


/** ��ʼ���ͻ��ˣ���ָ���ķ�������ַ�Ͷ˿�
 */
Socket_TCP_CS::Socket_TCP_CS(char *host, unsigned short p)
{
    sSock = INVALID_SOCKET;
    cSock = INVALID_SOCKET;
    port = p;

    if(socketStartUp() == -1)  
    {
      cout << "SOCKET: Unable to startUp!" <<endl;
      return;
    }

    // ����������������IP��ַ
    char  local_host[] = "127.0.0.1";
    char  *address = host ? host : local_host;

    if(*address >= '0'  &&  *address <= '9')
    {
        strncpy(m_HOST_IP, address, 15);
        m_HOST_IP[15] = '\0';
    }
    else
    {
        struct hostent *hp = gethostbyname(address);
        if(hp) if(hp->h_addr_list)
        {
            struct in_addr in;
            memcpy(&in.s_addr, *hp->h_addr_list, sizeof(in.s_addr));
            strncpy(m_HOST_IP, inet_ntoa(in), 15);
            m_HOST_IP[15] = '\0';
        }
    }

  
    // ���ӷ�����
    //connect_to_server();
    //cout << "SOCKET: initialize successed!" << endl;
}

bool Socket_TCP_CS::connect_to_server()
{
    cSock = socket(PF_INET, SOCK_STREAM, 0);
    if(cSock == INVALID_SOCKET)
    {
        cout << "SOCKET: Unable to initialize" << endl;
        return false;
    }

    unsigned long  dwIPAddr = inet_addr(m_HOST_IP);
    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    memcpy((char *)&stIt.sin_addr, (char *)&dwIPAddr, 4);
    stIt.sin_port = htons(port);

    if(connect(cSock, (struct sockaddr *)&stIt, sizeof(stIt)) == SOCKET_ERROR)
    {
        //cout << "SOCKET: Connect to server failed" << endl;
        closesocket(cSock);  cSock = INVALID_SOCKET;  return false;
    }

#ifdef WINSOCK_VERSION
    int  optval = 1;
    setsockopt(cSock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
#endif

    return true;
}

// ��ʼ���������ˣ�p=0��ʾ�Զ�����˿�
void Socket_TCP_CS::init_server(unsigned short p, int n)
{
    sSock = INVALID_SOCKET;
    cSock = INVALID_SOCKET;
    port = p;

    if(socketStartUp() == -1)  return;

    sSock = socket(AF_INET, SOCK_STREAM, 0);
    if(sSock == INVALID_SOCKET)
    {
	cout << "SOCKET: Unable to initialize" << endl;
        return;
    }

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    stIt.sin_addr.s_addr = getHostAddress(n);
    stIt.sin_port = htons(port);

    int  on = 1;
    setsockopt(sSock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
    if(bind(sSock, (struct sockaddr*)&stIt, sizeof(stIt)) )
    {
        cout << "SOCKET: Unable to bind" << endl;
        closesocket(sSock);  sSock = INVALID_SOCKET;  return;
    }

    listen(sSock, 512);

    int  len = sizeof(stIt);
    getsockname(sSock, (struct sockaddr *)&stIt, (socklen_t *)&len);
    port = ntohs(stIt.sin_port);
}

// ��ʼ���������ˣ��Զ�����Э��˿�
Socket_TCP_CS::Socket_TCP_CS()
{
    init_server(0, -1);
}

// ��ʼ���������ˣ��Զ�����˿ڣ�ָ����n��������
Socket_TCP_CS::Socket_TCP_CS(int *new_version, int n)
{
    init_server(0, n);
}

// ��ʼ���������ˣ�ָ��Э��˿�
Socket_TCP_CS::Socket_TCP_CS(unsigned short p, int n)
{
    init_server(p, n);
}

// �������˺��������ܿͻ�����
long Socket_TCP_CS::accept_connection()
{
    if(sSock == INVALID_SOCKET)  return -1;
    if(cSock != INVALID_SOCKET)  closesocket(cSock);
    
    sockaddr_in  clientAddr;
    while(1)
    {
        int  iLen = sizeof(sockaddr_in);
        cSock = accept(sSock, (struct sockaddr*)&clientAddr, (socklen_t *)&iLen);
        if(cSock != INVALID_SOCKET)  break;
    }

#ifdef WINSOCK_VERSION
    int  optval = 1;
    setsockopt(cSock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
#endif

    return clientAddr.sin_addr.s_addr;
}

// �������˺������رտͻ�����
void Socket_TCP_CS::close_connection()
{
    if(cSock != INVALID_SOCKET)  closesocket(cSock);
    cSock = INVALID_SOCKET;
}

Socket_TCP_CS::~Socket_TCP_CS()
{
    port = 0;

    if(cSock != INVALID_SOCKET)  closesocket(cSock);
    cSock = INVALID_SOCKET;

    if(sSock != INVALID_SOCKET)  closesocket(sSock);
    sSock = INVALID_SOCKET;
}

// ���ν���ָ�����ȵ����ݣ�����ʵ�ʽ��յĳ���
int Socket_TCP_CS::recv_data(char *buff, int length)
{
    if(cSock == INVALID_SOCKET)  return -1;

    if( !buff )  return -1;

    return recv(cSock, buff, length, 0);
}

// ���η���ָ�����ȵ����ݣ�����ʵ�ʷ��͵ĳ���
int Socket_TCP_CS::send_data(char *data, int length)
{
    if(cSock == INVALID_SOCKET)  return -1;

    if( !data )  return -1;

    return send(cSock, data, length, 0);
}

// ѭ������ָ�����ȵ����ݣ�����ʵ�ʽ��յĳ���
int Socket_TCP_CS::recv_data_all(char *buff, int len)
{
    if(cSock == INVALID_SOCKET)  return -1;

    if( !buff )  return -1;

    int  i = 0;

    while(i < len)
    {
        int  iRecv = recv(cSock, buff+i, len-i, 0);
        if(iRecv <= 0)  break;
        i += iRecv;
    }

    if(i < len)  return -1;

    return i;
}

// ѭ������ָ�����ȵ����ݣ�����ʵ�ʷ��͵ĳ���
int Socket_TCP_CS::send_data_all(char *data, int len)
{
    if(cSock == INVALID_SOCKET)  return -1;

    if( !data )  return -1;

    int  i = 0;

    while(i < len)
    {
        int  iSent = send(cSock, data+i, len-i, 0);
        if(iSent <= 0)  break;
        i += iSent;
    }

    if(i < len)  return -1;

    return i;
}

// ���ر�������IP����Ҫ�ͷŷ��ص��ַ�����
char* GetLocalIP(int n)
{
    if( n < 0 )  return "127.0.0.1";

    struct in_addr  in;

    in.s_addr = getHostAddress(n);

    return inet_ntoa(in);
}


Socket_MC_S::Socket_MC_S()
{
    s = INVALID_SOCKET;

    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;

    port = 0;
}

Socket_MC_S::~Socket_MC_S()
{
    if(s != INVALID_SOCKET)  closesocket(s);
    s = INVALID_SOCKET;
}

int Socket_MC_S::init_socket_multicast(char *multicat_ip, u_short p, int n)
{
    if(socketStartUp() == -1)  return -1;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if(s == INVALID_SOCKET)  return -1;

    //�������Ķ��ʵ��������ͬһ̨������
    const int  on = 1;
    int  ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if( ret == SOCKET_ERROR )
    {
#ifdef  WINSOCK_VERSION
        WSACleanup();
#endif
        cout << "Error in setsockopt(SO_REUSEADDR)." << endl;
        return -1;
    }

    // Time To Live
    const int  routenum = 10;
    ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&routenum, sizeof(routenum));
    if( ret == SOCKET_ERROR )
    {
#ifdef  WINSOCK_VERSION
        WSACleanup();
#endif
        cout << "Error in setsockopt(IP_MULTICAST_TTL)." << endl;
        return -1;
    }
/*
    //��ֹ����
    const int  loopback = 0;
    ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loopback, sizeof(loopback));
    if( ret == SOCKET_ERROR )
    {
#ifdef  WINSOCK_VERSION
        WSACleanup();
#endif
        cout<< "Error in setsockopt(IP_MULTICAST_LOOP)." << endl;
        return -1;
    }
*/
    // �˿ڰ�
    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_family = AF_INET;
    stIt.sin_addr.s_addr = getHostAddress(n);
    stIt.sin_port = htons(p);

    if(bind(s, (struct sockaddr *)&stIt, sizeof(stIt)) == -1)
    {
#ifdef  WINSOCK_VERSION
        WSACleanup();
#endif
        printf("Bind socket error (port=%d)\n", p);
        return -1;
    }

    //����һ���ಥ��
    ip_mreq  mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_interface.s_addr = getHostAddress(n);
    mreq.imr_multiaddr.s_addr = inet_addr(multicat_ip);

    ret = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
    if( ret == SOCKET_ERROR )
    {
#ifdef  WINSOCK_VERSION
        WSACleanup();
#endif
        cout << "Error in setsockopt(IP_ADD_MEMBERSHIP)." << endl;
        return -1;
     }

    // Fill an IP address structure.  
    port = p;
    memset(&stIt, 0, sizeof(stIt));
    stIt.sin_addr.s_addr = inet_addr(multicat_ip);
    stIt.sin_family = AF_INET ;
    stIt.sin_port = htons(port);

    return 0;
}

int Socket_MC_S::send_data(char *data, int length)
{
    if(s == INVALID_SOCKET)  return -1;
    if(length <= 0)  return 0;

    int  sLen = sendto(s, data, length, 0, (sockaddr *)&stIt, sizeof(stIt));

    return sLen;
}

int Socket_MC_S::recv_data(char *data, int length)
{
    if(s == INVALID_SOCKET)  return -1;
    if( !data  ||  length <= 0 )  return  0;

    int  recvlen = recv(s, data, length, 0);
    return  recvlen;
}

#ifndef _Net4all_Include_
#define _Net4all_Include_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
    #include <winsock.h>
    #ifndef WINSOCK_VERSION
        #define  WINSOCK_VERSION	0x0002
    #endif
    #define  SIGPIPE		SIGINT
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #define  INVALID_SOCKET     -1
    #define  SOCKET_ERROR       -1
    #define  closesocket(s)     close(s)
    typedef  int   SOCKET;
#endif

///////////////////////////////////////////////////////

// Prototype declearation for socket utilties

// Version:  Release 1.0

// first create: 1999-10-28

// Last modify:  2000-08-12
// Last modify:  2002-05-20
// Last modify:  2004-03-12
// Last modify:  2006-06-29

///////////////////////////////////////////////////////

/** SOCKET start-up & clean-up(for Winsock)
 */
extern int   socketStartUp();
extern void  socketCleanUp();
extern unsigned long getHostAddress(int n);

class Socket_UDP_R
{
public:
    SOCKET          s;                 // ���ڵ��SOCKET��Դ
    sockaddr_in     stIt;              // ʵ��ӳ��������ַ
    unsigned short  port;              // ʵ�ʰ󶨵Ľ��ն˿�
    
    Socket_UDP_R(unsigned short p, char *local=NULL);   // ���Ľ���ʵ�壬ָ���˿ڣ�ָ��������ͨ��IP��
    Socket_UDP_R();                                     // ���Ľ���ʵ�壬��̬����˿�
    Socket_UDP_R(char *new_version, int n);             // ���Ľ���ʵ�壬��̬����˿ڣ�ָ��������
    Socket_UDP_R(unsigned short p, int n, int);         // �㲥���Ľ��գ�ָ���˿ڣ�ָ��������

   ~Socket_UDP_R();

    int recv_data(char *data, int length);
};

class Socket_UDP_S
{
public:
    SOCKET          s;                  // ���ڵ��SOCKET��Դ
    sockaddr_in     stIt;               // Ŀ�ĵ�ַ���˿���Ϣ

    Socket_UDP_S(unsigned short port, char *local=NULL);         // ���ķ��ͣ��㲥��ʽ��ָ��������ͨ��IP��
	Socket_UDP_S(char *dest, unsigned short port, char *local);  // ���ķ��ͣ���Ե㷽ʽ��ָ��������ͨ��IP��
    Socket_UDP_S(char *host, unsigned short port);               // ���ķ��ͣ���Ե㷽ʽ�����޶�����
   ~Socket_UDP_S();

    void set_dest(u_long host_addr, unsigned short port); // ����Ŀ�ĵ�ַ���˿���Ϣ
    int send_data(char *data, int length);
    int init_socket_broadcast(unsigned short port, char *local=NULL); 
};

class Socket_TCP_CS
{
    void init_server(unsigned short p, int n);     // ��ʼ���������ˣ�ָ����n��������

public:
    SOCKET          sSock;              // ��������SOCKET���Կͻ���ʵ����Ч
    SOCKET          cSock;              // �ͻ���SOCKET�����ڽ��պͷ���
    sockaddr_in     stIt;               // ʵ��ӳ��������ַ
    unsigned short  port;               // ʵ�ʷ����Э��˿ں�
    char  m_HOST_IP[16];
   
    Socket_TCP_CS(char *host, unsigned short p);   // ��ʼ���ͻ��ˣ���ָ���ķ�������ַ�Ͷ˿�

    Socket_TCP_CS();                               // ��ʼ���������ˣ��Զ�����˿�
    Socket_TCP_CS(int *new_version, int n);       // ��ʼ���������ˣ��Զ�����˿ڣ�ָ����n��������
    Socket_TCP_CS(unsigned short p, int n=-1);     // ��ʼ���������ˣ�ָ��Э��˿ڣ�ָ����n��������

    long accept_connection();                      // �������˺��������ܿͻ�����
    void close_connection();                       // �������˺������رտͻ�����

   ~Socket_TCP_CS();

    bool connect_to_server();
    int recv_data(char *buff, int length);         // ���ν���ָ�����ȵ����ݣ�����ʵ�ʽ��յĳ���
    int send_data(char *data, int length);         // ���η���ָ�����ȵ����ݣ�����ʵ�ʷ��͵ĳ���
    int recv_data_all(char *buff, int length);     // ѭ������ָ�����ȵ����ݣ�����ʵ�ʽ��յĳ���
    int send_data_all(char *data, int length);     // ѭ������ָ�����ȵ����ݣ�����ʵ�ʷ��͵ĳ���
};

/** ���ر�������IP����Ҫ�ͷŷ��ص��ַ�����
 */
extern char*  GetLocalIP(int n=0);

/** �ಥ����
 */
class Socket_MC_S
{
public:
    SOCKET          s;                  // ����˿��׽���
    sockaddr_in     stIt;               // ��ַ��Ϣ
    unsigned short  port;               // Э��˿ں�

    Socket_MC_S();
   ~Socket_MC_S();

    int init_socket_multicast(char *multicat_ip, unsigned short p, int n);

    int send_data(char *data, int length);
    int recv_data(char *data, int length);
} ;

#endif

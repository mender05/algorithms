#include "common.h"
#include "socket.h"

uint8 buildSocket(char *argv[])
{
    /* ��ȡ������� */
    in_addr_t server_ip = inet_addr(argv[1]);
    in_port_t server_port = htons(atoi(argv[2])); 
    in_addr_t my_ip = inet_addr(argv[3]);
    in_port_t my_port = htons(atoi(argv[4])); 
//    int my_id = atoi(argv[5]);

    /* ����socket */
    m_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket_id < 0)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    /* ����socketѡ���ַ�ظ�ʹ�ã���ֹ����������˳����´�����ʱbindʧ�� */
    int is_reuse_addr = 1;
    setsockopt(m_socket_id, SOL_SOCKET, SO_REUSEADDR, (const char*)&is_reuse_addr, sizeof(is_reuse_addr));

    /* ��ָ��ip��port����Ȼ�ᱻserver�ܾ� */
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = my_ip;
    my_addr.sin_port = my_port;
    if(bind(m_socket_id, (struct sockaddr*)&my_addr, sizeof(my_addr)))
    {
        printf("bind error: %s(errno: %d)\n", strerror(errno), errno); 
        return -1;
    }

    /* ����server */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = server_ip;
    server_addr.sin_port = server_port;
    while(connect(m_socket_id, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        usleep(100*1000); /* sleep 100ms, Ȼ�����ԣ���֤����server�����Ǻ��𣬶����������� */
    }
    return 0;
}
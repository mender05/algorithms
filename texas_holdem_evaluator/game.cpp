#include "common.h"
#include "socket.h"
#include "handle.h"

#include "evaluator.h"

#ifdef	DEBUG
//�Զ�������˳�log��־�ļ���
char logname[30] = {'\0'};
#endif

//socketId���ⲿ�����������е�socket�շ�
int m_socket_id = -1;

//�˿�����Ϣ��������ɫ�͵�ֵ
CARD m_card[7];

//�Լ���Ǯ����Ϣ
MONEY m_money;

//�Լ���ID���
PLAYER_ID m_playid;

//��ȡ�׳رȽ���Ϣ
char pot_com[] = {"total"};

//���ڼ�¼��õ��˿��Ƶ�����
uint8 poker_num;

//���ڱ��淢�͵�����
char *cmd_point[] = {"check", "call", "raise", "all_in", "fold"};

//���ڼ�¼�Ƿ�����all_in����
bool is_all_in = false;

//����ͳ���������
int player_num;

//����ͳ�Ƽ�ע�ĳ���ֵ
int raise_num;
//*������ȫ�ֱ������������ṹ��Ķ���

int main(int argc, char *argv[])
{
	//�ж�����Ĳ��������Ƿ���ȷ
	if (argc != 6)
	{
		printf("Usage: ./%s server_ip server_port my_ip my_port my_id\n", argv[0]);
		return -1;
	}
	//����socketͨ�Žӿڣ�socketID������m_socket_id��
	uint8 flag = buildSocket(argv);
	if(flag != 0)
	{
		printf("building the socket occur something wrong!%d", flag);
		return flag;
	}
	/* ��serverע�� */
	/* reg: pid pname eol */
	char reg_msg[50] = {'\0'};
	uint8 pidlen = 0;
	while(argv[5][pidlen] != '\0')
	{
		++pidlen;
	}
	m_playid.id_length = pidlen;
	memcpy(m_playid.idbuff, argv[5], pidlen);
	m_playid.idbuff[pidlen+1] = '\0';
	snprintf(reg_msg, sizeof(reg_msg) - 1, "reg: %s %s \n", argv[5], "sunflower"); 
	send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);

#ifdef DEBUG
	//��ʼ��logname�����ڱ���log��־�ļ���
	snprintf(logname, sizeof(logname)-1, "/home/game/%s.txt", argv[5]);
#endif
	//�������У���ʼ�����й�����Ϣ
	memset((uint8*)(&m_money), 0, sizeof(m_money));

	/* ����server��Ϣ��������Ϸ */	
	while(1)
	{
		char buffer[1024] = {'\0'};
		int length = recv(m_socket_id, buffer, sizeof(buffer) - 1, 0);
		if(length > 0)
		{  
			//���ú����Խ��յ�����Ϣ���д���
			if (-1 == handle_recv_msg(buffer, length, argv[5]))
			{
				break;
			}
		} 
	}

	/* �ر�socket */
	close(m_socket_id);

	return 0;
}
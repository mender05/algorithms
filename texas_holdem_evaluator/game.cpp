#include "common.h"
#include "socket.h"
#include "handle.h"

#include "evaluator.h"

#ifdef	DEBUG
//自定定义的退出log日志文件名
char logname[30] = {'\0'};
#endif

//socketId，外部变量用于所有的socket收发
int m_socket_id = -1;

//扑克牌信息，包括花色和点值
CARD m_card[7];

//自己金钱的信息
MONEY m_money;

//自己的ID编号
PLAYER_ID m_playid;

//获取底池比较信息
char pot_com[] = {"total"};

//用于记录获得的扑克牌的张数
uint8 poker_num;

//用于保存发送的命令
char *cmd_point[] = {"check", "call", "raise", "all_in", "fold"};

//用于记录是否发送了all_in命令
bool is_all_in = false;

//用于统计玩家人数
int player_num;

//用于统计加注的筹码值
int raise_num;
//*以上是全局变量的声明、结构体的定义

int main(int argc, char *argv[])
{
	//判断输入的参数个数是否正确
	if (argc != 6)
	{
		printf("Usage: ./%s server_ip server_port my_ip my_port my_id\n", argv[0]);
		return -1;
	}
	//生成socket通信接口，socketID保存在m_socket_id中
	uint8 flag = buildSocket(argv);
	if(flag != 0)
	{
		printf("building the socket occur something wrong!%d", flag);
		return flag;
	}
	/* 向server注册 */
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
	//初始化logname，用于保存log日志文件名
	snprintf(logname, sizeof(logname)-1, "/home/game/%s.txt", argv[5]);
#endif
	//程序运行，初始化所有公共信息
	memset((uint8*)(&m_money), 0, sizeof(m_money));

	/* 接收server消息，进入游戏 */	
	while(1)
	{
		char buffer[1024] = {'\0'};
		int length = recv(m_socket_id, buffer, sizeof(buffer) - 1, 0);
		if(length > 0)
		{  
			//调用函数对接收到的信息进行处理
			if (-1 == handle_recv_msg(buffer, length, argv[5]))
			{
				break;
			}
		} 
	}

	/* 关闭socket */
	close(m_socket_id);

	return 0;
}
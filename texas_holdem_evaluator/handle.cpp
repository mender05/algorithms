#include "common.h"
#include "socket.h"
#include "handle.h"

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <string>

#include <ctime>
#include <climits>
#include "evaluator.h"

#ifdef DEBUG
extern char logname[30];
#endif

//声明外部引用变量
extern CARD m_card[7];
extern MONEY m_money;
//自己的ID编号
extern PLAYER_ID m_playid;
extern char pot_com[];
extern uint8 poker_num;
extern int _MAX_VALUE_[3];
extern bool is_all_in;
extern char *cmd_point[];
extern int player_num;
extern int raise_num;

int handle_recv_msg(char *buffer, int length, char *uid)
{
    //对buffer进行判断
    if(buffer == NULL)
    {
        return -1;
    }
    switch (buffer[0])
    {
        case 's':
        {
            
            if(buffer[1] == 'e')
            {
                //对seat命令进行处理
                seat_response(buffer, length);
            }
            else
            {
                //对showdown命令进行处理
                showdown_response(buffer, length);
            }
            break;
        }
        case 'g':
        {
            //对game_over命令进行处理
            game_over_response(buffer, length);
            return -1;
            break;
        }
        case 'b':
        {
            //对blind命令进行处理
            blind_respose(buffer, length);
            break;
        }
        case 'h':
        {
            //对hold命令进行处理
            hold_response(buffer, length);
            break;
        }
        case 'i':
        {
            //对inquire命令进行处理
            if(~is_all_in)
                inquire_response(buffer, length);
            break;
        }
        case 'f':
        {
            
            //对flop命令进行处理
            flop_response(buffer, length);
            break;
        }
        case 't':
        {
            //对turn命令进行处理
            turn_response(buffer, length);
            break;
        }
        case 'r':
        {
            //对river命令进行处理
            river_response(buffer, length);
            break;
        }
        case 'p':
        {
            //对pot_win命令进行处理
            pot_win_response(buffer, length);
            break;
        }
        default :
            break;
    }
    return 0;
}
//对接收到的牌值进行计算，有c_val返回该值
//c_val = (0-3)*13[花色] + (0-12)[牌值];
//SPADES、HEARTS、CLUBS、DIAMONDS有common中枚举了
int storeCard(uint8 *buffer, uint8 start, uint8 &c_val)
{
    int i = start;
    c_val = 0;
    //由于花色的字符个数确定，直接加上具体值即可
    if(buffer[i] == 'S')
    {
        c_val = SPADES * 13;
        i += 7;
    }else if(buffer[i] == 'H')
    {
        c_val = HEARTS * 13;
        i += 7;
    }else if(buffer[i] == 'C')
    {
        c_val = CLUBS * 13;
        i += 6;
    }else if(buffer[i] == 'D')
    {
        c_val = DIAMONDS * 13;
        i += 9;
    }
    if(buffer[i] > '0' && buffer[i] <= '9')
    {
        c_val += (uint8)(buffer[i] - '0' - 1);
    }
    //这里A定义为0，后续稍微修改即可
    if(buffer[i] == 'A')
        c_val += 0;
    if(buffer[i] == '1')
        c_val += 9;
    if(buffer[i] == 'J')
        c_val += 10;
    if(buffer[i] == 'Q')
        c_val += 11;
    if(buffer[i] == 'K')
        c_val += 12;
    while(buffer[i] != '\n')
    {
        ++i;
    }
    return i+1;
}
//log中seat信息
//seat/ 
//button: 1111 2000 8000 
//small blind: 4444 2000 8000 
//big blind: 8888 2000 8000 
//6666 2000 8000 
//3333 2000 8000 
//5555 2000 8000 
//7777 2000 8000 
//2222 2000 8000 
///seat 
//seat命令的处理
void seat_response(char *buffer, int length)
{
    //初始化全局变量
    poker_num = 0;

    for(int i=0;i<7;++i)
    {
        m_card[i].color_val = 0;
        m_card[i].card_val =0;
    }
    
    memset(_MAX_VALUE_, 0, sizeof(int)*3);
    is_all_in = false;
    int i = 0;
    //跳过seat/ eol
    while(buffer[i]!='\n' && i<length)
    {
        ++i;
    }
    if(i<length+1)
    {
        ++i;
    }
    //统计人数
    int j = i;
    player_num = 0;
    while(buffer[j] != '/' && j < length)
    {
        if(buffer[j] == '\n')
            ++player_num;
        ++j;
    }
    //清空raise_num加注筹码的值
    raise_num = 0;
#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "the seat_msg is : \n%s",buffer);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif

    //判断不到/seat eol，则不断调用store_m_val函数，当存储了自己的
    //筹码时推出
    while(buffer[i] != '/' && i<length)
    {
        //传入引用类型的下标，让下标跟着增加
        if(store_m_val(buffer, i) == 0)
        {

            break;
        }
    }
}
//存储机子当前的筹码值
//传入下标引用，随着判断增加下标的值
int store_m_val(char *buffer, int &i)
{
    int j = i;
    //区分button/big blind
    if(buffer[i] == 'b')
    {
        //跳过一个字符
        ++i;
        if(buffer[i] == 'u')
        {
            //button、跳过7个字符，指向PID
            i += 7;
#if 0
            FILE *input;
            input = fopen(logname, "a+");
            fprintf(input, "%s, %c\n", "is button", buffer[i]);
            if(fclose(input) != 0)
            {
                perror("fclose");
                exit(EXIT_FAILURE);
            }
#endif
        }
        else
        {
            //big blind、跳过10个字符，指向PID
            i += 10;
#if 0
            FILE *input;
            input = fopen(logname, "a+");
            fprintf(input, "%s, %c\n", "is big blind", buffer[i]);
            if(fclose(input) != 0)
            {
                perror("fclose");
                exit(EXIT_FAILURE);
            }
#endif
        }
    }
    else
    {
        if(buffer[i] == 's')
        {
            //small blind、跳过13个字符，指向PID
            i += 13;
#if 0
            FILE *input;
            input = fopen(logname, "a+");
            fprintf(input, "%s, %d\n", "is small blind", buffer[i]);
            if(fclose(input) != 0)
            {
                perror("fclose");
                exit(EXIT_FAILURE);
            }
#endif
        }
    }
    //获取pid的长度、下标i跟着走
    //PID不包括后面的空格' '
    int pidlen = 0;
    while(buffer[i] != ' ')
    {
        ++i;
        ++pidlen;
    }

#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "%d, %d %d, %c, %c\n", pidlen, (int)m_playid.id_length, i, buffer[i], buffer[i-pidlen]);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif

    //比较是否ID一致
    //m_playid结构体在game.cpp中初始化为参数5
    //pid为座位信息中的pid，pidlen为不包括空格' '的pid长度
    if(isTheSame(m_playid.idbuff, &buffer[i-pidlen], (int)m_playid.id_length, pidlen))
    {
        //pid相同，保存筹码和金额
        //++i跳过最后的换行符
        ++i;
        //将char转换成int，传入引用i，让i跟着增大、返回的i指向跳过空格' '的字符
        charToInt(buffer, i, m_money.jetton_val);
        charToInt(buffer, i, m_money.money_val);
        if(buffer[j] == 's')
        {
            m_money.jetton_val -= 20;
        }
        else
        {
            if(buffer[j] == 'b')
            {
                if(buffer[j+1] == 'l')
                {
                    m_money.jetton_val -= 40;
                }
            }
        }
#if 0
        FILE *input;
        input = fopen(logname, "a+");
        fprintf(input, "in isTheSame m_money.jetton_val = %d, m_money.money_val = %d\n",m_money.jetton_val, m_money.money_val);
        if(fclose(input) != 0)
        {
            perror("fclose");
            exit(EXIT_FAILURE);
        }
#endif
        return 0;
    }
    else
    {
        //如果ID不一致，则跳到下一行开始
        while(buffer[i] != '\n')
        {
            ++i;
        }
        ++i;
    }
    return 1;
}

void showdown_response(char *buffer, int length)
{
    
}

void game_over_response(char *buffer, int length)
{
    
}

void blind_respose(char *buffer, int length)
{
    
}
//log中的hold信息
///hold 
//player 8888:hold/这行应该没有的 
//HEARTS 9 
//DIAMONDS 6 
///hold

void hold_response(char *buffer, int length)
{
    //发出2张牌
    poker_num = 2;

    int i = 0;
    uint8 c_val = 0;
    //总共5行，跳过第1行
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
    //保存第1张牌，传入引用，返回指向下1行的首字符的下标
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[0].card_val = c_val % 13;
    m_card[0].color_val = c_val / 13;
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[1].card_val = c_val % 13;
    m_card[1].color_val = c_val / 13;
#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "%s\n", buffer);
    fprintf(input, "the hold_msg is : %d %d %d %d\n",m_card[0].card_val, m_card[0].color_val, m_card[1].card_val, m_card[1].color_val);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
}
//inquire/ 
//3333 1800 8000 200 call 
//6666 1800 8000 200 raise 
//8888 1900 8000 100 blind 
//4444 1950 8000 50 blind 
//total pot: 550 
///inquire
void inquire_response(char *buffer, int length)
{
#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "the inquire_msg is : \n%s",buffer);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
    //跳过第1行
    if(length < 20)
    {
        return ;
    }
    // mender++
    my_money_update(buffer, length);
    // ++mender
    int i = 0;
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
    //提取上一次下注值的大小与动作
    int last_val = 0;
    int last_action = 0;
    int max_val = 0;
    while(i<length && buffer[i] != 't')
    {
        last_handle(buffer, i, last_action, last_val);
        max_val = max_val>last_val?max_val:last_val;
    }
    last_val = max_val;
    if(last_val == 0 && last_action == 0)
    {
        //弃牌
        sendCmd(FOLD, 0);
        return;
    }
    while(buffer[i] != '/' && i<length)
    {
        //传入下标引用i，对pot信息进行处理，i随着处理增加
        if(store_m_pot(buffer, i) == 0)
        {
            /*char temp[] = {"all_in \n"};
            send(m_socket_id, temp, strlen(temp) + 1, 0);*/

#if 0
            FILE *input;
            input = fopen(logname, "a+");
            fprintf(input, "in isTheSame m_money.pot_val = %d, pot_com = %s, i = %d\n", m_money.pot_val, pot_com, i);
            if(fclose(input) != 0)
            {
                perror("fclose");
                exit(EXIT_FAILURE);
            }
#endif
            break;
        }
    }
    //到这里获得了当前底池、当前自己的筹码值、当前牌型的最大值
    //上一个动作、代价多大
    int cur_action = 0;
    // mender++ 添加一个变量，表示加注金额
    int cur_val = 0;
    // ++mender
    int card2_level = 0;
//    int poker_max_val = 0;
    if(poker_num == 2)
    {
        //2张牌的处理
        poker2_handle(card2_level);

        switch (card2_level)
        {
        case 0:
            {
                level0(last_action, last_val, cur_action);
                break;
            }
        case 1:
            {
                level1(last_action, last_val, cur_action);
                break;
            }
        case 2:
            {
                level2(last_action, last_val, cur_action);
                break;
            }
        case 3:
            {
                level3(last_action, last_val, cur_action);
                break;
            }
        case 4:
            {
                level4(last_action, last_val, cur_action);
                break;
            }
        default:
            {
                level0(last_action, last_val, cur_action);
                break;
            }
        }
    }
    else if(poker_num >4 && poker_num < 8)
    {
        ///5、6、7张牌的处理;
        // mender++
        poker5_handle(last_action, last_val, cur_action, cur_val);
        // ++mender
    }
    else
    {
        //都不是说明没有收到hold命令，保守起见弃牌
        cur_action = FOLD;
        cur_val = 0;
    }
    //先换成call
//    cur_action = CALL;
    sendCmd(cur_action, cur_val);
}
//两张牌的处理
void poker2_handle(int &card2_level)
{
    if((m_card[0].card_val == 0 && m_card[1].card_val == 0) ||
        (m_card[0].card_val == 12 && m_card[1].card_val == 12) ||
        (m_card[0].card_val == 11 && m_card[1].card_val == 11) ||
        (m_card[0].card_val == 10 && m_card[1].card_val == 10)
         )
    {
        card2_level = 1;
        return ;
    }
    if( (m_card[0].card_val == 9 && m_card[1].card_val == 9) ||
        (m_card[0].card_val == 8 && m_card[1].card_val == 8) ||
        (m_card[0].card_val == 7 && m_card[1].card_val == 7) ||
        (m_card[0].card_val == 6 && m_card[1].card_val == 6))
    {
        card2_level = 2;
        return ;
    }
    if(    (m_card[0].color_val == m_card[1].color_val && (m_card[0].card_val > 11 || m_card[1].card_val >11) 
        || m_card[0].card_val == 0 || m_card[0].card_val == 0) ||
        (m_card[0].card_val == 5 && m_card[1].card_val == 5)
        )
    {
        card2_level = 3;
        return ;
    }
    if(
    (m_card[0].card_val == 0 || m_card[1].card_val == 11) ||
    (m_card[0].card_val == 11 || m_card[1].card_val == 0) ||
    (m_card[0].card_val == 0 || m_card[1].card_val == 12) ||
    (m_card[0].card_val == 12 || m_card[1].card_val == 0))
    {
        card2_level = 4;
        return ;
    }
    card2_level = 0;
}

//发送命令
void sendCmd(const int &action, const int &cur_val)
{
    char sendBuff[20] = {'\0'};
    
    if(action == RAISE)
    {
        sprintf(sendBuff, "%s %d \n", cmd_point[action], cur_val);
    }
    else
    {
        sprintf(sendBuff, "%s \n", cmd_point[action]);
    }
    send(m_socket_id, sendBuff, strlen(sendBuff) + 1, 0);
    if(action == ALL_IN || m_money.jetton_val == 0)
    {
        is_all_in = true;
    }
    return;
}

//提取上一次下注值的大小与动作
void last_handle(char *buffer, int &i, int &last_action, int &last_val)
{
    for(int j=0; j<3; ++j)
    {
        while(buffer[i] != ' ')
            ++i;
        ++i;
    }
#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "last_handle buffer[%d] = %c\n", i, buffer[i]);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
    //计算last_val的值
    charToInt(buffer, i, last_val);
#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "last_handle buffer[%d] = %c\n", i, buffer[i]);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif    
    switch (buffer[i])
    {
    case 'a' :
        {
            last_action = ALL_IN;
            break;
        }
    case 'r' :
        {
            last_action = RAISE;
            break;
        }
    case 'f' :
        {
            last_action = FOLD;
            break;
        }
    case 'c' :
        {
            if(buffer[++i] == 'h')
                last_action = CHECK;
            else
                last_action = CALL;
            break;
        }
    default:
        {
            last_action = ALL_IN;
            break;
        }
    }
    //跳过当前行
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
}

//最保守的处理，可以跟则跟，不可以就弃牌
void level0(const int &last_action, const int &last_val, int &cur_action)
{
    if(last_action == CHECK)
    {
        cur_action = CHECK;
        return;
    }
    int val = rand();
    val %= 200;
    if(last_val < 0)
    {
        cur_action = CALL;
        if(m_money.jetton_val > last_val)
        {
            m_money.jetton_val -= last_val;
        }
        else
        {
            m_money.jetton_val = 0;
        }
    }
    else
    {
        cur_action = FOLD;
    }
    return;
}
//牌型最好，不考虑pot，由于有了2张牌的判断，所以直接跟
void level1(const int &last_action, const int &last_val, int &cur_action)
{
    cur_action = CALL;
    if(m_money.jetton_val > last_val)
    {
        m_money.jetton_val -= last_val;
    }
    else
    {
        m_money.jetton_val = 0;
    }
}
//牌型值第二大，pot大于jetton的1/2
void level2(const int &last_action, const int &last_val, int &cur_action)
{
    if(player_num < 5)
    {
        /*if(last_val > m_money.jetton_val/4)*/
        if(last_val > 1000)
        {
            cur_action = FOLD;
            return;
        }
        cur_action = CALL;
        if(m_money.jetton_val > last_val)
        {
            m_money.jetton_val -= last_val;
        }
        else
        {
            m_money.jetton_val = 0;
        }
        return;
    }
    cur_action = FOLD;
}
//牌型值第三大，pot大于jetton的1/4，代价不超过jetton的1/4
void level3(const int &last_action, const int &last_val, int &cur_action)
{
    if(player_num < 4)
    {
        /*if(last_val > m_money.jetton_val/8)*/
        if(last_val > 600)
        {
            cur_action = FOLD;
            return;
        }
        cur_action = CALL;
        if(m_money.jetton_val > last_val)
        {
            m_money.jetton_val -= last_val;
        }
        else
        {
            m_money.jetton_val = 0;
        }
        return;
    }
    cur_action = FOLD;
}
//牌型值第四大，代价不超过200
void level4(const int &last_action, const int &last_val, int &cur_action)
{
    if(player_num < 3)
    {
        if(last_val > 200)
        {
            cur_action = FOLD;
            return;
        }
        cur_action = CALL;
        if(m_money.jetton_val > last_val)
        {
            m_money.jetton_val -= last_val;
        }
        else
        {
            m_money.jetton_val = 0;
        }
        return;
    }
    cur_action = FOLD;
}

//处理inquire底池信息
int store_m_pot(char *src, int &index)
{
#if 0
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "in store_m_pot pot_com = %s, index = %d ",pot_com, index);
    fprintf(input, "src[index] = %c\n", *(src+index));
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
    //获取首个空格之间的字符长度
    int pot_len = 0;
    while(src[index] != ' ')
    {
        ++index;
        ++pot_len;
    }
    //判断与"total"是否一致
    if(isTheSame(&src[index - pot_len], pot_com, pot_len, pot_com_len))
    {
        ++index;
        while(src[index] != ' ')
        {
            ++index;
        }
        index += 1;
#if 0
//    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "in isTheSame， index = %d, src[%d] = %c\n", index, index, *(src+index));
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
        //计算pot的值
        charToInt(src, index, m_money.pot_val);
        index += 3; 
        return 0;
    }
    else
    {
        while(src[index] != '\n')
        {
            ++index;
        }
        ++index;
    }
    return 1;
}
//与hold_response一致
void flop_response(char *buffer, int length)
{
    //发出3张公共牌
    poker_num = 5;

    int i = 0;
    uint8 c_val = 0;
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[2].card_val = c_val % 13;
    m_card[2].color_val = c_val / 13;
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[3].card_val = c_val % 13;
    m_card[3].color_val = c_val / 13;
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[4].card_val = c_val % 13;
    m_card[4].color_val = c_val / 13;
    
#if 0    
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "%s\n", buffer);
    fprintf(input, "the flop_msg is : %d %d %d %d %d %d\n", m_card[2].card_val, m_card[2].color_val, m_card[3].card_val, m_card[3].color_val, m_card[4].card_val, m_card[4].color_val);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
}
//与hold_response一致
void turn_response(char *buffer, int length)
{
    //发出6张牌
    poker_num = 6;
    
    int i = 0;
    uint8 c_val = 0;
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[5].card_val = c_val % 13;
    m_card[5].color_val = c_val / 13;
#if 0    
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "%s\n", buffer);
    fprintf(input, "the turn_msg is : %d %d\n", m_card[5].card_val, m_card[5].color_val);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
}
//与hold_response一致
void river_response(char *buffer, int length)
{
    //发出7张牌
    poker_num = 7;

    int i = 0;
    uint8 c_val = 0;
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
    i = storeCard((uint8 *)buffer, i, c_val);
    m_card[6].card_val = c_val % 13;
    m_card[6].color_val = c_val / 13;
#if 0    
    FILE *input;
    input = fopen(logname, "a+");
    fprintf(input, "%s\n", buffer);
    fprintf(input, "the river_msg is : %d %d\n", m_card[6].card_val, m_card[6].color_val);
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
}

void pot_win_response(char *buffer, int length)
{
    
}

void charToInt(const char *p, int &index, int &val)
{
    if(p == NULL)
    {
        return ;
    }
    val = 0;
    while(p[index] != ' ')
    {
        val = val*10 + (p[index] - '0');
        ++index;
    }
    ++index;
    return;
}
// mender 入参加了const
bool isTheSame(const char *source, const char *dst, const int slen, const int dlen)
{
    if(slen != dlen && slen == 0)
    {
        return false;
    }
    if(source == NULL || dst == NULL)
    {
        return false;
    }
    for(int i=0; i<slen; ++i)
    {
        if(source[i] != dst[i])
        {
            return false;
        }
    }
    return true;
}

// 收到inquire-msg后更新m_money信息
bool my_money_update(const char *buffer, const int length) {
  if (buffer == NULL) {
    return false;
  }
  const char *p = buffer;
  // flag==1表示还没找到自己那行
  int flag = 1;
  int i = 0;
  while (flag) {
    // 跳过一行
    while (*p != '\n') {
      ++p;
    }
    // 指向下一行首
    ++p;
    i = 0;
    while (p[i] != ' ') {
      ++i;
    }
    // 判断是否是自己的行
    if (*p == 't' || isTheSame(p, m_playid.idbuff, i, (int)m_playid.id_length)) {
      // 如果找到了自己那行，或者已经到最后一行了，退出循环
      flag = 0;
    }
    // 如果还要继续找，跳过当前行
  }
  if (*p == 't') {
    ++i;
    charToInt(p, i, m_money.jetton_val);
    charToInt(p, i, m_money.money_val);
    charToInt(p, i, m_money.bet);
  }
  else {
    m_money.bet = 0;
  }
  // 更新底池信息
  while (*p != '/' && *p != ':') {
      ++p;
  }
  if (*p == '/') {
    return false;
  }
  p = p + 2;
  i = 0;
  charToInt(p, i, m_money.pot_val);
  return true;
}

//五张牌的处理
void poker5_handle(const int &last_action, const int &last_val, int &cur_action, int &cur_val)
{
  if(0 == max_value(poker_num)) {
    poker5_level0(last_action, last_val, cur_action, cur_val);
    return;
  }
    if(max_value(poker_num) <= 166)
    {
        poker5_level1(last_action, last_val, cur_action, cur_val);
        return;
    }
    if(max_value(poker_num) <= 1599)
    {
        poker5_level2(last_action, last_val, cur_action, cur_val);
        return;
    }
    if(max_value(poker_num) <= 2467)
    {
        poker5_level3(last_action, last_val, cur_action, cur_val);
        return;
    }
    if(max_value(poker_num) <= 3325)
    {
        poker5_level4(last_action, last_val, cur_action, cur_val);
        return;
    }
    poker5_level0(last_action, last_val, cur_action, cur_val);
}

void poker5_level0(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // 异常情况，或者牌太小
  // 可以让牌就让牌，否则弃牌
  if ( m_money.bet >= last_val) {
    cur_action = CHECK;
  }
  else {
    cur_action = FOLD;
  }
}

void poker5_level1(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // 好牌，无上限加注
  // 筹码不够了，ALL-IN
  int gap = m_money.bet - last_val;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  // 加注随机金额，无上限
  srand((unsigned)time(NULL));
  int raise_val = (rand() % (m_money.jetton_val-gap)) + gap;
  // 加注
  cur_action = RAISE;
  cur_val = raise_val;
}

void poker5_level2(const int &last_action, const int &last_val, int &cur_action, int &cur_val) { 
  // 好牌，无上限加注
  // 筹码不够了，ALL-IN
  int gap = last_val - m_money.bet;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  // 加注随机金额，上限为当前筹码的1/2
  srand((unsigned)time(NULL));
  int raise_val = rand() % ((m_money.jetton_val - gap) / 2) + gap;
  // 加注
  cur_action = RAISE;
  cur_val = raise_val;
}

void poker5_level3(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // 中等牌，当玩家多余4个的时候按小牌处理
  if (player_num > 4) {
    poker5_level0(last_action, last_val, cur_action, cur_val);
    return;
  }
  // 筹码不够了，ALL-IN
  int gap = last_val - m_money.bet;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  // 加注随机金额，上限为当前筹码的1/4
  srand((unsigned)time(NULL));
  int raise_val = rand() % ((m_money.jetton_val - gap) / 4) + gap;
  cur_action = RAISE;
  cur_val = raise_val;
}

void poker5_level4(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // 中等牌，当玩家大于3个的时候。按小牌处理
  if (player_num > 3) {
    poker5_level0(last_action, last_val, cur_action, cur_val);
    return;
  }
  // 筹码不够了，ALL-IN
  int gap = last_val - m_money.bet;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  cur_action = CALL;
}

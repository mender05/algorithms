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

//�����ⲿ���ñ���
extern CARD m_card[7];
extern MONEY m_money;
//�Լ���ID���
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
    //��buffer�����ж�
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
                //��seat������д���
                seat_response(buffer, length);
            }
            else
            {
                //��showdown������д���
                showdown_response(buffer, length);
            }
            break;
        }
        case 'g':
        {
            //��game_over������д���
            game_over_response(buffer, length);
            return -1;
            break;
        }
        case 'b':
        {
            //��blind������д���
            blind_respose(buffer, length);
            break;
        }
        case 'h':
        {
            //��hold������д���
            hold_response(buffer, length);
            break;
        }
        case 'i':
        {
            //��inquire������д���
            if(~is_all_in)
                inquire_response(buffer, length);
            break;
        }
        case 'f':
        {
            
            //��flop������д���
            flop_response(buffer, length);
            break;
        }
        case 't':
        {
            //��turn������д���
            turn_response(buffer, length);
            break;
        }
        case 'r':
        {
            //��river������д���
            river_response(buffer, length);
            break;
        }
        case 'p':
        {
            //��pot_win������д���
            pot_win_response(buffer, length);
            break;
        }
        default :
            break;
    }
    return 0;
}
//�Խ��յ�����ֵ���м��㣬��c_val���ظ�ֵ
//c_val = (0-3)*13[��ɫ] + (0-12)[��ֵ];
//SPADES��HEARTS��CLUBS��DIAMONDS��common��ö����
int storeCard(uint8 *buffer, uint8 start, uint8 &c_val)
{
    int i = start;
    c_val = 0;
    //���ڻ�ɫ���ַ�����ȷ����ֱ�Ӽ��Ͼ���ֵ����
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
    //����A����Ϊ0��������΢�޸ļ���
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
//log��seat��Ϣ
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
//seat����Ĵ���
void seat_response(char *buffer, int length)
{
    //��ʼ��ȫ�ֱ���
    poker_num = 0;

    for(int i=0;i<7;++i)
    {
        m_card[i].color_val = 0;
        m_card[i].card_val =0;
    }
    
    memset(_MAX_VALUE_, 0, sizeof(int)*3);
    is_all_in = false;
    int i = 0;
    //����seat/ eol
    while(buffer[i]!='\n' && i<length)
    {
        ++i;
    }
    if(i<length+1)
    {
        ++i;
    }
    //ͳ������
    int j = i;
    player_num = 0;
    while(buffer[j] != '/' && j < length)
    {
        if(buffer[j] == '\n')
            ++player_num;
        ++j;
    }
    //���raise_num��ע�����ֵ
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

    //�жϲ���/seat eol���򲻶ϵ���store_m_val���������洢���Լ���
    //����ʱ�Ƴ�
    while(buffer[i] != '/' && i<length)
    {
        //�����������͵��±꣬���±��������
        if(store_m_val(buffer, i) == 0)
        {

            break;
        }
    }
}
//�洢���ӵ�ǰ�ĳ���ֵ
//�����±����ã������ж������±��ֵ
int store_m_val(char *buffer, int &i)
{
    int j = i;
    //����button/big blind
    if(buffer[i] == 'b')
    {
        //����һ���ַ�
        ++i;
        if(buffer[i] == 'u')
        {
            //button������7���ַ���ָ��PID
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
            //big blind������10���ַ���ָ��PID
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
            //small blind������13���ַ���ָ��PID
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
    //��ȡpid�ĳ��ȡ��±�i������
    //PID����������Ŀո�' '
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

    //�Ƚ��Ƿ�IDһ��
    //m_playid�ṹ����game.cpp�г�ʼ��Ϊ����5
    //pidΪ��λ��Ϣ�е�pid��pidlenΪ�������ո�' '��pid����
    if(isTheSame(m_playid.idbuff, &buffer[i-pidlen], (int)m_playid.id_length, pidlen))
    {
        //pid��ͬ���������ͽ��
        //++i�������Ļ��з�
        ++i;
        //��charת����int����������i����i�������󡢷��ص�iָ�������ո�' '���ַ�
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
        //���ID��һ�£���������һ�п�ʼ
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
//log�е�hold��Ϣ
///hold 
//player 8888:hold/����Ӧ��û�е� 
//HEARTS 9 
//DIAMONDS 6 
///hold

void hold_response(char *buffer, int length)
{
    //����2����
    poker_num = 2;

    int i = 0;
    uint8 c_val = 0;
    //�ܹ�5�У�������1��
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
    //�����1���ƣ��������ã�����ָ����1�е����ַ����±�
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
    //������1��
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
    //��ȡ��һ����עֵ�Ĵ�С�붯��
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
        //����
        sendCmd(FOLD, 0);
        return;
    }
    while(buffer[i] != '/' && i<length)
    {
        //�����±�����i����pot��Ϣ���д���i���Ŵ�������
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
    //���������˵�ǰ�׳ء���ǰ�Լ��ĳ���ֵ����ǰ���͵����ֵ
    //��һ�����������۶��
    int cur_action = 0;
    // mender++ ���һ����������ʾ��ע���
    int cur_val = 0;
    // ++mender
    int card2_level = 0;
//    int poker_max_val = 0;
    if(poker_num == 2)
    {
        //2���ƵĴ���
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
        ///5��6��7���ƵĴ���;
        // mender++
        poker5_handle(last_action, last_val, cur_action, cur_val);
        // ++mender
    }
    else
    {
        //������˵��û���յ�hold��������������
        cur_action = FOLD;
        cur_val = 0;
    }
    //�Ȼ���call
//    cur_action = CALL;
    sendCmd(cur_action, cur_val);
}
//�����ƵĴ���
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

//��������
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

//��ȡ��һ����עֵ�Ĵ�С�붯��
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
    //����last_val��ֵ
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
    //������ǰ��
    while(buffer[i] != '\n')
    {
        ++i;
    }
    ++i;
}

//��صĴ������Ը�����������Ծ�����
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
//������ã�������pot����������2���Ƶ��жϣ�����ֱ�Ӹ�
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
//����ֵ�ڶ���pot����jetton��1/2
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
//����ֵ������pot����jetton��1/4�����۲�����jetton��1/4
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
//����ֵ���Ĵ󣬴��۲�����200
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

//����inquire�׳���Ϣ
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
    //��ȡ�׸��ո�֮����ַ�����
    int pot_len = 0;
    while(src[index] != ' ')
    {
        ++index;
        ++pot_len;
    }
    //�ж���"total"�Ƿ�һ��
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
    fprintf(input, "in isTheSame�� index = %d, src[%d] = %c\n", index, index, *(src+index));
    if(fclose(input) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
#endif
        //����pot��ֵ
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
//��hold_responseһ��
void flop_response(char *buffer, int length)
{
    //����3�Ź�����
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
//��hold_responseһ��
void turn_response(char *buffer, int length)
{
    //����6����
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
//��hold_responseһ��
void river_response(char *buffer, int length)
{
    //����7����
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
// mender ��μ���const
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

// �յ�inquire-msg�����m_money��Ϣ
bool my_money_update(const char *buffer, const int length) {
  if (buffer == NULL) {
    return false;
  }
  const char *p = buffer;
  // flag==1��ʾ��û�ҵ��Լ�����
  int flag = 1;
  int i = 0;
  while (flag) {
    // ����һ��
    while (*p != '\n') {
      ++p;
    }
    // ָ����һ����
    ++p;
    i = 0;
    while (p[i] != ' ') {
      ++i;
    }
    // �ж��Ƿ����Լ�����
    if (*p == 't' || isTheSame(p, m_playid.idbuff, i, (int)m_playid.id_length)) {
      // ����ҵ����Լ����У������Ѿ������һ���ˣ��˳�ѭ��
      flag = 0;
    }
    // �����Ҫ�����ң�������ǰ��
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
  // ���µ׳���Ϣ
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

//�����ƵĴ���
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
  // �쳣�����������̫С
  // �������ƾ����ƣ���������
  if ( m_money.bet >= last_val) {
    cur_action = CHECK;
  }
  else {
    cur_action = FOLD;
  }
}

void poker5_level1(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // ���ƣ������޼�ע
  // ���벻���ˣ�ALL-IN
  int gap = m_money.bet - last_val;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  // ��ע�����������
  srand((unsigned)time(NULL));
  int raise_val = (rand() % (m_money.jetton_val-gap)) + gap;
  // ��ע
  cur_action = RAISE;
  cur_val = raise_val;
}

void poker5_level2(const int &last_action, const int &last_val, int &cur_action, int &cur_val) { 
  // ���ƣ������޼�ע
  // ���벻���ˣ�ALL-IN
  int gap = last_val - m_money.bet;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  // ��ע���������Ϊ��ǰ�����1/2
  srand((unsigned)time(NULL));
  int raise_val = rand() % ((m_money.jetton_val - gap) / 2) + gap;
  // ��ע
  cur_action = RAISE;
  cur_val = raise_val;
}

void poker5_level3(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // �е��ƣ�����Ҷ���4����ʱ��С�ƴ���
  if (player_num > 4) {
    poker5_level0(last_action, last_val, cur_action, cur_val);
    return;
  }
  // ���벻���ˣ�ALL-IN
  int gap = last_val - m_money.bet;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  // ��ע���������Ϊ��ǰ�����1/4
  srand((unsigned)time(NULL));
  int raise_val = rand() % ((m_money.jetton_val - gap) / 4) + gap;
  cur_action = RAISE;
  cur_val = raise_val;
}

void poker5_level4(const int &last_action, const int &last_val, int &cur_action, int &cur_val) {
  // �е��ƣ�����Ҵ���3����ʱ�򡣰�С�ƴ���
  if (player_num > 3) {
    poker5_level0(last_action, last_val, cur_action, cur_val);
    return;
  }
  // ���벻���ˣ�ALL-IN
  int gap = last_val - m_money.bet;
  if (gap >= m_money.jetton_val) {
    cur_action = ALL_IN;
    return;
  }
  cur_action = CALL;
}

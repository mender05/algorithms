#ifndef HANDLE_H_
#define HANDLE_H_

int handle_recv_msg(char *buffer, int length, char *uid);
int storeCard(uint8 *buffer, uint8 start, uint8 &c_val);

void seat_response(char *buffer, int length);

void showdown_response(char *buffer, int length);

void game_over_response(char *buffer, int length);

void blind_respose(char *buffer, int length);

void hold_response(char *buffer, int length);

void inquire_response(char *buffer, int length);

void flop_response(char *buffer, int length);

void turn_response(char *buffer, int length);

void river_response(char *buffer, int length);

void pot_win_response(char *buffer, int length);

int store_m_val(char *buffer, int &i);
// mender++ ????¨¢???o¡¥¨ºy?¨®¨¢?const
void charToInt(const char *p, int &index, int &val);

bool isTheSame(const char *source, const char *dst, const int slen, const int dlen);
// ++mender
void level0(const int &last_action, const int &last_val, int &cur_action);

void level1(const int &last_action, const int &last_val, int &cur_action);

void level2(const int &last_action, const int &last_val, int &cur_action);

void level3(const int &last_action, const int &last_val, int &cur_action);

void level4(const int &last_action, const int &last_val, int &cur_action);

void sendCmd(const int &action, const int &cur_val);

void poker2_handle(int &card2_level);

int store_m_pot(char *src, int &index);

void last_handle(char *buffer, int &i, int &last_action, int &last_val);
// ¨®?inquire-msg?¨¹D?m_money
bool my_money_update(const char *buffer, const int length);
// ??????¦Ì???2?o¡¥¨ºy
void poker5_handle(const int &last_action, const int &last_val, int &cur_action, int &cur_val);
// D????¨°¨°¨¬3¡ê
void poker5_level0(const int &last_action, const int &last_val, int &cur_action, int &cur_val);
// ¨ª??¡§?3¡ê???¨¬?
void poker5_level1(const int &last_action, const int &last_val, int &cur_action, int &cur_val);
// o¨´??¡ê?¨ª??¡§
void poker5_level2(const int &last_action, const int &last_val, int &cur_action, int &cur_val);
// ?3¡Á¨®¡ê?¨¨y¨¬?
void poker5_level3(const int &last_action, const int &last_val, int &cur_action, int &cur_val);
// ¨¢???
void poker5_level4(const int &last_action, const int &last_val, int &cur_action, int &cur_val);

#endif
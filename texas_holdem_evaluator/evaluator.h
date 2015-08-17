#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "kev_poker.h"

// 5、6、7张牌的估值函数
// 输入：待估值牌的张数k
// 输出：计算m_card[k]中，5张牌组成的最大牌型的数值，并更新MAX_CARDS_VALUE[]
int max_value (const int card_num);

// 五张牌的估值函数
// 辅助函数
int evaluator (const unsigned int c[5]);

// 二分搜索
// 辅助函数
int binary_search (const int k);

// 将牌表示为kev_poker的格式
// 辅助函数
inline int card2int (const unsigned int color, const unsigned int point);

#endif

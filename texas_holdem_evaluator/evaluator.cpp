#include "evaluator.h"
#include "kev_poker.h"
#include "kev_array.h"
#include "common.h"

// global var
// store the max value of 5/6/7 cards. 
// the var should reset to 0 when game begin or over
int _MAX_VALUE_[3];

// 全局常量
// 保存CARD到kev_poker.h定义的数据的映射
extern const unsigned int POKER_COLOR[4] = {SPADE, HEART, CLUB, DIAMOND};
extern const unsigned int POKER_POINT[13] = {Ace, Deuce, Trey, Four, Five,
  Six, Seven, Eight, Nine, Ten, Jack, Queen, King};

// 全局常量，保存5、6、7张牌的组合数
// [0]是5张牌的组合；[0,5]是6张牌的组合；[0,21]是7张牌的组合
const int COMBINATION[21][5] = {
  {0, 1, 2, 3, 4},
  {0, 1, 2, 3, 5},
  {0, 1, 2, 4, 5},
  {0, 1, 3, 4, 5},
  {0, 2, 3, 4, 5},
  {1, 2, 3, 4, 5}, 
  {0, 1, 2, 3, 6},
  {0, 1, 2, 4, 6},
  {0, 1, 3, 4, 6},
  {0, 2, 3, 4, 6},
  {1, 2, 3, 4, 6},
  {0, 1, 2, 5, 6},
  {0, 1, 3, 5, 6},
  {0, 2, 3, 5, 6},
  {1, 2, 3, 5, 6},
  {0, 1, 4, 5, 6},
  {0, 2, 4, 5, 6},
  {1, 2, 4, 5, 6},
  {0, 3, 4, 5, 6},
  {1, 3, 4, 5, 6},
  {2, 3, 4, 5, 6}
};

// defined in game.cpp
extern CARD m_card[7];

inline int card2int (const unsigned int color, const unsigned int point) {
  // convert a card to an int
  // in: color = CLUB, DIAMOND, HEART, SPADE
  // in: point = Deuce, Trey, four, ... , King, Ace
  // out: 0x 000AKQJT,98765432,CDHSrrrr,00pppppp
  return (0x00010000 << point) | color | (point << 8) | primes[point];
}

int binary_search (const int k) {
  // binary search in products[]
  int beg = 0, end = SIZE_products;
  int mid = end / 2;
  while (mid != end && products[mid] != k) {
    if (products[mid] > k) {
      end = mid;
    } 
    else {
      beg = mid + 1;
    }
    mid = beg + (end - beg) / 2;
  }
  if (mid != end) {
    return values[mid];
  } 
  else {
    return 0;
  }
}

int evaluator (const unsigned int c[5]) {
  // return the value of the suit; if error, return 0 
  // user should guarantee the input is an int array with five elements
  // c[i] = 000AKQJT,98765432,CDHSrrrr,00pppppp
  unsigned int q = (c[0] | c[1] | c[2] | c[3] | c[4]) >> 16;
  if (q >= SIZE_flushes || q >= SIZE_unique5) {
    return 0;
  }
  if (c[0] & c[1] & c[2] & c[3] & c[4] & 0x0000F000) {
    /* flush or straight flush */
    return flushes[q];
  } 
  else if(unique5[q]) {
    // straight or high cards
    return unique5[q];
  } else {
    // others, binary search
    int k = (c[0] & 0x000000FF) * (c[1] & 0x000000FF) * \
        (c[2] & 0x000000FF) * (c[3] & 0x000000FF) * (c[4] & 0x000000FF);
    return binary_search(k);
  }
}

int max_value (const int card_num) {
  //
  if (card_num < 5 || card_num > 7) {
    return 0;
  }
 
  // card_num = 5, 6, 7
  unsigned int c[5];
  int beg;
  int end;
  int index;
  if (5 == card_num) {
    if (0 != _MAX_VALUE_[0]) {
      // 如果已经计算过5张牌的最大值，直接返回
      return _MAX_VALUE_[0];
    }
    beg = 0;
    end = 1;
    index = 0;
  } 
  else if (6 == card_num) {
    if (0 != _MAX_VALUE_[1]) {
      // 如果已经计算过6张牌的最大值，直接返回
      return _MAX_VALUE_[1];
    }
    beg = 1;
    end = 6;
    index = 1;
    if (0 == _MAX_VALUE_[0]) {
      // 如果没有计算过card_num == 5的最大值，则先计算之，并把这个值作为6张牌的初值
      _MAX_VALUE_[1] = max_value (5);
    }
    _MAX_VALUE_[1] = _MAX_VALUE_[0];
  } 
  else {
    if (0 != _MAX_VALUE_[2]) {
      //如果已经计算过7张牌的最大值，直接返回
    }
    beg = 6;
    end = 21;
    index = 2;
    if (0 == _MAX_VALUE_[1]) {
      // 如果没有计算过card_num == 6的最大值，则先计算之，并把这个值作为7张牌的初值
      _MAX_VALUE_[2] = max_value (6);
    }
    _MAX_VALUE_[2] = _MAX_VALUE_[1];
  }
  int val = 0;
  for (int i = beg; i < end; ++i) {
    for (int j = 0; j < 5; ++j) {
      c[j] = card2int(POKER_COLOR[m_card[COMBINATION[i][j]].color_val], 
          POKER_POINT[m_card[COMBINATION[i][j]].card_val]);
    }
    val = evaluator(c);
    if (val < _MAX_VALUE_[index] || 0 == _MAX_VALUE_[index]) {
      _MAX_VALUE_[index] = val;
    }
  }
  return _MAX_VALUE_[index];
}

// test code
/*
#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

int main () {
  unsigned int c[5];
  
  // 98765 straight flushes, value = 6
  c[0] = card2int(CLUB, Nine);
  assert(c[0] == 0x00808713);
  c[1] = card2int(CLUB, Eight);
  assert(c[1] == 0x00408611);
  c[2] = card2int(CLUB, Seven);
  assert(c[2] == 0x0020850D);
  c[3] = card2int(CLUB, Six);
  assert(c[3] == 0x0010840B);
  c[4] = card2int(CLUB, Five);
  assert(c[4] == 0x00088307);
  assert(evaluator(c) == 6);
  
  // AAAAQ, value = 12
  c[0] = card2int(CLUB, Ace);
  assert(c[0] == 0x10008C29);
  c[1] = card2int(DIAMOND, Ace);
  c[2] = card2int(HEART, Ace);
  c[3] = card2int(SPADE, Ace);
  c[4] = card2int(SPADE, Queen);
  assert(evaluator(c) == 12);
  
  // AAAQQ, value = 168
  c[0] = card2int(CLUB, Ace);
  c[1] = card2int(DIAMOND, Ace);
  c[2] = card2int(HEART, Ace);
  c[3] = card2int(CLUB, Queen);
  c[4] = card2int(SPADE, Queen);
  assert(evaluator(c) == 168);

  // 76432 flushes, value = 1598
  c[0] = card2int(CLUB, Seven);
  c[1] = card2int(CLUB, Six);
  c[2] = card2int(CLUB, Four);
  c[3] = card2int(CLUB, Trey);
  c[4] = card2int(CLUB, Deuce);
  assert(evaluator(c) == 1598);

  // KQJT9 straight, value = 1601
  c[0] = card2int(CLUB, King);
  c[1] = card2int(DIAMOND, Queen);
  c[2] = card2int(HEART, Jack);
  c[3] = card2int(SPADE, Ten);
  c[4] = card2int(SPADE, Nine);
  assert(evaluator(c) == 1601);

  // AAAKJ, value = 1611
  c[0] = card2int(CLUB, Ace);
  c[1] = card2int(DIAMOND, Ace);
  c[2] = card2int(HEART, Ace);
  c[3] = card2int(SPADE, King);
  c[4] = card2int(SPADE, Jack);
  assert(evaluator(c) == 1611);

  // 33224, value = 3325
  c[0] = card2int(CLUB, Trey);
  c[1] = card2int(DIAMOND, Trey);
  c[2] = card2int(HEART, Deuce);
  c[3] = card2int(SPADE, Deuce);
  c[4] = card2int(SPADE, Four);
  assert(evaluator(c) == 3325);

  // 22643, value = 6184
  c[0] = card2int(CLUB, Deuce);
  c[1] = card2int(DIAMOND, Deuce);
  c[2] = card2int(HEART, Six);
  c[3] = card2int(SPADE, Four);
  c[4] = card2int(SPADE, Trey);
  assert(evaluator(c) == 6184);

  // AKQJ9, value = 6186
  c[0] = card2int(CLUB, Ace);
  c[1] = card2int(DIAMOND, King);
  c[2] = card2int(HEART, Queen);
  c[3] = card2int(SPADE, Jack);
  c[4] = card2int(SPADE, Nine);
  assert(evaluator(c) == 6186);

  return 0;
}
*/

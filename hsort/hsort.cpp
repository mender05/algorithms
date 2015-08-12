#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <utility>
#include <functional>

using std::cout;
using std::default_random_engine;
using std::uniform_int_distribution;
using std::swap;

/* Hsort��һ��ʵ�ֶ������function object */
template<typename T>
class Hsort {
public:
    typedef std::function<bool(T, T)> FuncType;
    /* call operator ������������ *
     * seq�Ǵ���������ָ�룬n�����鳤�ȣ�comp�����ڱȽϵ�callable object */
    void operator()(T *seq, const int n, FuncType comp = std::less<T>()) {
        if (nullptr == seq || n <= 0) {
            return;
        }
        compare = comp;
        heap_construct(seq, n);
        heap_sort(seq, n);
    }
private:
    FuncType compare;
    /* ����һ���� */
    void heap_construct(T *seq, const int n) {
        for (int i = parent(n - 1); i >= 0; --i) {
            heap_adjust(seq, i, n);
        }
    }
    
    /* ������ */
    void heap_sort(T *seq, const int n) {
        int m = n - 1;
        while (m > 0) {
            swap(seq[0], seq[m]);
            heap_adjust(seq, 0, m);
            --m;
        }
    }
    
    /* ������seq�У���begΪ���㣬endΪ�����Ķ� */
    void heap_adjust(T *seq, const int beg, const int end) {
        if (parent(end - 1) < beg) {
            return;
        }
        int i = beg;
        while (i <= parent(end - 1)) {
            int j = i;
            if (left(i) < end && right(i) < end) {
                j = compare(seq[left(i)], seq[right(i)]) ? right(i) : left(i);
                j = compare(seq[i], seq[j]) ? j : i;
            }
            else if (left(i) < end && compare(seq[i], seq[left(i)])) {
                j = left(i);
            }
            else if (right(i) < end && compare(seq[i], seq[right(i)])) {
                j = right(i);
            }
            if (i != j) {
                swap(seq[i], seq[j]);
                i = j;
            }
            else {
                i = end;
            }
        } /* while */
    } /* heap_adjust */

    static int parent(int i) {
        return static_cast<int>((i - 1) * 0.5);
    }
    static int left(int i) {
        return (i + 1) * 2 - 1;
    }
    static int right(int i) {
        return (i + 1) * 2;
    }
};


int main()
{
    int n = 10;
    /* ��ӡ������ */
    for (int i = 0; i < n; ++i) {
        cout << std::left;
        cout << std::setw(2) << i << " ";
    }
    cout << "\n--------------------------------\n";

    /* ��������� */
    int *p = new int[n];
    default_random_engine generator;
    uniform_int_distribution<int> distribution(10, 99);
    for (int i = 0; i < n; ++i) {
        p[i] = distribution(generator);
        cout << p[i] << " ";
    }
    cout << "\n";

    /* ���������� */
    Hsort<int> hp;
    hp(p, n);
    for (int i = 0; i < n; ++i) {
        cout << p[i] << " ";
    }
    cout << "\n";

    /* ���������� */
    hp(p, n, [](int i, int j) -> bool { return i < j ? false : true; });
    for (int i = 0; i < n; ++i) {
        cout << p[i] << " ";
    }
    cout << "\n";

    return 0;
}

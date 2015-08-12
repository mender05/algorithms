// rb_tree.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "rb_tree.h"
#include <iostream>
#include <iterator>
int _tmain(int argc, _TCHAR* argv[])
{
	
	Rb_tree<int> t;
	//int a[] = {8, 13, 1, 27, 11, 6, 15, 25, 22, 17}; 
	int a[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
	for (auto ita = std::begin(a); ita != std::end(a); ++ita) {
		std::cout << *ita << " ";
		t.insert_node(*ita);
	}
	std::cout<< "\n<<<<" <<std::endl;
	t.Bfs();
	std::cout<< "\n<<<<" <<std::endl;
	t.delete_node(8);
	t.Bfs();
	std::cout<< "\n<<<<" <<std::endl;
	std::cin >> a[0];
	return 0;
}


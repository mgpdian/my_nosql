#include <iostream>
#include "my_skiplist_try.hpp"
#define FILE_PATH "./save/RDBFile"

int main() {

    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    my_zskiplist<int, std::string> skipList;
	skipList.insert(1, "q"); 
	skipList.insert(2, "w"); 
	skipList.insert(3, "e"); 
    skipList.insert(4, "r"); 
	skipList.insert(5, "t"); 
	skipList.insert(7, "y"); 
	skipList.insert(6, "u"); 

    std::cout << "skipList size:" << skipList.get_list_size() << std::endl;

    skipList.rdb_save();

    //skipList.rdb_load();

    skipList.search(2);
    skipList.search(18);


    skipList.ergodic_skiplist();

    skipList.Delete(3);
    skipList.Delete(7);

    std::cout << "skipList size:" << skipList.get_list_size() << std::endl;

    skipList.ergodic_skiplist();
}
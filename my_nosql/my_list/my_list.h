#ifndef MY_LIST_H
#define MY_LIST_H

#include <iostream>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <mutex>
#include <fstream>
//#include "../my_robj/my_robj.h"
#include <vector>
static const std::string  RDB_FILE_LIST = "../save/RDBFile_list";
//列表(list)

class my_list
{
public:
    //分配器
    bool parser_and_distributor(const std::string& str);
    //返回元素个数
    // strlen
    int get_size();
    my_list():size(0){}
    ~my_list(){}

    //解决RDB载入后 robj的记录无法登记类型的问题
    std::vector<std::string> robj_register();
private:

    //添加
    //lpush/rpush key value value1 value2
    bool insert_list(const int choice, const std::string &key, const std::string &value); //字符串对象

    //删除
    // del key
    bool delete_list(const std::string &key); //因为字符串 所以不用细分 实际上删除有俩种Key value pair
    //删除
    //lpop/rpop key
    bool delete_list(const int choice, const std::string &key);


    //查找 只能输入下标 
    //lindex key index (从左到右)
    std::string get_list(const std::string &key, const std::string& index);

    //返回元素个数
    // strlen
    //int get_size();

    //返回键中的元素个数
    //llen key
    int get_size(const std::string &key);

    //遍历
    //lrange list
    bool ergodic_list(const std::string& key);


    //遍历string的所有键值对
    bool ergodic_all();

    //清空
    // clear
    bool clear_list();

    //还有一个写入文件
    bool rdb_save();

    //和一个读出文件
    //但是好像没啥用 在这里读入的话 主函数那里记录不了
    bool rdb_load();

    //将文件传入的数据转换成key和value
    void get_key_value_from_string(const std::string &str, std::string &key, std::string &value);
    //判断文件传入的数据是否符合要求
    bool is_valid_string(const std::string &str);

    //分析字符串指令 动作 key value
    void get_action_key_value_from_string(const std::string& str, std::string &action, std::string &key, std::string &value);

    
private:
    //元素个数
    int size;

    std::unordered_map<std::string, std::list<std::string>> my_list_map;

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;

    //锁
    std::mutex mtx3;
    //友元
    //friend class my_robj;
};

#endif
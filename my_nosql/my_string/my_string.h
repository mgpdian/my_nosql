#ifndef MY_STRING_H
#define MY_STRING_H

#include <iostream>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <mutex>
#include <fstream>
//#include "../my_robj/my_robj.h"
static const std::string RDB_FILE_STRING = "../save/RDBFile_string";
//字符串(string)

//

class my_string
{
public:
    //分配器
    bool parser_and_distributor(const std::string& str);
    //返回元素个数
    // strlen
    int get_size();
    my_string():size(0){}
    ~my_string(){}


    //解决RDB载入后 robj的记录无法登记类型的问题
    std::vector<std::string> robj_register();
private:
    //添加
    // set key value
    bool insert_string(const std::string &key, const std::string &value); //字符串对象

    //删除
    // del key
    bool delete_string(const std::string &key); //因为字符串 所以不用细分 实际上删除有俩种Key value pair

    //查找
    // get key
    std::string get_string(const std::string &key);

    //返回元素个数
    // strlen
    //int get_size();

    //遍历string的所有键值对
    bool ergodic_all();

    //清空
    // clear
    bool clear_string();

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

    std::unordered_map<std::string, std::string> my_string_map;

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;

    //锁
    std::mutex mtx5;

    //友元
    //friend class my_robj;
};

#endif
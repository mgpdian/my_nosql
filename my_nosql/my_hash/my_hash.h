#ifndef MY_HASH_H
#define MY_HASH_H

#include <iostream>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <mutex>
#include <fstream>
#include <vector>
static const std::string  RDB_FILE_HASH = "../save/RDBFile_hash";
//#include "../my_robj/my_robj.h"
//哈希(hash)





class my_hash
{
public:
    //分配器
    bool parser_and_distributor(const std::string& str);
    //返回元素个数
    // strlen
    int get_size();

    my_hash():size(0){}
    ~my_hash(){}

    //解决RDB载入后 robj的记录无法登记类型的问题
    std::vector<std::string> robj_register();
private:


    //添加
    // hset <key> <field> <value>
    //insert_hash
    bool insert_hash(const std::string &key, const std::string &field, const std::string &value); 

    //删除
    // del key
    
    bool delete_hash(const std::string &key); 

        // //删除
    // hdel <key> <field>
    bool delete_hash(const std::string &key, const std::string &field);



    // //查找
    // hexists<key1><field>
    std::string get_hash(const std::string &key, const std::string& field);

    //返回元素个数
    // strlen
    //int get_size();


    // //返回元素个数
    // hlen <key1>
    int get_size(const std::string &key);


    // //遍历
    // hgetall <key> //全部
    // hkeys <key>	//field
    // hvals <key> //value
    bool ergodic_hash(const int choice, const std::string& key);
    

    //遍历string的所有键值对
    bool ergodic_all();

    //清空
    // clear
    bool clear_hash();

    //还有一个写入文件
    bool rdb_save();

    //和一个读出文件
    //但是好像没啥用 在这里读入的话 主函数那里记录不了
    bool rdb_load();

    //将文件传入的数据转换成key和value
    void get_key_field_value_from_string(const std::string &str, std::string &key, std::string &field, std::string &value);
    //判断文件传入的数据是否符合要求
    bool is_valid_string(const std::string &str);

    //分析字符串指令 动作 key value
    void get_action_key_value_from_string(const std::string& str, std::string &action, std::string &key, std::string &field, std::string &value);

    
    
private:
    //元素个数
    int size;

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> my_hash_map;

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;

    //锁
    std::mutex mtx4;

    //友元
    //friend class my_robj;
};

#endif
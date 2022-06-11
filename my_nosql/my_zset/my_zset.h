#ifndef MY_ZSET_H
#define MY_ZSET_H

#include <iostream>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <mutex>
#include <fstream>
#include "../my_skiplist/my_zskiplist.h"
#include <vector>
//#include "../my_robj/my_robj.h"
static const std::string  RDB_FILE_ZSET = "../save/RDBFile_zset";
//有序集合(zset)
// //添加
// zadd key score value
// //删除
// zrem key value
// //查找
// zscore key value
// //返回元素个数
// zcard key
// //返回排序 (正反) 遍历
// zrange key 


class my_zset
{
public:
    //分配器
    bool parser_and_distributor(const std::string& str);
    //返回元素个数
    // strlen
    int get_size();

    my_zset():size(0){}
    ~my_zset(){}

     //解决RDB载入后 robj的记录无法登记类型的问题
    std::vector<std::string> robj_register();
private:


    //添加
    // zadd key score value
    bool insert_zset(const std::string &key, const double &score, const std::string &value); 

    //删除
    // del key
    
    bool delete_zset(const std::string &key); 

        // //删除
    // zrem key value
    bool delete_zset(const std::string &key, const std::string &value);



    // //查找
    // zscore key value
    double get_zset(const std::string &key, const std::string& value);

    


    // //返回元素个数
    // zcard key
    int get_size(const std::string &key);


    // //遍历
    // zrange key 
    bool ergodic_zset(const std::string& key);
    

    //遍历string的所有键值对
    bool ergodic_all();

    //清空
    // clear
    bool clear_zset();

    //还有一个写入文件
    bool rdb_save();

    //和一个读出文件
    //但是好像没啥用 在这里读入的话 主函数那里记录不了
    bool rdb_load();

    //将文件传入的数据转换成key和value
    void get_key_field_value_from_string(const std::string &str, std::string &key);
    //判断文件传入的数据是否符合要求
    bool is_valid_string(const std::string &str);

    //分析字符串指令 动作 key value
    void get_action_key_value_from_string(const std::string& str, std::string &action, std::string &key, double &score, std::string &value);

   
private:
    //元素个数
    int size;

    std::unordered_map<std::string, my_zskiplist> my_zset_map;

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;

    //锁
    std::mutex mtx6;

    //友元
    //friend class my_robj;
};

#endif
#ifndef MY_ROBJ_H
#define MY_ROBJ_H
#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <mutex>
#include <fstream>
#include "../my_skiplist/my_zskiplist.h"
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../my_string/my_string.h"
#include "../my_list/my_list.h"
#include "../my_hash/my_hash.h"
#include "../my_set/my_set.h"
#include "../my_zset/my_zset.h"
static const std::string  RDB_FILE_ROBJ = "../save/RDBFile_zset";

class my_robj{
public:
   
    enum SQL_TYPE
    {
        REDIS_STRING ,//字符串对象
        REDIS_LIST,//列表对象
        REDIS_HASH,//哈希对象
        REDIS_SET,//集合对象
        REDIS_ZSET,//有序集合对象
    };

    //解析传入的字符串
    //现在的思路是 只暴露这个函数  通过这个函数来解析字符串 以此来调用其他方法 问题是解析过程有点麻烦
    void parser_and_distributor(const std::string& str);
    my_robj():size(0), my_string_size(0), my_list_size(0), my_hash_size(0), my_set_size(0), my_zset_size(0){}
    ~my_robj(){}

private:
    //字符串对象
    //列表对象
    //哈希对象
    //集合对象
    //有序集合对象
    //加入数据库 要定义5种方法
    //作为添加字符串的分配器和解析器
    //删除
    bool delete_key(const std::string& str, const std::string& key);
    

    //遍历 要6种
    bool ergodic_all();

    //keys *
    //查看当前库所有的key
    bool ergodic_key();

    //type key
    //返回值的类型  感觉会很实用
    bool type_key(const std::string& key);

    //flushdb //清空当前库
    bool clear();


    //保存键的总数 也要6种 
    //dbsize 查看当前数据库的key数量
    int get_size();
    


    //RBD持久化
    //保存
    bool rdb_save();
    //载入
    bool rdb_load();

    //解析分配给数据库的字符串
    int parse_tosql_string(const std::string &str, std::string& key);
    //分析字符串
    int parse_string(const std::string &str, std::string& key);

    // //每次执行给其他数据库的命令后检查size是否改变 如果改变就需要改变robj的对应键
    // bool change_robj(std::string& key);

    //每次进行其他数据库操作前 对robj访问 查看是否是对应的key
    bool key_type_real(std::string& key, SQL_TYPE sql_type);

    //解决RDB载入后 robj的记录无法登记类型的问题
    bool rdb_load_robj();
private:
    //保存 键以及他的类型
    std::unordered_map<std::string, my_robj::SQL_TYPE> robj;

    //REDIS_STRING,//字符串对象
    my_string my_string_object;

    //REDIS_LIST,//列表对象
    my_list my_list_object;

    //REDIS_HASH,//哈希对象
    my_hash my_hash_object;

    //REDIS_SET,//集合对象
    my_set my_set_object;

    //REDIS_ZSET,//有序集合对象
    my_zset my_zset_object;

    //保存键的总数 也要6种 
    //dbsize 查看当前数据库的key数量
    int size;//当前的总数 dbsize

    int my_string_size; //字符串对象总数

    int my_list_size;//列表对象总数

    int my_hash_size;//哈希对象总数

    int my_set_size;//集合对象总数

    int my_zset_size;//有序集合对象总数

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;
    //锁
    std::mutex mtx1;
};
#endif
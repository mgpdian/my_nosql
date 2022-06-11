#ifndef MY_ZSKIPLIST_H
#define MY_ZSKIPLIST_H



#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <mutex>
#include <fstream>
#include <unordered_map>
#include "./my_zskiplistNode.h"
#define Skiplist_MAXLEVEL 32
//设置跳表的层数

#define Skiplist_p 0.25
//决定层数的随机值

//RDB保存位置 (可能没必要 因为后面后整合一个类 里面应该有这个效果)
static const std::string  RDB_FILE = "../save/RDBFile_zset";


//跳表

class my_zskiplist{
// public:
private:
    //表头表尾指针
    my_zskiplistNode *header, *tail;
    //最大层数
    int level;
    //表长
    unsigned long length;

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;

    //锁
    std::mutex mtx;


    //设置一个哈希表来作为字典 保存键值对对应的值
    std::unordered_map<std::string, my_zskiplistNode*> dictionaries;
private:
    // void get_key_value_from_string(const std::string& str, std::string* ele, std::string* score);
    // bool is_valid_string(const std::string& str);

    //随机生成节点层数
    static int RandomLevel(){
        int level = 1;
        //0xFFFF = 65535
        //根据rand()和掩码相与得到对应的随机值(0,0xffff)来产生level
        while((rand() & 0xFFFF) < (Skiplist_p * 0xFFFF))
        {   
            level += 1;
        }
        return level < Skiplist_MAXLEVEL ? level : Skiplist_MAXLEVEL;
    }

    //负责删除节点函数
    static void DeleteHelper(my_zskiplist *zsl, my_zskiplistNode *current, my_zskiplistNode **update);

    //将str分为key和value
    void get_score_value_from_string(const std::string& str, double& score, std::string& ele);
    //判断str是否符合:格式
    bool is_valid_string(const std::string& str);

    //读取文件 得到key来分别每个key键
    bool get_key_from_string(const std::string& str, std::string& key);
public:
    //构造函数
    my_zskiplist();

    //析构
    ~my_zskiplist();

    //添加
    my_zskiplistNode* insert(double score, std::string ele);

    //删除
    int Delete(const std::string& ele, my_zskiplistNode **node = nullptr);

    //查找
    my_zskiplistNode* search(const std::string& ele);

    //遍历元素
    void ergodic_skiplist();

    //得到元素数量
    int get_list_size();

    //持久化
    //保存到本地
    bool rdb_save();
    //从本地读取
    bool rdb_load(const std::string& key);

};




#endif
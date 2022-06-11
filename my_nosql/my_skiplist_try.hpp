#ifndef MY_SKIPLIST_TRY_H
#define MY_SKIPLIST_TRY_H



#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <mutex>
#include <fstream>

#define Skiplist_MAXLEVEL 32
//设置跳表的层数

#define Skiplist_p 0.25
//决定层数的随机值

//
#define RDB_FILE "save/RDBFile"

std::mutex mtx;
const std::string delimiter = ":";
//跳表 面向的是有序集合 key - key - value

//设置跳跃表节点
template<typename K, typename V>
class my_zskiplistNode{

public:
    my_zskiplistNode(){}

    my_zskiplistNode(int level,const K k, const V v);

    ~my_zskiplistNode();
    //本来应该是简单动态字符串sds, 但还去设计 所以这里暂时用string吧!
    //想错了 还有一层
    //应该保存的robj对象 对象中保存着sds
    // std::string ele;

    // //保存值
    // double score;
    
    K get_key() const;

    V get_value() const;

    void set_value(V v);

    //回退指针
    my_zskiplistNode<K, V> *backward;

    //层
    struct my_zskiplistLevel{
        //前进指针
        my_zskiplistNode<K, V> *forward;
        //跨度
        unsigned int span;

    } *level;
private:
    K key;
    V value;

};

template<typename K, typename V>
inline
my_zskiplistNode<K, V>::my_zskiplistNode(int level, const K k, const V v) :
                 level(new my_zskiplistLevel[level]), key(k), value(v)
{
}

template<typename K, typename V>
inline
my_zskiplistNode<K, V>::~my_zskiplistNode(){
    delete[] level;
}
template<typename K, typename V>
inline
K my_zskiplistNode<K, V>::get_key() const
{
    return key;
}
template<typename K, typename V>
inline
V my_zskiplistNode<K, V>::get_value() const
{
    return value;
}
template<typename K, typename V>
inline
void my_zskiplistNode<K, V>::set_value(V value)
{
    this -> value = value;
}




//跳表
template<typename K, typename V>
class my_zskiplist{
// public:
private:
    //表头表尾指针
    my_zskiplistNode<K, V> *header, *tail;
    //最大层数
    int level;
    //表长
    unsigned long length;

    //文件操作
    std::ofstream rdbfile_save;
    std::ifstream rdbfile_load;

    //锁
    std::mutex mtx;
private:
    // void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
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
    static void DeleteHelper(my_zskiplist<K, V> *zsl, my_zskiplistNode<K, V> *current, my_zskiplistNode<K, V> **update);

    //将str分为key和value
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    //判断str是否符合:格式
    bool is_valid_string(const std::string& str);

public:
    //构造函数
    my_zskiplist();

    //析构
    ~my_zskiplist();

    //添加
    my_zskiplistNode<K, V>* insert(K key, V value);

    //删除
    int Delete(K key, my_zskiplistNode<K, V> **node = nullptr);

    //查找
    my_zskiplistNode<K, V>* search(K key);

    //遍历元素
    void ergodic_skiplist();

    //得到元素数量
    int get_list_size();

    //持久化
    //保存到本地
    void rdb_save();
    //从本地读取
    void rdb_load();

};
//实际上实现删除的函数
template<typename K, typename V>
void my_zskiplist<K, V>::DeleteHelper(my_zskiplist<K, V> *zsl, my_zskiplistNode<K, V> *current, my_zskiplistNode<K, V> **update)
{
    int i;
    //处理current前面的指针对它的指向
    for(i = 0; i < zsl -> level; i++)
    {
        if(update[i] -> level[i].forward == current)
        {
            update[i] -> level[i].forward = current -> level[i].forward;
            update[i] -> level[i].span += current -> level[i].span - 1;
        }
        else{
            update[i] -> level[i].span--;
        }
    }

    //处理指向 x的backward
    if(current -> level[0].forward)
    {
        current -> level[0].forward -> backward = current -> backward;
    }
    //没有指向x 的backward说明他是表尾
    else{
        zsl -> tail = current -> backward;
    }

    //移除x代表的最高层
    while(zsl -> level > 1 && zsl -> header -> level[zsl -> level - 1].forward == NULL)
    {
        zsl -> level--;
    }
    zsl -> length--;
    //current -> ~my_zskiplistNode();
    //delete current;
}
//构造函数
template<typename K, typename V>
my_zskiplist<K, V>::my_zskiplist(): level(1), length(0), tail(nullptr)
{
    K key;
    V value;
    //初始化跳表
    this -> header = new my_zskiplistNode<K, V>(Skiplist_MAXLEVEL, key, value);
    for(int i = 0; i < Skiplist_MAXLEVEL; i++)
    {
        this -> header -> level[i].forward = nullptr;
    }
    this -> header -> backward = nullptr;
}

//析构
template<typename K, typename V>
my_zskiplist<K, V>::~my_zskiplist()
{
    my_zskiplistNode<K, V> *node = header, *next;
    while(node)
    {
        next = node -> level[0].forward;
        delete node;
        node = next;
    }
}

//添加
template<typename K, typename V>
my_zskiplistNode<K, V>* my_zskiplist<K, V>::insert(K key, V value)
{
    mtx.lock();
    my_zskiplistNode<K, V> *update[Skiplist_MAXLEVEL], *current;
    unsigned int rank[Skiplist_MAXLEVEL];
     //rank 计算每层到达当前节点的跨度  下一层以上一层的跨度为起点 
     //最后用rank[0] - rank[i] 来得到 i层到目标节点的跨度
    int i, level;

    current = this -> header;
    //确定节点匹配的地点
    for(i = this -> level - 1; i >= 0; i--)
    {
        rank[i] = (i == (this -> level - 1)) ? 0 : rank[i + 1];
        while (current->level[i].forward && (current->level[i].forward -> get_key() < key) )
        // || (current->level[i].forward->get_key() == key && current->level[i].forward->get_value() < value))
        {
            rank[i] += current->level[i].span;
            current = current->level[i].forward;
        }
        update[i] = current;
    }

    //只能说这个跳表...我不好说  用key作为唯一值 value作为保存量 
    current = current -> level[0].forward;

    if( current != NULL && current -> get_key() == key)
    {
        std::cout << "Key: " << key << "已经存在" << std::endl;
        mtx.unlock();
        return current;
    }

    //生成随机层数
    level = RandomLevel();
    if(level > this -> level)
    {
        for(i = this -> level; i < level; i++)
        {
            rank[i] = 0;
            update[i] = this -> header;
            update[i] -> level[i].span = this -> length;
        }

        this -> level = level;

    }
    current = new my_zskiplistNode(level, key, value);

    //连接
    for(i = 0; i < level; i++)
    {
        current -> level[i].forward = update[i] -> level[i].forward;
        update[i] -> level[i].forward = current;

        current -> level[i].span = update[i] -> level[i].span - (rank[0] - rank[i]) ;
        update[i] -> level[i].span = rank[0] - rank[i] + 1;
    }

    //收尾
    //1 高于x的层数 加1 
    //2 backward设置
    //3 更新length
    for(i = level; i < this -> level; i++)
    {
        update[i] -> level[i].span++;
    }

    current -> backward = (update[0] == this -> header) ? nullptr : update[0];
    
    if(current -> level[0].forward == nullptr)
    {
        this -> tail = current;
    }
    else{
        current -> level[0].forward -> backward = current;
    }
    this -> length++;
    mtx.unlock();
    return current;
}

//删除
template<typename K, typename V>
int my_zskiplist<K, V>::Delete(K key, my_zskiplistNode<K, V> **node )
{
    mtx.lock();
    my_zskiplistNode<K, V> *update[Skiplist_MAXLEVEL], *current;
    int i;
    
    current = this -> header;
    for(i = this -> level - 1; i >= 0; i--)
    {
        while (current->level[i].forward && (current->level[i].forward -> get_key() < key) )
            // || (current -> level[i].forward -> score == score && current -> level[i].forward -> ele < ele))
            {
                current = current -> level[i].forward;
            }
            update[i] = current;
    }

    current = current -> level[0].forward;
    // if(current && current -> score == score && current -> ele == ele)
    if(current && current -> get_key() == key)
    {
        DeleteHelper(this, current, update);

        if(!node)
        {
            delete current;
        }
        else{
            *node = current;
        }
        mtx.unlock();
        return 1;
    }
    mtx.unlock();
    return 0;

}

//查找
template<typename K, typename V>
my_zskiplistNode<K, V>* my_zskiplist<K, V>::search(K key)
{
    my_zskiplistNode<K, V> *current = this -> header;
    for(int i = this -> level - 1; i >= 0; i--)
    {
        while (current->level[i].forward && (current->level[i].forward -> get_key() < key) )
           
            {
                current = current -> level[i].forward;
            }
    }
    auto val = current -> level[0].forward;
    if(val && val -> get_key() == key){
        std::cout << " 找到了key: " << key << ", 他的value为: " << current->get_value() << std::endl;
        return val;
    }
     std::cout << "Not Found Key:" << key << std::endl;
    return nullptr;

}
//遍历元素
template<typename K, typename V>
void  my_zskiplist<K, V>::ergodic_skiplist()
{
    std::cout << "\n*****跳表内容*****"<<"\n"; 
    for(int i = 0; i < level; i++)
    {
        my_zskiplistNode<K, V> *node = this -> header -> level[i].forward;
        std::cout << "Level " << i << ": ";
        while(node != NULL)
        {
            std::cout << node -> get_key() << ":" << node -> get_value() << ";";
            node = node -> level[i].forward;
        }
        std::cout << std::endl;
    }
}

//得到元素数量
template<typename K, typename V>
int my_zskiplist<K, V>::get_list_size()
{
    return length;
}

//将文件传入的数据转换成key和value
template<typename K, typename V>
void my_zskiplist<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

//判断文件传入的数据是否符合要求
template<typename K, typename V>
bool my_zskiplist<K, V>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

//持久化
//将内存中的数据转储到文件
template<typename K, typename V>
void my_zskiplist<K, V>::rdb_save()
{
    std::cout << "开始进行RDB持久化保存" << std::endl;
    rdbfile_save.open(RDB_FILE);

    my_zskiplistNode<K, V> *node = this -> header -> level[0].forward;

    while(node != NULL)
    {
        rdbfile_save << node -> get_key() << delimiter << node -> get_value() << "\n";
        std::cout << node -> get_key() << delimiter << node -> get_value() << ";\n";
        node = node -> level[0].forward;
    
    }

    rdbfile_save.flush();
    rdbfile_save.close();
    return;
}

//从磁盘加载数据
template<typename K, typename V>
void my_zskiplist<K, V>:: rdb_load()
{
    std::cout << "开始进行RDB持久化载入" << std::endl;
    rdbfile_load.open(RDB_FILE);

    //这里的key和value 代码是new 为什么 要这样呢
    std::string line;
    std::string key;
    std::string value;

    while(getline(rdbfile_load, line))
    {
        get_key_value_from_string(line, &key, &value);
        if (key.empty() || value.empty()){
            continue;
        }
        
        this -> insert(key, value);
        std::cout << "key:" << key << "value:" << value << std::endl;
    }

    rdbfile_load.close();
    return;
}


#endif
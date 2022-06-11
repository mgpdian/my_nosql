#ifndef MY_SKIPLIST_H
#define MY_SKIPLIST_H


#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <mutex>
#include <fstream>

#define Skiplist_MAXLEVEL 32
//设置跳表的层数

#define Skiplist_p 0.25
//决定层数的随机值


//跳表 面向的是有序集合 key - key - value

//设置跳跃表节点
typedef struct my_zskiplistNode{
    //本来应该是简单动态字符串sds, 但还去设计 所以这里暂时用string吧!
    //想错了 还有一层
    //应该保存的robj对象 对象中保存着sds
    std::string ele;

    //保存值
    double score;
    
    //回退指针
    my_zskiplistNode *backward;

    //层
    struct my_zskiplistLevel{
        //前进指针
        my_zskiplistNode *forward;
        //跨度
        unsigned int span;

    } *level;

    my_zskiplistNode(int level, std::string ele, double score);
    ~my_zskiplistNode();

} my_zskiplistNode;
inline
my_zskiplistNode::my_zskiplistNode(int level, std::string ele, double score) :
                 level(new my_zskiplistLevel[level]), ele(ele), score(score){}
inline
my_zskiplistNode::~my_zskiplistNode(){
    delete[] level;
}

class my_zskiplist{
public:
    //表头表尾指针
    my_zskiplistNode *header, *tail;
    //最大层数
    int level;
    //表长
    unsigned long length;

private:
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
    static void DeleteHelper(my_zskiplist *zsl, my_zskiplistNode *x, my_zskiplistNode **update);

public:
    //构造函数
    my_zskiplist();

    //析构
    ~my_zskiplist();

    //添加
    my_zskiplistNode* insert(std::string ele, double score);

    //删除
    int Delete(std::string ele, double score, my_zskiplistNode **node = nullptr);

    //查找
    my_zskiplistNode* search(my_zskiplist *zsl, double score);

};
//实际上实现删除的函数
void my_zskiplist::DeleteHelper(my_zskiplist *zsl, my_zskiplistNode *x, my_zskiplistNode **update)
{
    int i;
    //处理x前面的指针对它的指向
    for(i = 0; i < zsl -> level; i++)
    {
        if(update[i] -> level[i].forward == x)
        {
            update[i] -> level[i].forward = x -> level[i].forward;
            update[i] -> level[i].span += x -> level[i].span - 1;
        }
        else{
            update[i] -> level[i].span--;
        }
    }

    //处理指向 x的backward
    if(x -> level[0].forward)
    {
        x -> level[0].forward -> backward = x -> backward;
    }
    //没有指向x 的backward说明他是表尾
    else{
        zsl -> tail = x -> backward;
    }

    //移除x代表的最高层
    while(zsl -> level > 1 && zsl -> header -> level[zsl -> level - 1].forward == NULL)
    {
        zsl -> level--;
    }
    zsl -> length--;
    //x -> ~my_zskiplistNode();
    //delete x;
}
//构造函数
my_zskiplist::my_zskiplist(): level(1), length(0), tail(nullptr)
{
    //初始化跳表
    this -> header = new my_zskiplistNode(Skiplist_MAXLEVEL, "", 0);
    for(int i = 0; i < Skiplist_MAXLEVEL; i++)
    {
        this -> header -> level[i].forward = nullptr;
    }
    this -> header -> backward = nullptr;
}

//析构
my_zskiplist::~my_zskiplist()
{
    my_zskiplistNode *node = header, *next;
    while(node)
    {
        next = node -> level[0].forward;
        delete node;
        node = next;
    }
}

//添加
my_zskiplistNode* my_zskiplist::insert(std::string ele, double score)
{
    my_zskiplistNode *update[Skiplist_MAXLEVEL], *x;
    unsigned int rank[Skiplist_MAXLEVEL];
     //rank 计算每层到达当前节点的跨度  下一层以上一层的跨度为起点 
     //最后用rank[0] - rank[i] 来得到 i层到目标节点的跨度
    int i, level;

    x = this -> header;
    //确定节点匹配的地点
    for(i = this -> level - 1; i >= 0; i--)
    {
        rank[i] = (i == (this -> level - 1)) ? 0 : rank[i + 1];
        while (x->level[i].forward && (x->level[i].forward->score < score) 
        || (x->level[i].forward->score == score && x->level[i].forward->ele < ele))
        {
            rank[i] += x->level[i].span;
            x = x->level[i].forward;
        }
        update[i] = x;
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
    x = new my_zskiplistNode(level, ele, score);

    //连接
    for(i = 0; i < level; i++)
    {
        x -> level[i].forward = update[i] -> level[i].forward;
        update[i] -> level[i].forward = x;

        x -> level[i].span = update[i] -> level[i].span - (rank[0] - rank[i]) ;
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

    x -> backward = (update[0] == this -> header) ? nullptr : update[0];
    
    if(x -> level[0].forward == nullptr)
    {
        this -> tail = x;
    }
    else{
        x -> level[0].forward -> backward = x;
    }
    this -> length++;
    return x;
}

//删除
int my_zskiplist::Delete(std::string ele, double score, my_zskiplistNode **node )
{
    my_zskiplistNode *update[Skiplist_MAXLEVEL], *x;
    int i;
    
    x = this -> header;
    for(i = this -> level - 1; i >= 0; i--)
    {
        while(x -> level[i].forward && (x -> level[i].forward -> score < score)
            || (x -> level[i].forward -> score == score && x -> level[i].forward -> ele < ele))
            {
                x = x -> level[i].forward;
            }
            update[i] = x;
    }

    x = x -> level[0].forward;
    if(x && x -> score == score && x -> ele == ele)
    {
        DeleteHelper(this, x, update);

        if(!node)
        {
            delete x;
        }
        else{
            *node = x;
        }
        return 1;
    }
    return 0;

}

//查找
my_zskiplistNode* my_zskiplist::search(my_zskiplist *zsl, double score)
{
    my_zskiplistNode *x = zsl -> header;
    for(int i = zsl -> level - 1; i >= 0; i--)
    {
        while(x -> level[i].forward && (x -> level[i].forward -> score < score)
           )
            {
                x = x -> level[i].forward;
            }
    }
    auto val = x -> level[0].forward;
    if(val -> score == score){
        return val;
    }
    return nullptr;

}
#endif
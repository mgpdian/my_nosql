#ifndef MY_ZSKIPLISTNODE_H
#define MY_ZSKIPLISTNODE_H

#include <iostream>
#include <string.h>


//设置跳跃表节点
class my_zskiplistNode{

public:
    my_zskiplistNode(){}

    my_zskiplistNode(int level, const double& score, const std::string& ele);

    ~my_zskiplistNode();
    //本来应该是简单动态字符串sds, 但还去设计 所以这里暂时用string吧!
    //想错了 还有一层
    //应该保存的robj对象 对象中保存着sds
    // std::string ele;

    // //保存值
    // double score;
    
    std::string get_ele() const;

    double get_score() const;

    //没有必要的修改 因为一修改就要重新排序了
    void set_score(const double& score);

    //回退指针
    my_zskiplistNode *backward;

    //层
    struct my_zskiplistLevel{
        //前进指针
        my_zskiplistNode *forward;
        //跨度
        unsigned int span;

    } *level;
private:
    std::string ele;
    double score;

};


inline
my_zskiplistNode::my_zskiplistNode(int level, const double& score, const std::string& ele):
                 level(new my_zskiplistLevel[level]), ele(ele), score(score)
{
}


inline
my_zskiplistNode::~my_zskiplistNode(){
    delete[] level;
}

inline
std::string my_zskiplistNode::get_ele() const
{
    return ele;
}

inline
double my_zskiplistNode::get_score() const
{
    return score;
}

inline
void my_zskiplistNode::set_score(const double& score)
{
    this -> score = score;
}




#endif
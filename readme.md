# 基于跳表的kv数据库

启: 最近学了redis, 也看了Redis设计与实现, 然后学习了力扣的[1206. 设计跳表](https://leetcode.cn/problems/design-skiplist/), 正好看到卡哥的项目KV[存储引擎](https://github.com/youngyangyang04/Skiplist-CPP), 所以想自己在这些基础上实现一个简单的kv数据库

> 在写项目之前先说说我自己对kv数据库以及跳表的认识和设计方向 --这些基于<Redis设计与实现>一书 也就是Redis3.0版本的认识 如果后面版本对这些有修改的话 可能学到的时候会回来补充
>
> Redis数据库有经典的5种类型 (6.0开始又多了三种)
>
> 分别为 
>
> 1 字符串(string)
>
> 2 列表(list)
>
> 3 集合(set)
>
> 4 哈希(Hash)
>
> 5 有序列表(Zset)
>
> Redis是由对象来表示类型的
>
> ```c
> typedef struct redisObject{
> 	//类型
> 	unsigned type:4;
> 	//编码
> 	unsigned encoding:4;
> 	//指向底层实现数据结构的指针
> 	void *ptr;
> }robj;
> 
> /*
> 	编码  每种类型都至少有俩种不同的编码
> 	REDIS_STRING
> 
> */
> ```
>
> 类型
>
> | 对象unsigned type | 类型unsigned type |
> | ----------------- | ----------------- |
> | 字符串对象        | REDIS_STRING      |
> | 列表对象          | REDIS_LIST        |
> | 哈希对象          | REDIS_HASH        |
> | 集合对象          | REDIS_SET         |
> | 有序集合对象      | REDIS_ZSET        |
>
>  编码
>
> | 类型         | 编码                      | 对象                                           |
> | ------------ | ------------------------- | ---------------------------------------------- |
> | REDIS_STRING | REDIS_ENCODING_INT        | 使用整数实现的字符串对象                       |
> | REDIS_STRING | REDIS_ENCODING_EMBSTR     | 使用embstr编码的简单动态字符串实现的字符串对象 |
> | REDIS_STRING | REDIS_ENCODING_RAW        | 使用简单字符串实现的字符串对象                 |
> | REDIS_LIST   | REDIS_ENCODING_ZIPLIST    | 使用压缩列表实现的列表对象                     |
> | REDIS_LIST   | REDIS_ENCODING_LINKEDLIST | 使用双端链表实现的列表对象                     |
> | REDIS_HASH   | REDIS_ENCODING_ZIPLIST    | 使用压缩列表实现的列表对象                     |
> | REDIS_HASH   | REDIS_ENCODING_HT         | 使用字典实现的哈希对象                         |
> | REDIS_SET    | REDIS_ENCODING_INTSET     | 使用整数集合实现的集合对象                     |
> | REDIS_SET    | REDIS_ENCODING_HT         | 使用字典实现的集合对象                         |
> | REDIS_ZSET   | REDIS_ENCODING_ZIPLIST    | 使用压缩列表实现的有序集合对象                 |
> | REDIS_ZSET   | REDIS_ENCODING_SKIPLIST   | 使用跳跃表和字典实现的有序集合对象             |
>
> 当当前编码的范围无法满足对象的使用时 会进行编号转换 来达到满足
>
> 
>
> 6.0的新类型有(不是目标 所以不说)
>
> 1 Bitmaps 这是一直有的二进制 但现在把它变成了类型
>
> 2 HyperLogLog	保存不重复的变量
>
> 3 Geospatial 保存经纬度

  由于有卡哥的项目可以作为参考  我们首先来实现 基于跳表的数据库 完成后再继续来实现其他的类型

​	既然是基于跳表 就该好好学习一下跳表

> 跳表
>
> 跳表的核心就在于跳
>
> 由于它是一个一个放入的 在放入的时候会将他们排序 (这是重点  跳表只有在有序时才有效)
>
> 所以跳表的功能在于 查找 插入删除  但不能与排序有关(但实现时有序 所以无所谓)
>
> 跳的实现 在于随机 
>
> 跳表的每个节点都分为多层  一个数据能有多少次都是随机的(为什么要随机 而不是数越大层越多, 因为这样就无法跳, 只能爬了)
>
> 接下来是我个人对跳表的实现
>
> \#ifndef MY_SKIPLIST_TRY_H
>
> \#define MY_SKIPLIST_TRY_H
>
> 
>
> 
>
> \#include <iostream>
>
> \#include <math.h>
>
> \#include <algorithm>
>
> \#include <stdlib.h>
>
> \#include <mutex>
>
> \#include <fstream>
>
> 
>
> \#define Skiplist_MAXLEVEL 32
>
> //设置跳表的层数
>
> 
>
> \#define Skiplist_p 0.25
>
> //决定层数的随机值
>
> 
>
> //
>
> 
>
> //跳表 面向的是有序集合 key - key - value
>
> 
>
> //设置跳跃表节点
>
> typedef struct my_zskiplistNode{
>
>   //本来应该是简单动态字符串sds, 但还去设计 所以这里暂时用string吧!
>
>   //想错了 还有一层
>
>   //应该保存的robj对象 对象中保存着sds
>
>   std::string ele;
>
> 
>
>   //保存值
>
>   double score;
>
>   
>
>   //回退指针
>
>   my_zskiplistNode *backward;
>
> 
>
>   //层
>
>   struct my_zskiplistLevel{
>
> ​    //前进指针
>
> ​    my_zskiplistNode *forward;
>
> ​    //跨度
>
> ​    unsigned int span;
>
> 
>
>   } *level;
>
> 
>
>   my_zskiplistNode(int level, std::string ele, double score);
>
>   ~my_zskiplistNode();
>
> 
>
> } my_zskiplistNode;
>
> inline
>
> my_zskiplistNode::my_zskiplistNode(int level, std::string ele, double score) :
>
> ​         level(new my_zskiplistLevel[level]), ele(ele), score(score){}
>
> inline
>
> my_zskiplistNode::~my_zskiplistNode(){
>
>   delete[] level;
>
> }
>
> 
>
> class my_zskiplist{
>
> public:
>
>   //表头表尾指针
>
>   my_zskiplistNode *header, *tail;
>
>   //最大层数
>
>   int level;
>
>   //表长
>
>   unsigned long length;
>
> 
>
> private:
>
>   //随机生成节点层数
>
>   static int RandomLevel(){
>
> ​    int level = 1;
>
> ​    //0xFFFF = 65535
>
> ​    //根据rand()和掩码相与得到对应的随机值(0,0xffff)来产生level
>
> ​    while((rand() & 0xFFFF) < (Skiplist_p * 0xFFFF))
>
> ​    {  
>
> ​      level += 1;
>
> ​    }
>
> ​    return level < Skiplist_MAXLEVEL ? level : Skiplist_MAXLEVEL;
>
>   }
>
> 
>
>   //负责删除节点函数
>
>   static void DeleteHelper(my_zskiplist *zsl, my_zskiplistNode *x, my_zskiplistNode **update);
>
> 
>
> public:
>
>   //构造函数
>
>   my_zskiplist();
>
> 
>
>   //析构
>
>   ~my_zskiplist();
>
> 
>
>   //添加
>
>   my_zskiplistNode* insert(std::string ele, double score);
>
> 
>
>   //删除
>
>   int Delete(std::string ele, double score, my_zskiplistNode **node = nullptr);
>
> 
>
>   //查找
>
>   my_zskiplistNode* search(my_zskiplist *zsl, double score);
>
> 
>
> };
>
> //实际上实现删除的函数
>
> void my_zskiplist::DeleteHelper(my_zskiplist *zsl, my_zskiplistNode *x, my_zskiplistNode **update)
>
> {
>
>   int i;
>
>   //处理x前面的指针对它的指向
>
>   for(i = 0; i < zsl -> level; i++)
>
>   {
>
> ​    if(update[i] -> level[i].forward == x)
>
> ​    {
>
> ​      update[i] -> level[i].forward = x -> level[i].forward;
>
> ​      update[i] -> level[i].span += x -> level[i].span - 1;
>
> ​    }
>
> ​    else{
>
> ​      update[i] -> level[i].span--;
>
> ​    }
>
>   }
>
> 
>
>   //处理指向 x的backward
>
>   if(x -> level[0].forward)
>
>   {
>
> ​    x -> level[0].forward -> backward = x -> backward;
>
>   }
>
>   //没有指向x 的backward说明他是表尾
>
>   else{
>
> ​    zsl -> tail = x -> backward;
>
>   }
>
> 
>
>   //移除x代表的最高层
>
>   while(zsl -> level > 1 && zsl -> header -> level[zsl -> level - 1].forward == NULL)
>
>   {
>
> ​    zsl -> level--;
>
>   }
>
>   zsl -> length--;
>
>   //x -> ~my_zskiplistNode();
>
>   //delete x;
>
> }
>
> //构造函数
>
> my_zskiplist::my_zskiplist(): level(1), length(0), tail(nullptr)
>
> {
>
>   //初始化跳表
>
>   this -> header = new my_zskiplistNode(Skiplist_MAXLEVEL, "", 0);
>
>   for(int i = 0; i < Skiplist_MAXLEVEL; i++)
>
>   {
>
> ​    this -> header -> level[i].forward = nullptr;
>
>   }
>
>   this -> header -> backward = nullptr;
>
> }
>
> 
>
> //析构
>
> my_zskiplist::~my_zskiplist()
>
> {
>
>   my_zskiplistNode *node = header, *next;
>
>   while(node)
>
>   {
>
> ​    next = node -> level[0].forward;
>
> ​    delete node;
>
> ​    node = next;
>
>   }
>
> }
>
> 
>
> //添加
>
> my_zskiplistNode* my_zskiplist::insert(std::string ele, double score)
>
> {
>
>   my_zskiplistNode *update[Skiplist_MAXLEVEL], *x;
>
>   unsigned int rank[Skiplist_MAXLEVEL];
>
>    //rank 计算每层到达当前节点的跨度  下一层以上一层的跨度为起点 
>
>    //最后用rank[0] - rank[i] 来得到 i层到目标节点的跨度
>
>   int i, level;
>
> 
>
>   x = this -> header;
>
>   //确定节点匹配的地点
>
>   for(i = this -> level - 1; i >= 0; i--)
>
>   {
>
> ​    rank[i] = (i == (this -> level - 1)) ? 0 : rank[i + 1];
>
> ​    while (x->level[i].forward && (x->level[i].forward->score < score) 
>
> ​    || (x->level[i].forward->score == score && x->level[i].forward->ele < ele))
>
> ​    {
>
> ​      rank[i] += x->level[i].span;
>
> ​      x = x->level[i].forward;
>
> ​    }
>
> ​    update[i] = x;
>
>   }
>
> 
>
>   //生成随机层数
>
>   level = RandomLevel();
>
>   if(level > this -> level)
>
>   {
>
> ​    for(i = this -> level; i < level; i++)
>
> ​    {
>
> ​      rank[i] = 0;
>
> ​      update[i] = this -> header;
>
> ​      update[i] -> level[i].span = this -> length;
>
> ​    }
>
> 
>
> ​    this -> level = level;
>
> 
>
>   }
>
>   x = new my_zskiplistNode(level, ele, score);
>
> 
>
>   //连接
>
>   for(i = 0; i < level; i++)
>
>   {
>
> ​    x -> level[i].forward = update[i] -> level[i].forward;
>
> ​    update[i] -> level[i].forward = x;
>
> 
>
> ​    x -> level[i].span = update[i] -> level[i].span - (rank[0] - rank[i]) ;
>
> ​    update[i] -> level[i].span = rank[0] - rank[i] + 1;
>
>   }
>
> 
>
>   //收尾
>
>   //1 高于x的层数 加1 
>
>   //2 backward设置
>
>   //3 更新length
>
>   for(i = level; i < this -> level; i++)
>
>   {
>
> ​    update[i] -> level[i].span++;
>
>   }
>
> 
>
>   x -> backward = (update[0] == this -> header) ? nullptr : update[0];
>
>   
>
>   if(x -> level[0].forward == nullptr)
>
>   {
>
> ​    this -> tail = x;
>
>   }
>
>   else{
>
> ​    x -> level[0].forward -> backward = x;
>
>   }
>
>   this -> length++;
>
>   return x;
>
> }
>
> 
>
> //删除
>
> int my_zskiplist::Delete(std::string ele, double score, my_zskiplistNode **node )
>
> {
>
>   my_zskiplistNode *update[Skiplist_MAXLEVEL], *x;
>
>   int i;
>
>   
>
>   x = this -> header;
>
>   for(i = this -> level - 1; i >= 0; i--)
>
>   {
>
> ​    while(x -> level[i].forward && (x -> level[i].forward -> score < score)
>
> ​      || (x -> level[i].forward -> score == score && x -> level[i].forward -> ele < ele))
>
> ​      {
>
> ​        x = x -> level[i].forward;
>
> ​      }
>
> ​      update[i] = x;
>
>   }
>
> 
>
>   x = x -> level[0].forward;
>
>   if(x && x -> score == score && x -> ele == ele)
>
>   {
>
> ​    DeleteHelper(this, x, update);
>
> 
>
> ​    if(!node)
>
> ​    {
>
> ​      delete x;
>
> ​    }
>
> ​    else{
>
> ​      *node = x;
>
> ​    }
>
> ​    return 1;
>
>   }
>
>   return 0;
>
> 
>
> }
>
> 
>
> //查找
>
> my_zskiplistNode* my_zskiplist::search(my_zskiplist *zsl, double score)
>
> {
>
>   my_zskiplistNode *x = zsl -> header;
>
>   for(int i = zsl -> level - 1; i >= 0; i--)
>
>   {
>
> ​    while(x -> level[i].forward && (x -> level[i].forward -> score < score)
>
> ​      )
>
> ​      {
>
> ​        x = x -> level[i].forward;
>
> ​      }
>
>   }
>
>   auto val = x -> level[0].forward;
>
>   if(val -> score == score){
>
> ​    return val;
>
>   }
>
>   return nullptr;
>
> 
>
> }
>
> \#endif

但发现所想思路和卡哥的不一样 所以先学了卡哥的

问题很多 抄完后发现这都不能算一个数据库 最多只能算是一个数据结构 而且是表面上的 完成程度一般

至此 基于卡哥的跳表数据库完成  下面是压力测试

由于是虚拟机 性能比较无法和别人的文档中的结果直接进行比较

所以我通过相同环境下卡哥的代码测试和我的进行对比测试

注意: 为了提高速度, 测试过程中我将插入和查找结果的输出删除了

# 存储引擎数据表现

## 插入操作

采用随机插入数据测试：


| 插入数据规模（万条） | 我的耗时 |
| -------------------- | -------- |
| 10                   | 0.613018 |
| 50                   | 4.23669  |
| 100                  | 9.55674  |

## 取数据操作

| 取数据规模（万条） | 我的耗时 |
| ------------------ | -------- |
| 10                 | 1.72222  |
| 50                 | 10.3202  |
| 100                | 18.3696  |

和卡哥的差距应该主要在虚拟机性能和跳表节点层数的算法的问题导致的



到此基于跳表的数据库部分就结束了

接下来是我个人对此的改进

> 记录下要用到的命令
>
> ```cpp
> //保存 键以及他的类型
>  std::unordered_map<std::string, my_robj::SQL_TYPE> robj;
> 
>  //REDIS_STRING,//字符串对象
>  std::unordered_map<std::string, std::string> my_string;
> 
>  //REDIS_LIST,//列表对象
>  std::unordered_map<std::string, std::list<std::string>> my_list;
> 
>  //REDIS_HASH,//哈希对象
>  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> my_hash;
> 
>  //REDIS_SET,//集合对象
>  std::unordered_map<std::string, std::unordered_set<std::string>> my_set;
> 
>  //REDIS_ZSET,//有序集合对象
>  std::unordered_map<std::string, my_zskiplist> my_zset;
> ```
>
> 
>
> ```json
> //注意 del可以删除任意的key
> keys key 
> keys *
> exists key //查看是否存在
> del key //删除
> dbsize //这个库的key数量
> flushdb //清空数据库
> 
> 
> //字符串(string)
> //添加
> set key value
> //删除
> del key
> //查找
> get key
> //返回元素个数
> strlen key
> //遍历
> mget 
> //清空
> // clear
> 
> //还有一个写入文件
> //和一个读出文件
> // 列表(List)
> //添加
> lpush/rpush key value value1 value2
> //删除
> lpop/rpop key
> //查找 只能输入下标
> lindex key index
> 
> //返回元素个数
> llen key
> //遍历
> lrange list
> 
> //集合(set)
> //添加
> sadd key value1 value2
> //删除
> srem key value1
> //查找
> sismember  key value 
> 
> //返回元素个数
> scard key
> //遍历
> smembers <key>
> 
> //哈希(Hash)
> //添加
> hset <key> <field> <value>
> //删除
> hdel <key> <field>
> //查找
> hexists<key1><field>
> //返回元素个数
> hlen <key1>
> //遍历
> hgetall <key> //全部
> hkeys <key>	//field
> hvals <key> //value
> 
> //有序列表(Zset)
> //添加
> zadd key score value
> //删除
> zrem key value
> //查找
> zscore key value
> //返回元素个数
> zcard key
> //返回排序 (正反) 遍历
> zrange key 
> 
> //在发现自己设计的跳表需要修改 添加了一个hash表放入键和跳表节点的关系 来提高查询删除的效率
> 
> //上面的遍历都只是指定key看他的值的遍历
> //那就还有6种遍历
> ```
>
> 



遇到的困难

> ```cpp
> 无法引用 "std::unordered_map<my_sds, my_robj::SQL_TYPE, std::hash<my_sds>, std::equal_to<my_sds>, std::allocator<std::pair<const my_sds, my_robj::SQL_TYPE>>>" 的默认构造函数 -- 它是已删除的函数C/C++(1790)
> ```
>
> 找到原因: unordered_map不会为自定义类进行哈希
>
> 必须自己自定义哈希计算
>
> ```cpp
> //哈希函数的哈希模板
> namespace std{
>  template<>//函数模板具体化
>      class hash<my_sds>{
>          public:
>              size_t operator()(const my_sds &sds) const
>              {
>                   return hash<string>()(sds.get_buf());
>              }
>      };
> };
> ```
>
> 但依旧不行 由于时间问题 放弃了sds 而使用了std::string 

### 最终形态

参考redis实现了他的5种数据结构 其中只有跳表是自己实现的 其他的都借助了STL,如 set map unordered_map等 

由于使用到了字符串解析, 所以效率不会很高 就没有进行压测

感谢

> 卡哥的项目 https://github.com/youngyangyang04/Skiplist-CPP
>
> 黄健宏老师的<Redis设计与实现>
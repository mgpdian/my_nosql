
#include "my_zskiplist.h"
std::mutex mtx;
const std::string delimiter = ":";
static const std::string SQL_TYPE = "ZSET ";
//实际上实现删除的函数

void my_zskiplist::DeleteHelper(my_zskiplist *zsl, my_zskiplistNode *current, my_zskiplistNode **update)
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

my_zskiplist::my_zskiplist(): level(1), length(0), tail(nullptr)
{
    //初始化跳表
    this -> header = new my_zskiplistNode(Skiplist_MAXLEVEL, 0, "");
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

my_zskiplistNode *my_zskiplist::insert(double score, std::string ele)
{
    
    my_zskiplistNode *update[Skiplist_MAXLEVEL], *current;
    if (dictionaries.find(ele) != dictionaries.end())
    {
        //防止万一
        Delete(ele);
    }
    mtx.lock();
    
    unsigned int rank[Skiplist_MAXLEVEL];
    // rank 计算每层到达当前节点的跨度  下一层以上一层的跨度为起点
    //最后用rank[0] - rank[i] 来得到 i层到目标节点的跨度
    int i, level;

    current = this->header;
    
    //确定节点匹配的地点
    for (i = this->level - 1; i >= 0; i--)
    {
        rank[i] = (i == (this->level - 1)) ? 0 : rank[i + 1];
       
        while (current->level[i].forward && ((current->level[i].forward->get_score() < score) || (current->level[i].forward->get_score() == score && current->level[i].forward->get_ele() < ele)))
        {
            rank[i] += current->level[i].span;
            current = current->level[i].forward;
        }
        update[i] = current;
    }

    //只能说这个跳表...我不好说  用key作为唯一值 value作为保存量
    current = current->level[0].forward;
    
    if (current != NULL && current->get_ele() == ele)
    {
        std::cout << "Key: " << ele << "已经存在" << std::endl;
        mtx.unlock();
        return current;
    }

    //生成随机层数
    level = RandomLevel();
    if (level > this->level)
    {
        for (i = this->level; i < level; i++)
        {
            rank[i] = 0;
            update[i] = this->header;
            update[i]->level[i].span = this->length;
        }

        this->level = level;
    }
    current = new my_zskiplistNode(level, score, ele);

    //连接
    for (i = 0; i < level; i++)
    {
        current->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = current;

        current->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = rank[0] - rank[i] + 1;
    }

    //收尾
    // 1 高于x的层数 加1
    // 2 backward设置
    // 3 更新length
    for (i = level; i < this->level; i++)
    {
        update[i]->level[i].span++;
    }

    current->backward = (update[0] == this->header) ? nullptr : update[0];

    if (current->level[0].forward == nullptr)
    {
        this->tail = current;
    }
    else
    {
        current->level[0].forward->backward = current;
    }
    this->length++;

    dictionaries[ele] = current;

    mtx.unlock();
    return current;
}

//删除

int my_zskiplist::Delete(const std::string& ele, my_zskiplistNode **node )
{
    
    
    if(dictionaries.find(ele) == dictionaries.end())
    {
        return 0;
    }
    mtx.lock();
    my_zskiplistNode *update[Skiplist_MAXLEVEL], *current;
    int i;
    double score = dictionaries[ele]->get_score();
    
    current = this -> header;
    for(i = this -> level - 1; i >= 0; i--)
    {
            while (current->level[i].forward && ((current->level[i].forward -> get_score() < score) 
            || (current->level[i].forward->get_score() == score && current->level[i].forward->get_ele() < ele)))
            {
                current = current -> level[i].forward;
            }
            update[i] = current;
    }

    current = current -> level[0].forward;
    // if(current && current -> score == score && current -> ele == ele)
    if(current && current -> get_score() == score && current -> get_ele() == ele)
    {
        DeleteHelper(this, current, update);

        if(!node)
        {
            delete current;
            dictionaries[ele] = nullptr;
            dictionaries.erase(ele);
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

my_zskiplistNode* my_zskiplist::search(const std::string& ele)
{
    if(dictionaries.find(ele) == dictionaries.end())
    {
         std::cout << "Not Found Key:" << ele << std::endl;
        return nullptr;
    }
    my_zskiplistNode *current = dictionaries[ele];
    std::cout << " 找到了ele: " << ele << ", 他的value为: " << current->get_score() << std::endl;
    return current;
 
}
//遍历元素

void  my_zskiplist::ergodic_skiplist()
{
    std::cout << "\n*****跳表内容*****"<<"\n"; 
    for(int i = 0; i < level; i++)
    {
        my_zskiplistNode *node = this -> header -> level[i].forward;
        std::cout << "Level " << i << ": ";
        while(node != NULL)
        {
            std::cout << node -> get_ele() << "  :  " << node -> get_score() << ";      ";
            node = node -> level[i].forward;
        }
        std::cout << std::endl;
    }
}

//得到元素数量

int my_zskiplist::get_list_size()
{
    return length;
}

//将文件传入的数据转换成key和value

void my_zskiplist::get_score_value_from_string(const std::string& str, double& score, std::string& ele) {

    if(!is_valid_string(str)) {
        return;
    }
    ele = str.substr(0, str.find(delimiter));
    std::string tmp; 
    tmp = str.substr(str.find(delimiter)+1, str.length());
    score = (double)stod(tmp);

}

//判断文件传入的数据是否符合要求

bool my_zskiplist::is_valid_string(const std::string& str) {

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

bool my_zskiplist::rdb_save()
{
    std::cout << "开始进行RDB持久化保存" << std::endl;
    rdbfile_save.open(RDB_FILE, std::ios::app);

    my_zskiplistNode *node = this -> header -> level[0].forward;

    while(node != NULL)
    {
        rdbfile_save << node -> get_ele() << delimiter << node -> get_score() << "\n";
        std::cout << node -> get_ele() << delimiter << node -> get_score() << ";\n";
        node = node -> level[0].forward;
    
    }

    rdbfile_save.flush();
    rdbfile_save.close();
    return true;
}

//从磁盘加载数据

bool my_zskiplist::rdb_load(const std::string& key)
{
    std::cout << "开始进行RDB持久化载入" << std::endl;
    rdbfile_load.open(RDB_FILE);

    //这里的key和value 代码是new 为什么 要这样呢
    std::string line;
    std::string ele;
    std::string key_value;
    double score = INT32_MIN;
    bool flag = false;
    while(getline(rdbfile_load, line))
    {
        bool get_type = get_key_from_string(line, key_value);
        if(get_type && key != key_value && flag == false)
        {
            continue;
        }
        else if(get_type && key != key_value && flag == true)
        {
            break;
        }
        else if(!get_type && flag == false)
        {
            continue;
        }
        else if(get_type && key == key_value && flag == false)
        {
            flag = true;
            continue;
        }
        get_score_value_from_string(line, score, ele);
        if (ele.empty() || score == INT32_MIN){
            continue;
        }
        
        this -> insert(score, ele);
        std::cout << "score:" << score << "     ele:" << ele  << std::endl;
    }

    rdbfile_load.close();
    return true;
}

bool my_zskiplist::get_key_from_string(const std::string& str, std::string& key)
{
    if (str.empty()) {
        return false;
    }
    if (str.find(SQL_TYPE) == std::string::npos) {
        return false;
    }
    key = str.substr(SQL_TYPE.length(), str.length());
    return true;
}

#include "my_zset.h"
std::mutex mtx6;
static const std::string delimiter = ":";
static const std::string separator = ";";
static const std::string SQL_TYPE = "ZSET ";

static const std::string SQL_ZADD = "zadd";


static const std::string SQL_DEL = "del";
static const std::string SQL_ZREM = "zrem";


static const std::string SQL_ZSCORE = "zscore";

static const std::string SQL_SIZE = "strlen";
static const std::string SQL_ZCARD = "zcard";

static const std::string SQL_ZRANGE = "zrange";
static const std::string SQL_ERGODIC = "ergodic_all";

static const std::string SQL_CLEAR = "clear";
static const std::string SQL_RDB_SAVE = "rdb_save";
static const std::string SQL_RDB_LOAD = "rdb_load";
//分配器
//通过解析字符串 来分配任务
bool my_zset::parser_and_distributor(const std::string& str)
{
    //action
    std::string action = "", key = "", value = "";
    double score;
    get_action_key_value_from_string(str, action, key, score, value);
    //遍历全部
    if(str == SQL_ERGODIC)
    {
        return ergodic_all();
    }
    //返回元素个数
    else if (str == SQL_SIZE)
    {
        get_size();
        return true;
    }
    //清空
    else if(str == SQL_CLEAR)
    {
        return clear_zset();
    }
    //RDB持久化保存
    else if(str == SQL_RDB_SAVE)
    {
        return rdb_save();
    }
    //RDB持久化读取
    else if(str == SQL_RDB_LOAD)
    {
        return rdb_load();
    }
    //添加// zadd key score value
    else if(action == SQL_ZADD )
    {
        
        return insert_zset(key, score, value);
    }
    //删除 del
    else if (action == SQL_DEL)
    {
        return delete_zset(key);
    }
    //删除// zrem key value
    else if (action == SQL_ZREM)
    {
        
        return delete_zset(key, value);
    }
    //查询 // zscore key value
    else if (action == SQL_ZSCORE)
    {
        get_zset(key, value);
        return true;
    }
    
    //返回键中的元素个数 // zcard key
    else if (action == SQL_ZCARD)
    {
        get_size(key);
        return true;
    }
    //遍历键
    // zrange key 
    else if (action == SQL_ZRANGE )
    {
        
        return ergodic_zset(key);
    }
    else{
        std::cout << "输入错误" << std::endl;
    }
    return true;
}
//分析字符串指令 动作 key value

void my_zset::get_action_key_value_from_string(const std::string &str, std::string &action, std::string &key, double & score,  std::string &value)
{
    int len = sizeof(str);
    int p1 = str.find(' ');
    
    if(p1 < 0){
        action = str;
        
        return;
    }
    action = str.substr(0, p1);

    int p2 = str.find(' ', p1 + 1);
    if(p2 < 0)
    {
        key = str.substr(p1 + 1, len - p1);
        
        return;
    }
    key = str.substr(p1 + 1, p2 - p1 - 1);

    std::string field;
    int p3 = str.find(' ', p2 + 1);
    if(p3 < 0)
    {
        field = str.substr(p2 + 1, len - p2);
        value = field;
        
        return;
    }
    field = str.substr(p2 + 1, p3 - p2 - 1);
    score = stod(field);
    value = str.substr(p3 + 1, len - p3 - 1);
}
//添加
// zadd key score value
bool my_zset::insert_zset(const std::string &key, const double &score, const std::string &value)
{
    mtx6.lock();
    if(my_zset_map.find(key) == my_zset_map.end())
    {
        size++;
        
        
    }
    //可以这样吗
    
    my_zset_map[key].insert(score, value);
    
    mtx6.unlock();
    return true;
}
//这是del直接删除键值对
bool my_zset::delete_zset(const std::string &key)
{
    mtx6.lock();
    if(my_zset_map.find(key) == my_zset_map.end())
    {
        mtx6.unlock();
        return false;
    }
    my_zset_map.erase(key);
    size--;
    mtx6.unlock();
    return true;
}
 // //删除
// zrem key value
bool my_zset::delete_zset(const std::string &key, const std::string &value)
{
    mtx6.lock();
    if(my_zset_map.find(key) == my_zset_map.end())
    {
        mtx6.unlock();
        return false;
    }
    my_zset_map[key].Delete(value);
    if(my_zset_map[key].get_list_size() == 0)
    {
        my_zset_map.erase(key);
        size--;
    }
    mtx6.unlock();
    return true;
}
 // //查找
// zscore key value
double my_zset::get_zset(const std::string &key, const std::string& value)
{
    if(my_zset_map.find(key) == my_zset_map.end())
    {
        std::cout << "(nil)" << std::endl;
        return 0;
    }
    double score;
    auto temp = my_zset_map[key].search(value);
    if(temp == nullptr)
    {
        std::cout << "(nil)" << std::endl;
        return 0;
    }
    score = temp->get_score();
    std::cout << "\"" << score <<"\"" << std::endl;
    return score;
}
//返回元素个数
// strlen
int my_zset::get_size()
{
    //std::cout << size << std::endl;
    return size;
}
// //返回元素个数
// zcard key
int my_zset::get_size(const std::string &key)
{
    int len = my_zset_map[key].get_list_size();
    std::cout << len << std::endl;
    return len;
}

 // //遍历
// zrange key 
bool my_zset::ergodic_zset(const std::string& key)
{
    if(my_zset_map.find(key) == my_zset_map.end())
    {
        std::cout << "key不存在";
        return false;
    }
    
        //遍历key的field和value
        std::cout << "key:" << key << ":" << std::endl;
        my_zset_map[key].ergodic_skiplist();
        std::cout << std::endl;
    
   
    std::cout << std::endl;
    return true;
}
bool my_zset::ergodic_all()
{
    std::cout << "\n*****有序集合表内容*****"<<"\n"; 
    for(auto & map : my_zset_map)
    {
        
        std::cout << "key:" << map.first << ":" << std::endl;
        my_zset_map[map.first].ergodic_skiplist();
        std::cout << std::endl;
    }
    return true;
}
bool my_zset::clear_zset()
{
    my_zset_map.clear();
    return true;
}

//持久化
//将内存中的数据转储到文件

bool my_zset::rdb_save()
{
    std::cout << "开始进行RDB持久化中的有序集合表保存" << std::endl;
    rdbfile_save.open(RDB_FILE_ZSET, std::ios::app);

    for(auto & map : my_zset_map)
    {
        
        rdbfile_save << SQL_TYPE << map.first << "\n";
        rdbfile_save.flush();
        
        std::cout << SQL_TYPE << ";\n";
        my_zset_map[map.first].rdb_save();
        
        
        std::cout << std::endl;
    }
    rdbfile_save.flush();
    rdbfile_save.close();
    return true;
}

//从磁盘加载数据

bool my_zset::rdb_load()
{
    std::cout << "开始进行RDB持久化中的有序集合表载入" << std::endl;
    rdbfile_load.open(RDB_FILE_ZSET);

    
    std::string line;
    
    std::string field;
    std::string value;
    
    while(getline(rdbfile_load, line))
    {
        std::string key;
        get_key_field_value_from_string(line, key);
        if (key.empty()){
            continue;
        }
        std::cout << "有序集合类型  key:" << key << std::endl;
        size++;
        my_zset_map[key].rdb_load(key);
        
    }
    rdbfile_load.close();
    return true;
}


//将文件传入的数据转换成key和value

void my_zset::get_key_field_value_from_string(const std::string& str, std::string& key) {

    if(!is_valid_string(str)) {
        return;
    }
    key = str.substr(SQL_TYPE.length(), str.length() - SQL_TYPE.length());
}

//判断文件传入的数据是否符合要求

bool my_zset::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(SQL_TYPE) == std::string::npos) {
        return false;
    }
    
    return true;
}

//解决RDB载入后 robj的记录无法登记类型的问题
std::vector<std::string> my_zset::robj_register()
{
    std::vector<std::string> robj;
    for (auto &str : my_zset_map)
    {
        robj.emplace_back(str.first);
    }
    return robj;
}
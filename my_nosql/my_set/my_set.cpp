
#include "my_set.h"
std::mutex mtx2;
static const std::string delimiter = ":";

static const std::string SQL_TYPE = "SET ";

static const std::string SQL_SADD = "sadd";


static const std::string SQL_DEL = "del";
static const std::string SQL_SREM = "srem";


static const std::string SQL_SISMEMBER = "sismember";

static const std::string SQL_SIZE = "strlen";
static const std::string SQL_SCARD = "scard";

static const std::string SQL_SMEMBERS = "smembers";
static const std::string SQL_ERGODIC = "ergodic_all";

static const std::string SQL_CLEAR = "clear";
static const std::string SQL_RDB_SAVE = "rdb_save";
static const std::string SQL_RDB_LOAD = "rdb_load";
//分配器
//通过解析字符串 来分配任务
bool my_set::parser_and_distributor(const std::string& str)
{
    //action
    std::string action = "", key = "", value = "";
    get_action_key_value_from_string(str, action, key, value);
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
        return clear_set();
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
    //添加// sadd key value1 value2
    else if(action == SQL_SADD )
    {
        
        return insert_set(key, value);
    }
    //删除 del
    else if (action == SQL_DEL)
    {
        return delete_set(key);
    }
    //删除// srem key value1
    else if (action == SQL_SREM)
    {
        
        return delete_set(key, value);
    }
    //查询 // sismember  key value 
    else if (action == SQL_SISMEMBER)
    {
        get_set(key, value);
        return true;
    }
    
    //返回键中的元素个数 // scard key
    else if (action == SQL_SCARD)
    {
        get_size(key);
        return true;
    }
    //遍历键
    // //遍历
    // smembers <key>
    else if (action == SQL_SMEMBERS)
    {
        
        return ergodic_set(key);
    }
    else{
        std::cout << "输入错误" << std::endl;
    }
    return true;
}
//分析字符串指令 动作 key value

void my_set::get_action_key_value_from_string(const std::string &str, std::string &action, std::string &key, std::string &value)
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
    value = str.substr(p2 + 1, len - p2 - 1);
}
//添加
// sadd key value1
bool my_set::insert_set(const std::string &key, const std::string &value)
{
    mtx2.lock();
    if(my_set_map.find(key) == my_set_map.end())
    {

        size++;
       
    } 
    //可以这样吗std::cout << size << std::endl;
    my_set_map[key].insert(value);

    mtx2.unlock();
    return true;
}
//这是del直接删除键值对
bool my_set::delete_set(const std::string &key)
{
    mtx2.lock();
    if(my_set_map.find(key) == my_set_map.end())
    {
        mtx2.unlock();
        return false;
    }
    my_set_map.erase(key);
    size--;
    mtx2.unlock();
    return true;
}
 // //删除
// srem key value1
bool my_set::delete_set(const std::string &key, const std::string &value)
{
    mtx2.lock();
    if(my_set_map.find(key) == my_set_map.end())
    {
        mtx2.unlock();
        return false;
    }
    my_set_map[key].erase(value);
    if(my_set_map[key].empty())
    {
        my_set_map.erase(key);
        size--;
    }
    mtx2.unlock();
    return true;
}
 // //查找
// sismember  key value
std::string my_set::get_set(const std::string &key, const std::string& value)
{
    if(my_set_map.find(key) == my_set_map.end())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }
    
    if(my_set_map[key].find(value) == my_set_map[key].end())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }
    
    std::cout << "\"" << value <<"\"" << std::endl;
    return value;
}
//返回元素个数
// strlen
int my_set::get_size()
{
    //std::cout << size << std::endl;
    return size;
}
// //返回元素个数
// scard key
int my_set::get_size(const std::string &key)
{
    int len = my_set_map[key].size();
    std::cout << len << std::endl;
    return len;
}

 // //遍历
// smembers <key>
bool my_set::ergodic_set(const std::string &key)
{
    if (my_set_map.find(key) == my_set_map.end())
    {
        std::cout << "key不存在";
        return false;
    }

    //遍历key的field和value
    std::cout << "key:" << key << ":" << std::endl;
    for (auto &set : my_set_map[key])
    {
        std::cout << "  value: " << set << std::endl;
    }
    std::cout << std::endl;

    return true;
}
bool my_set::ergodic_all()
{
    std::cout << "\n*****集合表内容*****"<<"\n"; 
    for(auto & map : my_set_map)
    {
        
        std::cout << "key:" << map.first << ":" << std::endl;
        for (auto &set : map.second)
        {
            std::cout << "  value: " << set << std::endl;
        }
        std::cout << std::endl;
    }
    return true;
}
bool my_set::clear_set()
{
    my_set_map.clear();
    return true;
}

//持久化
//将内存中的数据转储到文件

bool my_set::rdb_save()
{
    std::cout << "开始进行RDB持久化中的集合表保存" << std::endl;
    rdbfile_save.open(RDB_FILE_SET);

    for(auto & map : my_set_map)
    {
        
        for(auto & set : map.second)
        {
            rdbfile_save << SQL_TYPE << map.first << delimiter << set << "\n";
            std::cout << SQL_TYPE << map.first << delimiter << set << "\n";
        }
        
    }
    rdbfile_save.flush();
    rdbfile_save.close();
    return true;
}

//从磁盘加载数据

bool my_set::rdb_load()
{
    std::cout << "开始进行RDB持久化中的集合表载入" << std::endl;
    rdbfile_load.open(RDB_FILE_SET);

    
    std::string line;
    std::string key;
    std::string value;
    
    while(getline(rdbfile_load, line))
    {
        get_key_value_from_string(line, key, value);
        if (key.empty()  || value.empty()){
            continue;
        }
        insert_set(key, value);
        std::cout << "集合类型  key:" << key  <<  "value:" << value << std::endl;
    }
    rdbfile_load.close();
    return true;
}


//将文件传入的数据转换成key和value

void my_set::get_key_value_from_string(const std::string& str, std::string& key,  std::string& value) {

    if(!is_valid_string(str)) {
        return;
    }
    key = str.substr(SQL_TYPE.length(), str.find(delimiter) - SQL_TYPE.length());
    
    value = str.substr(str.find(delimiter)+1, str.length());
}

//判断文件传入的数据是否符合要求

bool my_set::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(SQL_TYPE) == std::string::npos) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    
    return true;
}
//解决RDB载入后 robj的记录无法登记类型的问题
std::vector<std::string> my_set::robj_register()
{
    std::vector<std::string> robj;
    for (auto &str : my_set_map)
    {
        robj.emplace_back(str.first);
    }
    return robj;
}
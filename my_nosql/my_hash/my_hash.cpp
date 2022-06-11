
#include "my_hash.h"
std::mutex mtx4;
static const std::string delimiter = ":";
static const std::string separator = ";";
static const std::string SQL_TYPE = "HASH ";

static const std::string SQL_HSET = "hset";


static const std::string SQL_DEL = "del";
static const std::string SQL_HDEL = "hdel";


static const std::string SQL_HEXISTS = "hexists";

static const std::string SQL_SIZE = "strlen";
static const std::string SQL_HLEN = "hlen";

static const std::string SQL_HGETALL = "hgetall";
static const std::string SQL_HKEYS = "hkeys";
static const std::string SQL_HVALS = "hvals";
static const std::string SQL_ERGODIC = "ergodic_all";

static const std::string SQL_CLEAR = "clear";
static const std::string SQL_RDB_SAVE = "rdb_save";
static const std::string SQL_RDB_LOAD = "rdb_load";
//分配器
//通过解析字符串 来分配任务
bool my_hash::parser_and_distributor(const std::string& str)
{
    //action
    std::string action = "", key = "", field = "", value = "";
    get_action_key_value_from_string(str, action, key, field, value);
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
        return clear_hash();
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
    //添加
    else if(action == SQL_HSET )
    {
        
        return insert_hash(key, field, value);
    }
    //删除 del
    else if (action == SQL_DEL)
    {
        return delete_hash(key);
    }
    //删除 hdel <key> <field>
    else if (action == SQL_HDEL)
    {
        
        return delete_hash(key, field);
    }
    //查询 hexists<key1><field>
    else if (action == SQL_HEXISTS)
    {
        get_hash(key, field);
        return true;
    }
    
    //返回键中的元素个数 hlen <key1>
    else if (action == SQL_HLEN)
    {
        get_size(key);
        return true;
    }
    //遍历键
    //hgetall <key> //全部
    //hkeys <key>	//field
    //hvals <key> //value
    else if (action == SQL_HGETALL || action == SQL_HKEYS || action == SQL_HVALS)
    {
        int choice;
        if(action == SQL_HGETALL) choice = 0;
        else if(action == SQL_HKEYS) choice = 1;
        else choice = 2;
        return ergodic_hash(choice, key);
    }
    else{
        std::cout << "输入错误" << std::endl;
    }
    return true;
}
//分析字符串指令 动作 key value

void my_hash::get_action_key_value_from_string(const std::string &str, std::string &action, std::string &key, std::string &field,  std::string &value)
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

    int p3 = str.find(' ', p2 + 1);
    if(p3 < 0)
    {
        field = str.substr(p2 + 1, len - p2);
        
        return;
    }
    field = str.substr(p2 + 1, p3 - p2 - 1);

    value = str.substr(p3 + 1, len - p3 - 1);
}
//添加
    // hset <key> <field> <value>
bool my_hash::insert_hash(const std::string &key, const std::string &field, const std::string &value)
{
    mtx4.lock();
    if(my_hash_map.find(key) == my_hash_map.end())
    {
        size++;
    }
    //可以这样吗
    my_hash_map[key][field] = value;
    mtx4.unlock();
    return true;
}
//这是del直接删除键值对
bool my_hash::delete_hash(const std::string &key)
{
    mtx4.lock();
    if(my_hash_map.find(key) == my_hash_map.end())
    {
        mtx4.unlock();
        return false;
    }
    my_hash_map.erase(key);
    size--;
    mtx4.unlock();
    return true;
}
 // //删除
    // hdel <key> <field>
bool my_hash::delete_hash(const std::string &key, const std::string &field)
{
    mtx4.lock();
    if(my_hash_map.find(key) == my_hash_map.end())
    {
        mtx4.unlock();
        return false;
    }
    my_hash_map[key].erase(field);
    if(my_hash_map[key].empty())
    {
        my_hash_map.erase(key);
        size--;
    }
    mtx4.unlock();
    return true;
}
 // //查找
    // hexists<key1><field>
std::string my_hash::get_hash(const std::string &key, const std::string& field)
{
    if(my_hash_map.find(key) == my_hash_map.end())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }
    std::string value;
    if(my_hash_map[key].find(field) == my_hash_map[key].end())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }
    value = my_hash_map[key][field];
    std::cout << "\"" << value <<"\"" << std::endl;
    return value;
}
//返回元素个数
// strlen
int my_hash::get_size()
{
    //std::cout << size << std::endl;
    return size;
}
// //返回元素个数
// hlen <key1>
int my_hash::get_size(const std::string &key)
{
    int len = my_hash_map[key].size();
    std::cout << len << std::endl;
    return len;
}

 // //遍历
    // hgetall <key> //全部
    // hkeys <key>	//field
    // hvals <key> //value
bool my_hash::ergodic_hash(const int choice, const std::string& key)
{
    if(my_hash_map.find(key) == my_hash_map.end())
    {
        std::cout << "key不存在";
        return false;
    }
    if (choice == 0)
    {
        //遍历key的field和value
        std::cout << "key:" << key << ":" << std::endl;
        for (auto &hash : my_hash_map[key])
        {
            std::cout << "     field:"<< hash.first << "  value: " << hash.second << std::endl;
        }
        std::cout << std::endl;
    }
    else if(choice == 1)
    {
        //遍历key 的field
        std::cout << "key:" << key << ":" << std::endl;
        for (auto &hash : my_hash_map[key])
        {
            std::cout << "     field:"<< hash.first << std::endl;
        }
        std::cout << std::endl;
    }
    else if(choice == 2)
    {
        //遍历key的value
        std::cout << "key:" << key << ":" << std::endl;
        for (auto &hash : my_hash_map[key])
        {
            std::cout << "     value: " << hash.second << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return true;
}
bool my_hash::ergodic_all()
{
    std::cout << "\n*****哈希表内容*****"<<"\n"; 
    for(auto & map : my_hash_map)
    {
        
        std::cout << "key:" << map.first << ":" << std::endl;
        for (auto &hash : map.second)
        {
            std::cout << "     field:"<< hash.first << "  value: " << hash.second << std::endl;
        }
        std::cout << std::endl;
    }
    return true;
}
bool my_hash::clear_hash()
{
    my_hash_map.clear();
    return true;
}

//持久化
//将内存中的数据转储到文件

bool my_hash::rdb_save()
{
    std::cout << "开始进行RDB持久化中的哈希表保存" << std::endl;
    rdbfile_save.open(RDB_FILE_HASH);

    for(auto & map : my_hash_map)
    {
        
        for(auto & hash : map.second)
        {
            rdbfile_save << SQL_TYPE << map.first << delimiter << hash.first << separator << hash.second<< "\n";
            std::cout << SQL_TYPE << map.first << delimiter << hash.first << separator << hash.second<< "\n";
        }
        
    }
    rdbfile_save.flush();
    rdbfile_save.close();
    return true;
}

//从磁盘加载数据

bool my_hash::rdb_load()
{
    std::cout << "开始进行RDB持久化中的哈希表载入" << std::endl;
    rdbfile_load.open(RDB_FILE_HASH);

    
    std::string line;
    std::string key;
    std::string field;
    std::string value;
    
    while(getline(rdbfile_load, line))
    {
        get_key_field_value_from_string(line, key, field, value);
        if (key.empty() || field.empty() || value.empty()){
            continue;
        }
        insert_hash(key, field, value);
        std::cout << "哈希类型  key:" << key <<"  field:" << field <<  "  value:" << value << std::endl;
    }
    rdbfile_load.close();
    return true;
}


//将文件传入的数据转换成key和value

void my_hash::get_key_field_value_from_string(const std::string& str, std::string& key, std::string &field,  std::string& value) {

    if(!is_valid_string(str)) {
        return;
    }
    key = str.substr(SQL_TYPE.length(), str.find(delimiter) - SQL_TYPE.length());
    field = str.substr(str.find(delimiter)+1, str.find(separator) - str.find(delimiter) - 1);
    value = str.substr(str.find(separator)+1, str.length());
}

//判断文件传入的数据是否符合要求

bool my_hash::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(SQL_TYPE) == std::string::npos) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    if (str.find(separator) == std::string::npos) {
        return false;
    }
    return true;
}
//解决RDB载入后 robj的记录无法登记类型的问题
std::vector<std::string> my_hash::robj_register()
{
    std::vector<std::string> robj;
    for (auto &str : my_hash_map)
    {
        robj.emplace_back(str.first);
    }
    return robj;
}
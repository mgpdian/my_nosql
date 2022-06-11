
#include "my_list.h"
std::mutex mtx3;
static const std::string delimiter = ":";
static const std::string SQL_TYPE = "LIST ";

static const std::string SQL_LPUSH = "lpush";
static const std::string SQL_RPUSH = "rpush";

static const std::string SQL_DEL = "del";
static const std::string SQL_LPOP = "lpop";
static const std::string SQL_RPOP = "rpop";

static const std::string SQL_LINDEX = "lindex";

static const std::string SQL_SIZE = "strlen";
static const std::string SQL_LLEN = "llen";

static const std::string SQL_LRANGE = "lrange";
static const std::string SQL_ERGODIC = "ergodic_all";

static const std::string SQL_CLEAR = "clear";
static const std::string SQL_RDB_SAVE = "rdb_save";
static const std::string SQL_RDB_LOAD = "rdb_load";
//分配器
//通过解析字符串 来分配任务
bool my_list::parser_and_distributor(const std::string& str)
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
        return clear_list();
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
    else if(action == SQL_LPUSH || action == SQL_RPUSH)
    {
        int choice;
        if(action == SQL_LPUSH) choice = 0;
        else choice = 1;
        return insert_list(choice, key, value);
    }
    //删除 del
    else if (action == SQL_DEL)
    {
        return delete_list(key);
    }
    //删除 lpop rpop
    else if (action == SQL_LPOP || action == SQL_RPOP)
    {
        int choice;
        if(action == SQL_LPUSH) choice = 0;
        else choice = 1;
        return delete_list(choice, key);
    }
    //查询
    else if (action == SQL_LINDEX)
    {
        get_list(key, value);
        return true;
    }
    
    //返回键中的元素个数 llen key
    else if (action == SQL_LLEN)
    {
        get_size(key);
        return true;
    }
    //遍历键
    else if (action == SQL_LRANGE)
    {
        return ergodic_list(key);
    }
    else{
        std::cout << "输入错误" << std::endl;
    }
    return true;
}
//分析字符串指令 动作 key value

void my_list::get_action_key_value_from_string(const std::string &str, std::string &action, std::string &key, std::string &value)
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
bool my_list::insert_list(const int choice, const std::string& key, const std::string& value)
{
    mtx3.lock();
    if(my_list_map.find(key) == my_list_map.end())
    {
        size++;
    }
    if(choice == 0)
    {
        my_list_map[key].push_front(value);
    }
    else {
        my_list_map[key].push_back(value);
    }
    
    mtx3.unlock();
    return true;
}
//这是del直接删除键值对
bool my_list::delete_list(const std::string& key)
{
    mtx3.lock();
    if(my_list_map.find(key) == my_list_map.end())
    {
        mtx3.unlock();
        return false;
    }
    my_list_map.erase(key);
    size--;
    mtx3.unlock();
    return true;
}
//这是lpop rpop 删除一个值 当没有值时 删除键
bool my_list::delete_list(const int choice, const std::string& key)
{
    mtx3.lock();
    if(my_list_map.find(key) == my_list_map.end())
    {
        mtx3.unlock();
        return false;
    }
    if(choice == 0)
    {
        my_list_map[key].pop_front();
    }
    else {
        my_list_map[key].pop_back();
    }
    if(my_list_map[key].empty())
    {
        my_list_map.erase(key);
        size--;
    }
    mtx3.unlock();
    return true;
}
//查找 只能输入下标 
//lindex key index (从左到右)
std::string my_list::get_list(const std::string& key, const std::string& index)
{
    if(my_list_map.find(key) == my_list_map.end())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }
    int i = 0;
    std::string value;
    int target = stoi(index);
    if(target > my_list_map[key].size())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }
    for(auto & list : my_list_map[key])
    {
        
        if(i == target - 1)
        {
            value = list;
            break;
        }
        i++;
    }
    std::cout << "\"" << value <<"\"" << std::endl;
    return value;
}
//返回元素个数
// strlen
int my_list::get_size()
{
    //std::cout << size << std::endl;
    return size;
}

//返回键中的元素个数
//llen key
int my_list::get_size(const std::string &key)
{
    int len = my_list_map[key].size();
    std::cout << len << std::endl;
    return len;
}

//遍历
//lrange list
bool my_list::ergodic_list(const std::string& key)
{
    if(my_list_map.find(key) == my_list_map.end())
    {
        std::cout << "key不存在";
        return false;
    }
    std::cout <<"key:"<< key << ":";
    for(auto & list : my_list_map[key])
    {
        std::cout <<"   value: " << list << std::endl;
    }
    std::cout << std::endl;
    return true;
}
bool my_list::ergodic_all()
{
    std::cout << "\n*****列表表内容*****"<<"\n"; 
    for(auto & map : my_list_map)
    {
        std::cout << map.first << ":";
        for(auto & list : map.second)
        {
            std::cout << list << "  ->  ";
        }
        std::cout << std::endl;
    }
    return true;
}
bool my_list::clear_list()
{
    my_list_map.clear();
    return true;
}

//持久化
//将内存中的数据转储到文件

bool my_list::rdb_save()
{
    std::cout << "开始进行RDB持久化中的列表表保存" << std::endl;
    rdbfile_save.open(RDB_FILE_LIST);

    for(auto & map : my_list_map)
    {
        
        for(auto & list : map.second)
        {
            rdbfile_save << SQL_TYPE << map.first << delimiter << list << "\n";
            std::cout << SQL_TYPE << map.first << delimiter << list << ";\n";
        }
        
    }
    rdbfile_save.flush();
    rdbfile_save.close();
    return true;
}

//从磁盘加载数据

bool my_list::rdb_load()
{
    std::cout << "开始进行RDB持久化中的列表表载入" << std::endl;
    rdbfile_load.open(RDB_FILE_LIST);

    
    std::string line;
    std::string key;
    std::string value;
    while(getline(rdbfile_load, line))
    {
        get_key_value_from_string(line, key, value);
        if (key.empty() || value.empty()){
            continue;
        }
        insert_list(0, key, value);
        std::cout << "列表类型  key:" << key << "  value:" << value << std::endl;
    }
    rdbfile_load.close();
    return true;
}


//将文件传入的数据转换成key和value

void my_list::get_key_value_from_string(const std::string& str, std::string& key, std::string& value) {

    if(!is_valid_string(str)) {
        return;
    }
    key = str.substr(SQL_TYPE.length(), str.find(delimiter) - SQL_TYPE.length());
    value = str.substr(str.find(delimiter)+1, str.length());
}

//判断文件传入的数据是否符合要求

bool my_list::is_valid_string(const std::string& str) {

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
std::vector<std::string> my_list::robj_register()
{
    std::vector<std::string> robj;
    for (auto &str : my_list_map)
    {
        robj.emplace_back(str.first);
    }
    return robj;
}
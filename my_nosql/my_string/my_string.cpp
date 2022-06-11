
#include "my_string.h"
std::mutex mtx5;
static const std::string delimiter = ":";
static const std::string SQL_TYPE = "STRING ";

static const std::string SQL_SET = "set";
static const std::string SQL_DEL = "del";
static const std::string SQL_GET = "get";
static const std::string SQL_SIZE = "strlen";
static const std::string SQL_ERGODIC = "ergodic_all";
static const std::string SQL_CLEAR = "clear";
static const std::string SQL_RDB_SAVE = "rdb_save";
static const std::string SQL_RDB_LOAD = "rdb_load";
//分配器
//通过解析字符串 来分配任务
bool my_string::parser_and_distributor(const std::string& str)
{
    //action
    std::string action = "", key = "", value = "";
    get_action_key_value_from_string(str, action, key, value);
    //std::cout << str << std::endl;
    //std::cout << action << std::endl;
    //遍历全部
    if(str == SQL_ERGODIC)
    {
        return ergodic_all();
    }
    //长度
    else if (str == SQL_SIZE)
    {
        get_size();
        return true;
    }
    //清空
    else if(str == SQL_CLEAR)
    {
        return clear_string();
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
    else if(action == SQL_SET)
    {
        return insert_string(key, value);
    }
    //删除
    else if (action == SQL_DEL)
    {
        return delete_string(key);
    }
    //查询
    else if (action == SQL_GET)
    {
        get_string(key);
        return true;
    }
    
    else{
        std::cout << "输入错误" << std::endl;
    }
    return true;
}
//分析字符串指令 动作 key value

void my_string::get_action_key_value_from_string(const std::string &str, std::string &action, std::string &key, std::string &value)
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
bool my_string::insert_string(const std::string& key, const std::string& value) 
{
    mtx5.lock();
    if(my_string_map.find(key) == my_string_map.end())
    {
        size++;
    }
    my_string_map[key] = value; 
    mtx5.unlock();
    return true;
}
bool my_string::delete_string(const std::string& key) //因为字符串 所以不用细分 实际上删除有俩种Key value pair
{
    mtx5.lock();
    if(my_string_map.find(key) == my_string_map.end())
    {
        mtx5.unlock();
        return false;
    }
    my_string_map.erase(key);
    size--;
    mtx5.unlock();
    return true;
}
std::string my_string::get_string(const std::string& key)
{
    if(my_string_map.find(key) == my_string_map.end())
    {
        std::cout << "(nil)" << std::endl;
        return "(nil)";
    }

    std::cout << "\"" << my_string_map[key] <<"\"" << std::endl;
    return my_string_map[key];
}
int my_string::get_size()
{
    //std::cout << size << std::endl;
    return size;
}
bool my_string::ergodic_all()
{
    std::cout << "\n*****字符串表内容*****"<<"\n"; 
    for(auto & map : my_string_map)
    {
        std::cout << "\"" << map.first <<"\": " <<  "\"" << map.second <<"\""<< std::endl;
    }
    return true;
}
bool my_string::clear_string()
{
    my_string_map.clear();
    return true;
}

//持久化
//将内存中的数据转储到文件

bool my_string::rdb_save()
{
    std::cout << "开始进行RDB持久化中的字符串表保存" << std::endl;
    rdbfile_save.open(RDB_FILE_STRING);
    for(auto & map : my_string_map)
    {
        rdbfile_save << SQL_TYPE << map.first << delimiter << map.second << "\n";
        std::cout << SQL_TYPE << map.first << delimiter << map.second << ";\n";
    }
    rdbfile_save.flush();
    rdbfile_save.close();
    return true;
}

//从磁盘加载数据

bool my_string::rdb_load()
{
    std::cout << "开始进行RDB持久化中的字符串表载入" << std::endl;
    rdbfile_load.open(RDB_FILE_STRING);

    
    std::string line;
    std::string key;
    std::string value;
    while(getline(rdbfile_load, line))
    {
        get_key_value_from_string(line, key, value);
        if (key.empty() || value.empty()){
            continue;
        }
        std::cout << "字符串类型  key:" << key << "  value:" << value << std::endl;
        insert_string(key, value);
        
    }
    rdbfile_load.close();
    return true;
}


//将文件传入的数据转换成key和value

void my_string::get_key_value_from_string(const std::string& str, std::string& key, std::string& value) {

    if(!is_valid_string(str)) {
        return;
    }
    key = str.substr(SQL_TYPE.length(), str.find(delimiter) - SQL_TYPE.length());
    value = str.substr(str.find(delimiter)+1, str.length());
}

//判断文件传入的数据是否符合要求

bool my_string::is_valid_string(const std::string& str) {

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
std::vector<std::string> my_string::robj_register()
{
    std::vector<std::string> robj;
    for (auto &str : my_string_map)
    {
        robj.emplace_back(str.first);
    }
    return robj;
}
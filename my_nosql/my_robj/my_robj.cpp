#include "my_robj.h"
//全局命令
static const std::string SQL_KEYS = "keys *";
static const std::string SQL_TYPE_KEY = "type";
static const std::string SQL_CLEAR_ALL = "flushdb";
static const std::string SQL_CLEAR = "clear";
static const std::string SQL_ERGODIC = "ergodic_all";//遍历全部
static const std::string SQL_DBSIZE = "dbsize";
//static const std::string SQL_SIZE = "strlen";
static const std::string SQL_RDB_SAVE = "rdb_save";
static const std::string SQL_RDB_LOAD = "rdb_load";

static const std::string SQL_DEL = "del";

//字符串(string)的命令
static const std::string SQL_SET = "set";
static const std::string SQL_GET = "get";


//列表(list)的命令
static const std::string SQL_LPUSH = "lpush";
static const std::string SQL_RPUSH = "rpush";
static const std::string SQL_LPOP = "lpop";
static const std::string SQL_RPOP = "rpop";
static const std::string SQL_LINDEX = "lindex";
static const std::string SQL_LLEN = "llen";
static const std::string SQL_LRANGE = "lrange";

//哈希(hash)命令
static const std::string SQL_HSET = "hset";
static const std::string SQL_HDEL = "hdel";
static const std::string SQL_HEXISTS = "hexists";
static const std::string SQL_HLEN = "hlen";
static const std::string SQL_HGETALL = "hgetall";
static const std::string SQL_HKEYS = "hkeys";
static const std::string SQL_HVALS = "hvals";

//集合(set)命令
static const std::string SQL_SADD = "sadd";
static const std::string SQL_SREM = "srem";
static const std::string SQL_SISMEMBER = "sismember";
static const std::string SQL_SCARD = "scard";
static const std::string SQL_SMEMBERS = "smembers";

//有序集合(zset)的命令
static const std::string SQL_ZADD = "zadd";
static const std::string SQL_ZREM = "zrem";
static const std::string SQL_ZSCORE = "zscore";
static const std::string SQL_ZCARD = "zcard";
static const std::string SQL_ZRANGE = "zrange";
std::mutex mtx1;
//解析传入的字符串
//现在的思路是 只暴露这个函数  通过这个函数来解析字符串 以此来调用其他方法 问题是解析过程有点麻烦
void my_robj::parser_and_distributor(const std::string &str)
{
    std::string key;
    int choice;
    choice = parse_string(str, key);
    switch(choice)
    {
        case 1:{
            //keys *
            //遍历全部键
            ergodic_key();

            break;
        }
        case 2:{
            //type key
            //key的对应类型
            type_key(key);
            
            break;
        }
        case 3:{
            //flushdb
            //清空库
            
            if(clear())
            {
                std::cout << "OK" << std::endl;
            }
            break;
        }
        case 4:{
            //ergodic_all
            //遍历全部
            ergodic_all();
            
            break;
        }
        case 5:{
            //dbsize
            //全部键的总长
            get_size();
            
            break;
        }
        case 6:{
            ////rdb_save
            //RDB保存
            
            if(rdb_save())
            {
                std::cout << "OK" << std::endl;
            }
            break;
        }
        case 7:{
            //rdb_load
            //RDB载入
            
            if(rdb_load())
            {
                std::cout << "OK" << std::endl;
            }
            break;
        }
        case 8:{
            //del key
            //删除键
            
            if(delete_key(str, key))
            {
                std::cout << "OK" << std::endl;
            }
            break;
        }
        case 9:{
            //string的数据库
            if(!key_type_real(key, REDIS_STRING))
            {
                break;
            }
            my_string_object.parser_and_distributor(str);
            if(my_string_size + 1 == my_string_object.get_size() )
            {
                robj[key] = REDIS_STRING;
                my_string_size++;
                size++;
                std::cout << "OK" << std::endl;
            }
            else if(my_string_size - 1 == my_string_object.get_size())
            {
                robj.erase(key);
                my_string_size--;
                size--;
                std::cout << "OK" << std::endl;
            }
            
            break;
        }
        case 10:{
            //list的数据库
            if(!key_type_real(key, REDIS_LIST))
            {
                break;
            }
            my_list_object.parser_and_distributor(str);
            if(my_list_size + 1 == my_list_object.get_size() )
            {
                robj[key] = REDIS_LIST;
                my_list_size++;
                size++;
                std::cout << "OK" << std::endl;
            }
            else if(my_list_size - 1 == my_list_object.get_size())
            {
                robj.erase(key);
                my_list_size--;
                size--;
                std::cout << "OK" << std::endl;
            }
            
            break;
        }
        case 11:{
            //hash的数据库
            if(!key_type_real(key, REDIS_HASH))
            {
                break;
            }
            my_hash_object.parser_and_distributor(str);
            if(my_hash_size + 1 == my_hash_object.get_size() )
            {
                robj[key] = REDIS_HASH;
                my_hash_size++;
                size++;
                std::cout << "OK" << std::endl;
            }
            else if(my_hash_size - 1 == my_hash_object.get_size())
            {
                robj.erase(key);
                my_hash_size--;
                size--;
                std::cout << "OK" << std::endl;
            }
            
            break;
        }
        case 12:{
            //set的数据库
            if(!key_type_real(key, REDIS_SET))
            {
                break;
            }
            my_set_object.parser_and_distributor(str);
            if(my_set_size + 1 == my_set_object.get_size() )
            {
                robj[key] = REDIS_SET;
                my_set_size++;
                size++;
                std::cout << "OK" << std::endl;
            }
            else if(my_set_size - 1 == my_set_object.get_size())
            {
                robj.erase(key);
                my_set_size--;
                size--;
                std::cout << "OK" << std::endl;
            }
            break;
        }
        case 13:{
            //zset的数据库
            if(!key_type_real(key, REDIS_ZSET))
            {
                break;
            }
            my_zset_object.parser_and_distributor(str);
            if(my_zset_size + 1 == my_zset_object.get_size())
            {
                robj[key] = REDIS_ZSET;
                my_zset_size++;
                size++;
                std::cout << "OK" << std::endl;
            }
            else if(my_zset_size - 1 == my_zset_object.get_size())
            {
                robj.erase(key);
                my_zset_size--;
                size--;
                std::cout << "OK" << std::endl;
            }
            break;
        }
        default:
            std::cout << "输入错误 请重新输入" << std::endl;
            break;
    }

}
//分析字符串
int my_robj::parse_string(const std::string &str, std::string& key)
{
    
    if(str == SQL_KEYS)//keys *
    {
        return 1;
    }
    
    else if(str == SQL_CLEAR_ALL)//flushdb
    {
        return 3;
    }
    else if(str == SQL_ERGODIC)//ergodic_all
    {
        return 4;
    }
    else if(str == SQL_DBSIZE)//dbsize
    {
        return 5;
    }
    else if(str == SQL_RDB_SAVE)//rdb_save
    {
        return 6;
    }
    else if(str == SQL_RDB_LOAD)//rdb_load 
    {
        return 7;
    }
    return parse_tosql_string(str, key);
    
}
//解析分配给数据库的字符串
int my_robj::parse_tosql_string(const std::string &str, std::string& key)
{
    std::string action;
    int p1 = str.find(' ');
    action = str.substr(0, p1);

    int p2 = str.find(' ', p1 + 1);
    if(p2 == -1)
    {
        key = str.substr(p1 + 1, str.length());
        
    }
    else
    {
        key = str.substr(p1 + 1, p2 - p1 - 1);
    }
    if(action == SQL_TYPE_KEY)
    {
        return 2;
    }
    else if(action == SQL_DEL) //del 
    {
        return 8;
    }
    else if(action == SQL_SET || action == SQL_GET)//string action
    {
        return 9;
    }
    //list action
    else if (action == SQL_LPUSH || action == SQL_RPUSH || action == SQL_LPOP || action == SQL_RPOP || action == SQL_LINDEX || action == SQL_LLEN || action == SQL_LRANGE)
    {
        return 10;
    }
    //hash action
    else if (action == SQL_HSET || action == SQL_HDEL || action == SQL_HEXISTS || action == SQL_HLEN || action == SQL_HGETALL || action == SQL_HKEYS || action == SQL_HVALS)
    {
        return 11;
    }
    //set action
    else if (action == SQL_SADD || action == SQL_SREM || action == SQL_SISMEMBER || action == SQL_SCARD || action == SQL_SMEMBERS)
    {
        return 12;
    }
    //zset action
    else if (action == SQL_ZADD || action == SQL_ZREM || action == SQL_ZSCORE || action == SQL_ZCARD || action == SQL_ZRANGE)
    {
        return 13;
    }
    return -1;
}

// //每次执行给其他数据库的命令后检查size是否改变 如果改变就需要改变robj的对应键
// bool my_robj::change_robj(std::string& key)
// {

// }


//每次进行其他数据库操作前 对robj访问 查看是否是对应的key
bool my_robj::key_type_real(std::string& key, SQL_TYPE sql_type)
{
    if(robj.find(key) == robj.end())
    {
        return true;
    }
    if(robj[key] == sql_type)
    {
        return true;
    }
    std::cout << key << "的类型是:" << robj[key] << "不是 " << sql_type << std::endl;
    return false;
}

//删除
bool my_robj::delete_key(const std::string& str, const std::string &key)
{
    if(robj.find(key) == robj.end())
    {
        std::cout << "未设置该键 无法删除" << std::endl;
        return false;
    }
    SQL_TYPE sql_type;
    sql_type = robj[key];
    robj.erase(key);
    if(sql_type == REDIS_STRING)
    {
        return my_string_object.parser_and_distributor(str);
    }
    else if(sql_type == REDIS_LIST)
    {
        return my_list_object.parser_and_distributor(str);
    }
    else if(sql_type == REDIS_HASH)
    {
        return my_hash_object.parser_and_distributor(str);
    }
    else if(sql_type == REDIS_SET)
    {
        return my_set_object.parser_and_distributor(str);
    }
    else if(sql_type == REDIS_ZSET)
    {
        return my_zset_object.parser_and_distributor(str);
    }
    return false;
}

//遍历 要6种
bool my_robj::ergodic_all()
{
    my_string_object.parser_and_distributor(SQL_ERGODIC);
    my_list_object.parser_and_distributor(SQL_ERGODIC);
    my_hash_object.parser_and_distributor(SQL_ERGODIC);
    my_set_object.parser_and_distributor(SQL_ERGODIC);
    my_zset_object.parser_and_distributor(SQL_ERGODIC);
    return true;
}

// keys *
//查看当前库所有的key
bool my_robj::ergodic_key()
{
    for(auto& key : robj)
    {
        std::cout << "\"" << key.first << "\"" << std::endl; 
    }
    return true;
}

// type key
//返回值的类型  感觉会很实用
bool my_robj::type_key(const std::string &key)
{
    if(robj.find(key) == robj.end())
    {
        std::cout << key << "未设置" << std::endl;
        return false;
    }
    std::cout << robj[key] << std::endl;
    return true;
}

// flushdb //清空当前库
bool my_robj::clear()
{
    robj.clear();
    my_string_object.parser_and_distributor(SQL_CLEAR);
    my_list_object.parser_and_distributor(SQL_CLEAR);
    my_hash_object.parser_and_distributor(SQL_CLEAR);
    my_set_object.parser_and_distributor(SQL_CLEAR);
    my_zset_object.parser_and_distributor(SQL_CLEAR);
    
    return true;
}

//保存键的总数 也要6种
// dbsize 查看当前数据库的key数量
int my_robj::get_size()
{
    std::cout << size << std::endl;
    return size;
}

// RBD持久化
//保存
bool my_robj::rdb_save()
{
    my_string_object.parser_and_distributor(SQL_RDB_SAVE);
    my_list_object.parser_and_distributor(SQL_RDB_SAVE);
    my_hash_object.parser_and_distributor(SQL_RDB_SAVE);
    my_set_object.parser_and_distributor(SQL_RDB_SAVE);
    
    rdbfile_save.open(RDB_FILE_ROBJ);
    rdbfile_save.clear();
    rdbfile_save.close();
    my_zset_object.parser_and_distributor(SQL_RDB_SAVE);
    return true;
}
//载入
bool my_robj::rdb_load()
{
    my_string_object.parser_and_distributor(SQL_RDB_LOAD);
    my_list_object.parser_and_distributor(SQL_RDB_LOAD);
    my_hash_object.parser_and_distributor(SQL_RDB_LOAD);
    my_set_object.parser_and_distributor(SQL_RDB_LOAD);
    my_zset_object.parser_and_distributor(SQL_RDB_LOAD);

    rdb_load_robj();
    return true;
}

//解决RDB载入后 robj的记录无法登记类型的问题
bool my_robj::rdb_load_robj()
{
    std::vector<std::string> temp;
    temp = my_string_object.robj_register();
    for(auto & object : temp)
    {
        robj[object] = REDIS_STRING;
    }
    temp = my_list_object.robj_register();
    for(auto & object : temp)
    {
        robj[object] = REDIS_LIST;
    }
    temp = my_hash_object.robj_register();
    for(auto & object : temp)
    {
        robj[object] = REDIS_HASH;
    }
    temp = my_set_object.robj_register();
    for(auto & object : temp)
    {
        robj[object] = REDIS_SET;
    }
    temp = my_zset_object.robj_register();
    for(auto & object : temp)
    {
        robj[object] = REDIS_ZSET;
    }
    return true;
}
#include <iostream>
#include "../my_robj/my_robj.h"
#include <string.h>
int main()
{
    my_robj robj;
    
    //字符串test
    std::cout << "-------------------字符串类型操作-------------------------" << std::endl;
    std::string str1 = "set q q";
    std::string str2 = "set q2 q2";
    std::string str3 = "get q";
    std::string str4 = "del q";
    
    std::cout <<"下面的操作是:: " << str1 << std::endl;
    robj.parser_and_distributor(str1);
    std::cout <<"下面的操作是:: " << str2 << std::endl;
    robj.parser_and_distributor(str2);
    std::cout <<"下面的操作是:: " << str3 << std::endl;
    robj.parser_and_distributor(str3);
    std::cout <<"下面的操作是:: " << str4 << std::endl;
    robj.parser_and_distributor(str4);



    //list test
    std::cout << "-------------------列表类型操作-------------------------" << std::endl;


    std::string lstr1 = "lpush q q";
    std::string lstr2 = "rpush q q2";
    std::string lstr3 = "rpush q q2";
    std::string lstr4 = "rpush q q2";

    std::string lstr5 = "lpop q";
    std::string lstr6 = "lindex q 1";
    std::string lstr7 = "rpop q";
    std::string lstr8 = "llen q";

    std::string lstr9 = "lrange q";
    std::cout <<"下面的操作是:: " << lstr1 << std::endl;
    robj.parser_and_distributor(lstr1);
    std::cout <<"下面的操作是:: " << lstr2 << std::endl;
    robj.parser_and_distributor(lstr2);
    std::cout <<"下面的操作是:: " << lstr3 << std::endl;
    robj.parser_and_distributor(lstr3);
    std::cout <<"下面的操作是:: " << lstr4 << std::endl;
    robj.parser_and_distributor(lstr4);
    std::cout <<"下面的操作是:: " << lstr5 << std::endl;
    robj.parser_and_distributor(lstr5);
    std::cout <<"下面的操作是:: " << lstr6 << std::endl;
    robj.parser_and_distributor(lstr6);
    std::cout <<"下面的操作是:: " << lstr7 << std::endl;
    robj.parser_and_distributor(lstr7);
    std::cout <<"下面的操作是:: " << lstr8 << std::endl;
    robj.parser_and_distributor(lstr8);
    std::cout <<"下面的操作是:: " << lstr9 << std::endl;
    robj.parser_and_distributor(lstr9);






    //hash test
    std::cout << "-------------------哈希类型操作-------------------------" << std::endl;
    std::string hstr1 = "hset q3 q q1";
    std::string hstr2 = "hset q3 q1 q2";
    std::string hstr3 = "hset q3 q2 q3";
    std::string hstr4 = "hset q3 q3 q4";

    std::string hstr5 = "hdel q3 q1";
    std::string hstr6 = "hexists q3 q";
    std::string hstr7 = "hlen q3";

    std::string hstr8 = "hgetall q3";
    std::string hstr9 = "hkeys q3";
    std::string hstr10 = "hvals q3";

    std::cout <<"下面的操作是:: " << hstr1 << std::endl;
    robj.parser_and_distributor(hstr1);
    std::cout <<"下面的操作是:: " << hstr2 << std::endl;
    robj.parser_and_distributor(hstr2);
    std::cout <<"下面的操作是:: " << hstr3 << std::endl;
    robj.parser_and_distributor(hstr3);
    std::cout <<"下面的操作是:: " << hstr4 << std::endl;
    robj.parser_and_distributor(hstr4);
    std::cout <<"下面的操作是:: " << hstr5 << std::endl;
    robj.parser_and_distributor(hstr5);
    std::cout <<"下面的操作是:: " << hstr6 << std::endl;
    robj.parser_and_distributor(hstr6);
    std::cout <<"下面的操作是:: " << hstr7 << std::endl;
    robj.parser_and_distributor(hstr7);
    std::cout <<"下面的操作是:: " << hstr8 << std::endl;
    robj.parser_and_distributor(hstr8);
    std::cout <<"下面的操作是:: " << hstr9 << std::endl;
    robj.parser_and_distributor(hstr9);
    std::cout <<"下面的操作是:: " << hstr10 << std::endl;
    robj.parser_and_distributor(hstr10);




    //set test
    std::cout << "-------------------集合类型操作-------------------------" << std::endl;
    std::string sstr1 = "sadd q4 q";
    std::string sstr2 = "sadd q4 q1";
    std::string sstr3 = "sadd q4 q2";
    std::string sstr4 = "sadd q4 q3";

    std::string sstr5 = "srem q4 q1";
    std::string sstr6 = "sismember q4 q";
    std::string sstr7 = "scard q4";

    std::string sstr8 = "smembers q4";


    std::cout <<"下面的操作是:: " << sstr1 << std::endl;
    robj.parser_and_distributor(sstr1);
    std::cout <<"下面的操作是:: " << sstr2 << std::endl;
    robj.parser_and_distributor(sstr2);
    std::cout <<"下面的操作是:: " << sstr3 << std::endl;
    robj.parser_and_distributor(sstr3);
    std::cout <<"下面的操作是:: " << sstr4 << std::endl;
    robj.parser_and_distributor(sstr4);
    std::cout <<"下面的操作是:: " << sstr5 << std::endl;
    robj.parser_and_distributor(sstr5);
    std::cout <<"下面的操作是:: " << sstr6 << std::endl;
    robj.parser_and_distributor(sstr6);
    std::cout <<"下面的操作是:: " << sstr7 << std::endl;
    robj.parser_and_distributor(sstr7);
    std::cout <<"下面的操作是:: " << sstr8 << std::endl;
    robj.parser_and_distributor(sstr8);


    //zset test
    std::cout << "-------------------有序集合类型操作-------------------------" << std::endl;
    std::string zstr1 = "zadd q5 1 q";
    std::string zstr2 = "zadd q5 2 q1";
    std::string zstr3 = "zadd q5 3 q2";
    std::string zstr4 = "zadd q5 4 q3";

    std::string zstr5 = "zrem q5 q1";
    std::string zstr6 = "zscore q5 q";
    std::string zstr7 = "zcard q5";

    std::string zstr8 = "zrange q5";

    std::cout <<"下面的操作是:: " << zstr1 << std::endl;
    robj.parser_and_distributor(zstr1);
    std::cout <<"下面的操作是:: " << zstr2 << std::endl;
    robj.parser_and_distributor(zstr2);
    std::cout <<"下面的操作是:: " << zstr3 << std::endl;
    robj.parser_and_distributor(zstr3);
    std::cout <<"下面的操作是:: " << zstr4 << std::endl;
    robj.parser_and_distributor(zstr4);
    std::cout <<"下面的操作是:: " << zstr5 << std::endl;
    robj.parser_and_distributor(zstr5);
    std::cout <<"下面的操作是:: " << zstr6 << std::endl;
    robj.parser_and_distributor(zstr6);
    std::cout <<"下面的操作是:: " << zstr7 << std::endl;
    robj.parser_and_distributor(zstr7);
    std::cout <<"下面的操作是:: " << zstr8 << std::endl;
    robj.parser_and_distributor(zstr8); 
    
    //统一操作
    std::cout << "-------------------统一类型操作-------------------------" << std::endl;
    std::string astr1 = "keys *";
    std::string astr2 = "type q5";
    std::string astr3 = "dbsize";
    std::string astr4 = "ergodic_all";

    std::string astr5 = "rdb_save";
    std::string astr6 = "flushdb";
    std::string astr7 = "rdb_load";
    
    std::string astr8 = "del q";
    std::string astr9 = "keys *";

    std::cout <<"下面的操作是:: " << astr1 << std::endl;
    robj.parser_and_distributor(astr1);
    std::cout <<"下面的操作是:: " << astr2 << std::endl;
    robj.parser_and_distributor(astr2);
    std::cout <<"下面的操作是:: " << astr3 << std::endl;
    robj.parser_and_distributor(astr3); 
    std::cout <<"下面的操作是:: " << astr4 << std::endl;
    robj.parser_and_distributor(astr4);
    std::cout <<"下面的操作是:: " << astr5 << std::endl;
    robj.parser_and_distributor(astr5);
    std::cout <<"下面的操作是:: " << astr6 << std::endl;
    robj.parser_and_distributor(astr6);
    std::cout <<"下面的操作是:: " << astr7 << std::endl;
    robj.parser_and_distributor(astr7);
    std::cout <<"下面的操作是:: " << astr8 << std::endl;
    robj.parser_and_distributor(astr8);
    std::cout <<"下面的操作是:: " << astr9 << std::endl;
    robj.parser_and_distributor(astr9);
    
    return 0;
}
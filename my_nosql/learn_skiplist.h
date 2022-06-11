/* ************************************************************************
> File Name:     skiplist.h
> Author:        程序员Carl
> 微信公众号:    代码随想录
> Created Time:  Sun Dec  2 19:04:26 2018
> Description:   
 ************************************************************************/

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define STORE_FILE "store/dumpFile"

std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

//Class template to implement node
//类模板来实现节点
template<typename K, typename V> 
class Node {

public:
    
    Node() {} 

    Node(K k, V v, int); 

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);
    
    // Linear array to hold pointers to next node of different level

    //用于保存指向不同级别下一个节点的指针的线性数组
    Node<K, V> **forward;

    int node_level;

private:
    K ele;
    V value;
};

template<typename K, typename V> 
Node<K, V>::Node(const K k, const V v, int level) {
    this->ele = k;
    this->value = v;
    this->node_level = level; 

    // level + 1, because array index is from 0 - level
    ////级别+1，因为数组索引是从0级别开始的
    this->forward = new Node<K, V>*[level+1];
    
	// Fill forward array with 0(NULL) 
    //用0（空）填充前向数组
    memset(this->forward, 0, sizeof(Node<K, V>*)*(level+1));
};

template<typename K, typename V> 
Node<K, V>::~Node() {
    delete []forward;
};

template<typename K, typename V> 
K Node<K, V>::get_key() const {
    return ele;
};

template<typename K, typename V> 
V Node<K, V>::get_value() const {
    return value;
};
template<typename K, typename V> 
void Node<K, V>::set_value(V value) {
    this->value=value;
};

// Class template for Skip list
//跳表的类模板
template <typename K, typename V> 
class SkipList {

public: 
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);

    //持久化
    //保存到本地
    void dump_file();
    //从本地读取
    void load_file();
    int size();

private:
    //将str分为key和value
    void get_key_value_from_string(const std::string& str, std::string* ele, std::string* value);
    //判断str是否符合:格式
    bool is_valid_string(const std::string& str);

private:    
    // Maximum level of the skip list 
    ////跳过列表的最大级别
    int _max_level;

    // current level of skip list 
    //跳过列表的当前级别
    int _skip_list_level;

    // pointer to header node 
    //指向头节点的指针
    Node<K, V> *_header;

    // file operator
    //文件操作员
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // skiplist current element count
    ////skiplist当前元素计数
    int _element_count;
};

// create new node 
//创建新节点
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

// Insert given ele and value in skip list 
// return 1 means element exists  
// return 0 means insert successfully
/* 
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template<typename K, typename V>
int SkipList<K, V>::insert_element(const K ele, const V value) {
    
    mtx.lock();
    Node<K, V> *current = this->_header;

    // create update array and initialize it 
    // update is array which put node that the node->forward[i] should be operated later
    //创建更新数组并初始化它
    //update是一个数组，该数组将放置节点，节点->前进[i]应该在以后操作
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));  

    // start form highest level of skip list 
    //从最高级别的跳过列表开始
    for(int i = _skip_list_level; i >= 0; i--) {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < ele) {
            current = current->forward[i]; 
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right node, which is desired to insert ele.
    ////已达到0级，并将指针向前指向右侧节点，该节点需要插入密钥。
    current = current->forward[0];

    // if current node have ele equal to searched ele, we get it
    ////如果当前节点的密钥等于搜索的密钥，我们就得到它
    if (current != NULL && current->get_key() == ele) {
        std::cout << "ele: " << ele << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    // if current is NULL that means we have reached to end of the level 
    // if current's ele is not equal to ele that means we have to insert node between update[0] and current node 
    //若电流为空，这意味着我们已经达到了水平的终点
    //如果current的key不等于ele，这意味着我们必须在update[0]和current节点之间插入节点
    
    if (current == NULL || current->get_key() != ele ) {
        
        // Generate a random level for node
        //为节点生成随机级别
        int random_level = get_random_level();

        // If random level is greater thar skip list's current level, initialize update value with pointer to header
        //如果随机级别大于跳过列表的当前级别，则使用指向标题的指针初始化更新值
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // create new node with random level generated 
        //创建新节点，生成随机级别
        Node<K, V>* inserted_node = create_node(ele, value, random_level);
        
        // insert node 
        //插入节点
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted ele:" << ele << ", value:" << value << std::endl;
        _element_count ++;
    }
    mtx.unlock();
    return 0;
}

// Display skip list 
//显示跳过列表
template<typename K, typename V> 
void SkipList<K, V>::display_list() {

    std::cout << "\n*****Skip List*****"<<"\n"; 
    for (int i = 0; i <= _skip_list_level; i++) {
        Node<K, V> *node = this->_header->forward[i]; 
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// Dump data in memory to file 
//将内存中的数据转储到文件
template<typename K, typename V> 
void SkipList<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0]; 

    while (node != NULL) {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return ;
}

// Load data from disk
//从磁盘加载数据
template<typename K, typename V> 
void SkipList<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* ele = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, ele, value);
        if (ele->empty() || value->empty()) {
            continue;
        }
        insert_element(*ele, *value);
        
        std::cout << "ele:" << *ele << "value:" << *value << std::endl;
    }
    _file_reader.close();
}

// Get current SkipList size 
//获取当前SkipList大小
template<typename K, typename V> 
int SkipList<K, V>::size() { 
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* ele, std::string* value) {

    if(!is_valid_string(str)) {
        return;
    }
    *ele = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

// Delete element from skip list 
//从跳过列表中删除元素
template<typename K, typename V> 
void SkipList<K, V>::delete_element(K ele) {

    mtx.lock();
    Node<K, V> *current = this->_header; 
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    // start from highest level of skip list
    //从跳过列表的最高级别开始
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] !=NULL && current->forward[i]->get_key() < ele) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != NULL && current->get_key() == ele) {
       
        // start for lowest level and delete the current node of each level
        //从最低级别开始，删除每个级别的当前节点
        for (int i = 0; i <= _skip_list_level; i++) {

            // if at level i, next node is not target node, break the loop.
            //如果在i级，下一个节点不是目标节点，则中断循环。
            if (update[i]->forward[i] != current) 
                break;

            update[i]->forward[i] = current->forward[i];
        }

        // Remove levels which have no elements
        //删除没有元素的级别
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
            _skip_list_level --; 
        }

        std::cout << "Successfully deleted ele "<< ele << std::endl;
        _element_count --;
    }
    mtx.unlock();
    return;
}

// Search for element in skip list 
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V> 
bool SkipList<K, V>::search_element(K ele) {

    std::cout << "search_element-----------------" << std::endl;
    Node<K, V> *current = _header;

    // start from highest level of skip list
    //从跳过列表的最高级别开始
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < ele) {
            current = current->forward[i];
        }
    }

    //reached level 0 and advance pointer to right node, which we search
    //达到0级并将指针前进到右侧节点，我们将搜索该节点
    current = current->forward[0];

    // if current node have ele equal to searched ele, we get it
    //如果当前节点的密钥等于搜索的密钥，我们就得到它
    if (current and current->get_key() == ele) {
        std::cout << "Found ele: " << ele << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << ele << std::endl;
    return false;
}

// construct skip list
//构造跳过列表
template<typename K, typename V> 
SkipList<K, V>::SkipList(int max_level) {

    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    // create header node and initialize ele and value to null
    //创建头节点并将键和值初始化为null
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
};

template<typename K, typename V> 
SkipList<K, V>::~SkipList() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level(){

    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120

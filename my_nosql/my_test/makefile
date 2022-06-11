CXX ?= g++
CXXFLAGS = -std=c++0x
CFLAGS = -I
my_test:my_test.cpp ../my_robj/my_robj.cpp ../my_string/my_string.cpp ../my_skiplist/my_zskiplist.cpp ../my_list/my_list.cpp ../my_hash/my_hash.cpp ../my_set/my_set.cpp ../my_zset/my_zset.cpp
	$(CXX) -o my_test  $^   --std=c++11 -pthread 

clean: 
	rm -f my_test
game:*.cpp
	g++ -pthread -std=c++11 -g -Wall $^ -o $@ -lzinx -lpthread -lprotobuf -lhiredis 

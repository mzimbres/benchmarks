
boost_include=/opt/boost_dev/include/

.PHONY: clean
all: read-until read-until-cobalt echo-server

read-until: read-until.cpp
	g++ -std=c++20 $@.cpp -I ${boost_include} -O2 -o $@

read-until-cobalt: read-until-cobalt.cpp
	g++ -std=c++20 $@.cpp -I ${boost_include} -I ~/Public/cobalt/include/ -O2 ~/mess/cobalt_build/libboost_cobalt.a -o $@

echo-server: echo-server.cpp
	g++ -std=c++20 $@.cpp -I ${boost_include} -O2 -o $@

.PHONY: clean
clean:
	rm -f read-until read-until-cobalt echo-server

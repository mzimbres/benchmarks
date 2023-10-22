# benchmarks

```
g++ -std=c++20 read-until.cpp -I /opt/boost_dev/include/ -O2
g++ -std=c++20 read-until-cobalt.cpp -I /opt/boost_dev/include/ -I ~/Public/cobalt/include/ -O2 ~/mess/cobalt_build/libboost_cobalt.a
```

Run a tcp server in another terminal

```
watch -n0 nc -C -l -p  12345
```

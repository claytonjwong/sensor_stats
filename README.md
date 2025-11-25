```
➜  sensor_stats git:(main) ✗ mkdir build
➜  sensor_stats git:(main) ✗ cd build
➜  build git:(main) ✗ cmake .. && make
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.7s)
-- Generating done (0.0s)
-- Build files have been written to: /home/clayton/sandbox/sensor_stats/build
[ 50%] Building CXX object CMakeFiles/sensor_stats.dir/main.cc.o
[100%] Linking CXX executable sensor_stats
[100%] Built target sensor_stats
➜  build git:(main) ✗ ./sensor_stats
process(0, 2, 4)
[2..4)

process(1, 3, 5)
[2..5)

process(0, 6, 12)
[2..5)
[6..12)

process(1, 7, 10)
[2..5)
[6..12)

process(2, 0, 20)
[0..20)
```

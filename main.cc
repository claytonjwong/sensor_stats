// # Imaging that you're running a system of IR sensors that monitor the region.
// Each of these sensors can be active or inactive. You receive an unordered stream
// of reports from these sensors when they were active with entries (sensor_id, ts1, ts2).
// The region these sensors are monitoring is considered covered when at least one sensor is active,
// and multiple sensors can be active at the same time. As these reports arrive, we want to be able
// to compute a total amount of time this region was covered.
// Implement two functions process(sensor_id, ts1, ts2) and total_time() that can give us this answer.

// # Example:

// # process(0, 2, 4)
// # process(1, 3, 5)
// # total_time() # prints 3
// # process(0, 6, 12)
// # total_time() # prints 9
// # process(1, 7, 10)
// # total_time() # prints 9
// process(2, 0, 20)

/*

ts1
ts2

process(0, 2, 4)

| x | x | x | | |
2   3   4   5
2, 3, 4

  234
0123456789


process(0, 6, 12)

ts1 inclusive
ts2 non-inclusive

6 7 8 9 10 11

3 + 6 = 9



TODO: maybe a part 2 will use the sensor_id

brute-force set based solution is NOT efficient enough

optimization would be to keep track of intervals

and keep them sorted by the start

when overlapping intervals are observed, we can merge them together

process(x, 1, 2)
process(x, 1, 2)
process(x, 2, 3)

[1..3)

pair<Time, Time>

set<pair<Time, Time>>

easy path may be using a map instead

map<Time, Time> <--- key is the start of the interval (inclusive)
                <--- value is the maximum end of the interval (non-inclusive)


*/

// #include <set>

// using namespace std;

// class SensorStats {
// public:
//     using SensorId = uint32_t;
//     using Time = uint64_t;

//     /**
//      * doc string info here
//      */
//     void process(SensorId sid, Time start, Time end);

//     /**
//      * @return Time
//      */
//     Time total_time() const {

//     }

// private:
//     using Pair = pair<Time, Time>;

//     struct Comp {
//         bool operator()(const Pair& a, const Pair& b) const {
//             if (a.first == b.first) {
//                 return a.second < b.second;
//             }
//             return a.first < b.first;
//         }
//     }

//     set<Time, Comp> time;
// };


/*

[1...4)

x x x
1 2 3 4


[2...5)

x x x x
1 2 3 4 5

helper function to determine if the intervals are overlapping or not

*/


#include <cassert>
#include <cmath>
#include <iostream>
#include <map>

using namespace std; // pollutes namespace, ok for practice, but NOT production code

class SensorStats {
public:
    using SensorId = uint32_t;
    using Time = uint64_t;

    /**
     * doc string info here
     */
    void process(SensorId sid, Time start, Time end) {

        // always insert into the multimap
        m.insert(make_pair(start, end));
        bool found = false;

        // check for overlap with the element BEFORE start
        do {
            found = false;
            auto it = m.lower_bound(start);
            if (it != m.begin() && it != m.end()) {
                auto [prev_start, prev_end] = *prev(it);
                found = merge(start, end, prev_start, prev_end);
                if (found) {
                    start = min(start, prev_start);
                    end = max(end, prev_end);
                }
            }
        } while (found);

        // check for overlap with the element AFTER start
        do {
            found = false;
            auto it = m.lower_bound(start);
            if (it != m.end() && next(it) != m.end()) {
                auto [next_start, next_end] = *next(it);
                found = merge(start, end, next_start, next_end);
                if (found) {
                    start = min(start, next_start);
                    end = max(end, next_end);
                }
            }
        } while (found);
    }

    /**
     * @return Time via a linear scan of the map m
     */
    Time total_time() const {
        Time total = 0;
        for (auto [start, end]: m) {
            total += end - start;
        }
        return total;
    }

    void pretty_print() const {
        for (auto [start, end]: m) {
            cout << "[" << start << ".." << end << ")" << endl;
        }
        cout << endl;
    }

private:
    multimap<Time, Time> m;

    /**
     * return true if-and-only-if the start/end times of the first interval
     * overlap with the start/end times of the second interval
     *
     * note: the start times are inclusive and the end times are non-inclusive
     */
    bool is_overlapping(Time first_start, Time first_end, Time second_start, Time second_end) const {
        // for simplicity, subtract 1 to make endpoints inclusive, this allows us to always use <= comparisons
        --first_end;
        --second_end;

        // check if second start/end is within first start/end
        if (first_start <= second_start && second_start <= first_end) return true;
        if (first_start <= second_end && second_end <= first_end) return true;

        // check if first start/end is within second start/end
        if (second_start <= first_start && first_start <= second_end) return true;
        if (second_start <= first_end && first_end <= second_end) return true;

        // NOT overlapping
        return false;
    }

    /**
     * return true if-and-only-if the intervals are overlapping and have been merged
     *
     * note: the start times are inclusive and the end times are non-inclusive
     */
    bool merge(Time first_start, Time first_end, Time second_start, Time second_end) {
        // if the intervals are NOT overlapping, do NOT merge, return false immediately
        if (!is_overlapping(first_start, first_end, second_start, second_end)) {
            return false;
        }

        m.erase(m.lower_bound(first_start));   // erase the first start/end time
        m.erase(m.lower_bound(second_start));  // erase the second start/end time

        // insert the merged interval which is the minimum start time and maximum end time
        m.insert(make_pair(min(first_start, second_start), max(first_end, second_end)));
        return true;
    }
};

int main() {
    SensorStats ss;

    /*
     * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
     *       |--|
     */
    cout << "process(0, 2, 4)" << endl;
    ss.process(0, 2, 4);
    assert(ss.total_time() == 2);
    ss.pretty_print();

    /*
     * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
     *       |-----|
     */
    cout << "process(1, 3, 5)" << endl;
    ss.process(1, 3 ,5);
    assert(ss.total_time() == 3);
    ss.pretty_print();

    /*
     * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
     *       |-----|     |--------------|
     */
    cout << "process(0, 6, 12)" << endl;
    ss.process(0, 6, 12);
    assert(ss.total_time() == 9);
    ss.pretty_print();

    /*
     * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
     *       |-----|     |--------------|
     */
    cout << "process(1, 7, 10)" << endl;
    ss.process(1, 7, 10);
    assert(ss.total_time() == 9);
    ss.pretty_print();

    /*
     * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
     * |--------------------------------------------------------|
     */
    cout << "process(2, 0, 20)" << endl;
    ss.process(2, 0, 20);
    assert(ss.total_time() == 20);
    ss.pretty_print();

    return 0;
}

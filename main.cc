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
    struct Interval {
        Interval(Time start, Time end) : start{ start }, end{ end } {}
        const Time start;
        const Time end;
    };

    /**
     * doc string info here
     */
    void process(SensorId sid, Time start, Time end) {
        if (end < start) {
            return; // do NOT process invalid input!  TODO: error handling -- decide if we should assert, throw exception, etc.
        }

        // always insert into the multimap
        m.emplace(start, end);

        // add the duration onto the running total time
        total += end - start;

        // check for overlap with the element BEFORE start
        bool found = false;
        do {
            found = false;
            const auto it = m.lower_bound(start);
            if (it != m.begin() && it != m.end()) {
                const auto [prev_start, prev_end] = *prev(it);
                found = merge(Interval{start, end}, Interval{prev_start, prev_end});
                if (found) {
                    start = min(start, prev_start);
                    end = max(end, prev_end);
                }
            }
        } while (found);

        // check for overlap with the element AFTER start
        do {
            found = false;
            const auto it = m.lower_bound(start);
            if (it != m.end() && next(it) != m.end()) {
                const auto [next_start, next_end] = *next(it);
                found = merge(Interval{start, end}, Interval{next_start, next_end});
                if (found) {
                    start = min(start, next_start);
                    end = max(end, next_end);
                }
            }
        } while (found);
    }

    /**
     * @return total time in O(1) since this is a running total which is updated via the process() function
     */
    Time total_time() const {
        return total;
    }

    void pretty_print() const {
        for (const auto [start, end]: m) {
            cout << "[" << start << ".." << end << ")" << endl;
        }
        cout << endl;
    }

private:
    multimap<Time, Time> m;
    Time total = 0;

    /**
     * return true if-and-only-if the start/end times of the first interval
     * overlap with the start/end times of the second interval
     *
     * note: the start times are inclusive and the end times are non-inclusive
     */
    bool is_overlapping(const Interval& first, const Interval& second) const {
        if (first.end == second.start || second.end == first.start) {
            return true; // corner case, adjacent intervals are NOT strictly overlapping, but they still should be merged, return true!
        }

        // check if second start/end is within first start/end
        if (first.start <= second.start && second.start <= first.end) return true;
        if (first.start <= second.end && second.end <= first.end) return true;

        // check if first start/end is within second start/end
        if (second.start <= first.start && first.start <= second.end) return true;
        if (second.start <= first.end && first.end <= second.end) return true;

        // NOT overlapping
        return false;
    }

    /**
     * return true if-and-only-if the intervals are overlapping and have been merged
     *
     * note: the start times are inclusive and the end times are non-inclusive
     */
    bool merge(const Interval&& first, const Interval&& second) {
        // if the intervals are NOT overlapping, do NOT merge, return false immediately
        if (!is_overlapping(first, second)) {
            return false;
        }

        // erase the first start/end time and subtract the duration from the running total time
        {
            const auto it = m.lower_bound(first.start);
            const auto [start, end] = *it;
            total -= end - start;
            m.erase(it);
        }

        // erase the second start/end time and subtract the duration from the running total time
        {
            const auto it = m.lower_bound(second.start);
            const auto [start, end] = *it;
            total -= end - start;
            m.erase(it);
        }

        // insert the merged interval which is the minimum start time and maximum end time
        // and add the duration onto the running total time
        {
            const auto start = min(first.start, second.start);
            const auto end = max(first.end, second.end);
            total += end - start;
            m.emplace(start, end);
        }

        return true;
    }
};

int main() {

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * original test cases
     */
    {
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
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * test adjacent intervals [0..5) and [5..10) which should be merged as [0..10)
     * even though the inverals are NOT strictly overlapping
     */
    {
        SensorStats ss;
        /*
        * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
        * |-----------|
        */
        ss.process(0, 0, 5);
        assert(ss.total_time() == 5);
        ss.pretty_print();

        /*
        * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
        * |--------------------------|
        */
        ss.process(0, 5, 10);
        assert(ss.total_time() == 10);
        ss.pretty_print();
    }



    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * test overlap on both sides, requiring multi-step overlap detection
     */
    {
        SensorStats ss;
        /*
        * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
        *                |-----------|
        *                                     |-----|
        *                            |-----------|
        */
        ss.process(0, 5, 10);
        ss.process(0, 12, 15);
        ss.process(0, 9, 14);
        /*
        * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
        *                |--------------------------|
        */
        ss.pretty_print();
        assert(ss.total_time() == 10);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * test overlap on both sides, requiring multi-step overlap detection
     */
    {
        SensorStats ss;
        /*
        * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
        * |-----------|                 |-----------|
        *          |--------------------------|
        */
        ss.process(0, 0, 5);
        ss.process(0, 10, 15);
        ss.process(0, 3, 13);
        /*
        * 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
        * |-----------------------------------------|
        */
        ss.pretty_print();
        assert(ss.total_time() == 15);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * do NOT process invalid input!
    */
    {
        SensorStats ss;
        ss.process(0, 10, 0);
        ss.process(0, 100, 10);
        ss.process(0, 234, 23);
        ss.pretty_print();
        assert(ss.total_time() == 0);
    }

    return 0;
}

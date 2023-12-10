#ifndef ELVEN_MEASURE
#define ELVEN_MEASURE

#include <string>

namespace ElvenMeasure {
    template <class F> 
    auto execute(const F& f, const std::size_t times=1) {
        using namespace std::chrono;

        const auto start_time = high_resolution_clock::now();
        
        auto result = f();
        for(int i = 0; i < times-1; ++i) { f(); }

        const auto end_time = high_resolution_clock::now();

        return std::make_pair(result, (end_time - start_time) / times);
    }

    unsigned long long inline operator"" _μs(const unsigned long long int val) { return val * 1'000; }
    unsigned long long inline operator"" _ms(const unsigned long long int val) { return val * 1'000_μs; }
    unsigned long long inline operator"" _s(const unsigned long long int val) { return val * 1'000_ms; }
    unsigned long long inline operator"" _min(const unsigned long long int val) { return val * 60_s; }
    unsigned long long inline operator"" _h(const unsigned long long int val) { return val * 60_min; }
    unsigned long long inline operator"" _day(const unsigned long long int val) { return val * 24_h; }

    template <class TTime>
    std::string cast_time(const TTime &time) {
        using namespace std::chrono;

        auto time_ns = time.count();

        if(time_ns < 1_μs) { return std::to_string(time_ns) + "ns"; }
        if(time_ns < 1_ms) {
            return std::to_string(time_ns / 1_μs) + "μs";
        }
        if(time_ns < 1_s) {
            return std::to_string(time_ns / 1_ms) + "ms";
        }
        if(time_ns < 1_min) {
            return std::to_string(time_ns / 1_s) + "s";
        }
        if(time_ns < 1_h) {
            return std::to_string(time_ns / 1_min) + "min";
        }
        if(time_ns < 1_day) {
            return std::to_string(time_ns / 1_h) + " hour(s)";
        }
        return std::to_string(time_ns / 1_day) + " day(s)";
    }

    template <class TRes1, class TIoTime, class TRes1Tme>
    auto report(const TRes1 & result, const TIoTime & io_time, const TRes1Tme & solve_time) {
        std::cout << "I/O   : " << cast_time(io_time) << std::endl;
        std::cout << "Solve : " << cast_time(solve_time) << std::endl;
        std::cout << "Solution: " << result << std::endl;
    }
}
#endif
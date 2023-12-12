#include "elven_io.h"
#include "elven_measure.h"

typedef std::tuple<size_t, size_t> race;

auto parse_input(const ElvenIO::input_type &input) {

    std::string time_line = input[0].substr(std::string("Time:").length());
    std::string distance_line = input[1].substr(std::string("Distance::").length());
    time_line.erase(std::remove_if(time_line.begin(), time_line.end(), isspace), time_line.end());
    distance_line.erase(std::remove_if(distance_line.begin(), distance_line.end(), isspace), distance_line.end());

    return race(std::stol(time_line), std::stol(distance_line));
}

size_t search_min_win_position(const size_t &distance, const size_t &time) {
    size_t min_win = distance;
    for (size_t high = time, low = 0, mid = (high + low) / 2; low <= high; mid = (high + low) / 2.0) {
        if (const auto run_distance = (time - mid) * mid; run_distance > distance) {
            min_win = mid;
            high = high == mid ? mid - 1 : mid;
        } else {
            low = low == mid ? mid + 1 : mid;
        }
    }
    return min_win;
}

size_t search_max_win_position(const size_t &distance, const size_t &time) {
    size_t max_win = 0;
    for (size_t high = time, low = 0, mid = (high + low) / 2; low <= high; mid = (high + low) / 2.0) {
        if (const auto run_distance = (time - mid) * mid; run_distance > distance) {
            max_win = mid;
            low = low == mid ? mid + 1 : mid;
        } else {
            high = high == mid ? mid - 1 : mid;
        }
    }
    return max_win;
}

auto solve(const ElvenIO::input_type &input) {
    auto [time, distance] = parse_input(input);
    return search_max_win_position(distance, time) - search_min_win_position(distance, time) + 1;
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}
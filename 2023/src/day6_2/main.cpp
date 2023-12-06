#include <iostream>
#include <fstream>
#include <regex>
#include <__numeric/transform_reduce.h>

typedef std::tuple<size_t, size_t> race;

auto parse_input(const char* filename) {
    std::fstream file(filename);

    std::string time_line;
    std::string distance_line;
    std::getline(file, time_line);
    std::getline(file, distance_line);
    time_line = time_line.substr(std::string("Time:").length());
    distance_line = distance_line.substr(std::string("Distance::").length());
    time_line.erase(std::remove_if(time_line.begin(), time_line.end(), isspace), time_line.end());
    distance_line.erase(std::remove_if(distance_line.begin(), distance_line.end(), isspace), distance_line.end());

    return race(std::stol(time_line), std::stol(distance_line));
}

size_t search_min_win_position(const size_t &distance, const size_t &time) {
    size_t min_win = distance;
    for (size_t high = time, low = 0, mid = (high + low) / 2; low <= high; mid = (high + low) / 2.0) {
        const auto run_distance = (time - mid) * mid;
        if (run_distance > distance) {
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
        const auto run_distance = (time - mid) * mid;
        if (run_distance > distance) {
            max_win = mid;
            low = low == mid ? mid + 1 : mid;
        } else {
            high = high == mid ? mid - 1 : mid;
        }
    }
    return max_win;
}

size_t solve(const race &race_data) {
    auto [time, distance] = race_data;
    return search_max_win_position(distance, time) - search_min_win_position(distance, time) + 1;
}

int main(int _, char** argv) {
    using namespace std::chrono;

    const auto lines = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(lines)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}

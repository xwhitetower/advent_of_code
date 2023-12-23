#include <sstream>
#include <__numeric/transform_reduce.h>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

typedef ElvenUtils::Map Map;
typedef ElvenUtils::Point Point;

constexpr char UP = 'U';
constexpr char DOWN = 'D';
constexpr char RIGHT = 'R';
constexpr char LEFT = 'L';

constexpr char TRENCH = ElvenUtils::Map::BLOCKED;
constexpr char PAINTED = 'X';


auto parse_line(const std::string& line) {
    std::stringstream stream;
    stream << line;
    char direction;
    int count;
    std::string color;
    stream >> direction >> count >> color;
    return std::make_tuple(direction, count);
}

void execute(const char direction, const int count, std::vector<Point>& path, ElvenUtils::Point& position) {
    if (direction == UP) {
        position.y -= count;
        path.push_back(position);
    } else if (direction == DOWN) {
        position.y += count;
        path.push_back(position);
    } else if (direction == RIGHT) {
        position.x += count;
        path.push_back(position);
    } else if (direction == LEFT) {
        position.x -= count;
        path.push_back(position);
    }
}

// https://rosettacode.org/wiki/Shoelace_formula_for_polygonal_area
// https://en.wikipedia.org/wiki/Pick%27s_theorem
std::size_t shoelace_and_picks(const std::vector<Point>& path) {
    long long area = 0;
    long long perifery = 0;
    for (int i = 1; i < path.size(); ++i) {
        area += path[i-1].x * path[i].y - path[i].x * path[i-1].y;
        perifery += std::abs(path[i].x - path[i-1].x + path[i].y - path[i-1].y);
    }
    return std::abs(area) / 2 + perifery  / 2 + 1;
}

std::size_t part1(const ElvenIO::input_type &input) {
    auto position = Point(0, 0);
    std::vector<Point> path;
    path.push_back(position);
    for(const auto &line: input) {
        auto [direction, count] = parse_line(line);
        execute(direction, count, path, position);
    }
    return shoelace_and_picks(path);
}

auto hex_to_int(const std::string& hex_string) {
    unsigned long parsed_value;
    std::stringstream stream;
    stream << std::hex << hex_string;
    stream >> parsed_value;
    return parsed_value;
}

auto digit_to_direction(const char digit) {
    if (digit == '0') { return RIGHT; }
    if (digit == '1') { return DOWN; }
    if (digit == '2') { return LEFT; }
    if (digit == '3') { return UP; }
}

auto parse_color_line(const std::string& line) {
    std::stringstream stream;
    stream << line;
    char fake_direction;
    int fake_count;
    std::string color;
    stream >> fake_direction >> fake_count >> color;
    return std::make_tuple(digit_to_direction(color[color.size() -2]), hex_to_int(color.substr(2, color.size() -4)));
}

std::size_t part2(const ElvenIO::input_type &input) {
    auto position = Point(0, 0);
    std::vector<Point> path;
    path.push_back(position);
    for(const auto &line: input) {
        auto [direction, count] = parse_color_line(line);
        execute(direction, count, path, position);
    }
    return shoelace_and_picks(path);
}

int main(int _, char** argv) {
    ElvenMeasure::Reporter reporter;
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    reporter.add_io_report(io_time);
    auto [result1, solution1_time] = ElvenMeasure::execute([=] { return part1(input); }, 1);
    reporter.add_report(1, result1, solution1_time);
    auto [result2, solution2_time] = ElvenMeasure::execute([=] { return part2(input); }, 1);
    reporter.add_report(2, result2, solution2_time);
    reporter.report();
    return 0;
}
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

void execute(const std::string& line, std::vector<Point>& path, ElvenUtils::Point& position) {
    if (const auto [direction, count] = parse_line(line); direction == UP) {
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

std::size_t solve(const ElvenIO::input_type &input) {
    auto position = Point(0, 0);
    std::vector<Point> path;
    path.push_back(position);
    for(const auto &line: input) {
        execute(line, path, position);
    }
    return shoelace_and_picks(path);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 1);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}
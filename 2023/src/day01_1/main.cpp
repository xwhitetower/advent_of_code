#include <iostream>
#include <fstream>
#include <numeric>

auto parse_input(const char* filename) {
    std::fstream file(filename);
    std::vector<std::string> lines;

    std::copy(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
        std::back_inserter(lines)
    );

    return std::move(lines);
}

std::size_t solve(const std::vector<std::string> &lines) {
    return std::transform_reduce(
        lines.begin(),
        lines.end(),
        0,
        std::plus(),
        [](auto line) {
            auto high = std::find_if(line.begin(), line.end(), isdigit);
            auto low = std::find_if(line.rbegin(), line.rend(), isdigit);
            return (*high - '0') * 10 + (*low - '0');
        }
    );
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

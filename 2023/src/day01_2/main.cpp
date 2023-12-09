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
    const std::array<std::string, 9> numbers = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

    return std::transform_reduce(
        lines.begin(),
        lines.end(),
        0,
        std::plus(),
        [=](auto line) {
            // THIS IS AWFUL FIX
            size_t high = 0;
            size_t low = 0;
            for ( int i = 0 ; i < line.length(); i++) {
                int digit = -1;
                if (isdigit(line[i])) {
                    digit = line[i] - '0';
                } else {
                    for(int j = 0 ; j < numbers.size(); j++) {
                        if (strncmp(&line[i], numbers[j].c_str(), numbers[j].length()) == 0) {
                            digit = j;
                            break;
                        }
                    }
                }
                if (digit != -1) {
                    high = high == -1 ? digit : high;
                    low = digit;
                }
            }
            return (high - '0') * 10 + (low - '0');
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

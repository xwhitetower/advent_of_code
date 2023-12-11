#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

std::size_t solve(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(),
        input.end(),
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
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}
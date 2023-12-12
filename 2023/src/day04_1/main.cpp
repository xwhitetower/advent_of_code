#include <set>
#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

typedef std::vector<std::tuple<std::set<int>, std::vector<int>>> cards_type;

void skip_header(std::stringstream &stream) {
    std::string card_label;
    std::string game_number;
    stream >> card_label >> game_number;
}

auto parse_winners(std::stringstream &stream) {
    std::set<int> winners;
    std::string winner;
    while (stream >> winner && winner != "|") {
        winners.emplace(std::stoi(winner));
    }
    return std::move(winners);
}

auto parse_cards(std::stringstream &stream) {
    std::vector<int> cards;
    int card;
    while (stream >> card) {
        cards.emplace_back(card);
    }
    return std::move(cards);
}

auto parse_input(const ElvenIO::input_type &input) {
    cards_type cards;
    for (const auto &line: input) {
        std::stringstream stream;
        stream << line;
        skip_header(stream);
        cards.emplace_back(parse_winners(stream), parse_cards(stream));
    }

    return std::move(cards);
}

size_t solve(const ElvenIO::input_type &input) {
    const auto cards = parse_input(input);
    return std::transform_reduce(
        cards.begin(),
        cards.end(),
        0,
        std::plus(),
        [](auto card) {
            auto found = std::count_if(
                std::get<1>(card).begin(),
                std::get<1>(card).end(),
                [=](auto number) { return std::get<0>(card).contains(number); }
            );
            return found == 0 ? 0 : std::pow(2, found - 1);
        }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}
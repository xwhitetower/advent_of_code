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

size_t part1(const ElvenIO::input_type &input) {
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

size_t part2(const ElvenIO::input_type &input) {
    const auto cards = parse_input(input);
    std::vector counts(cards.size(), 1);

    for(int i = 0; i < cards.size(); ++i) {
        auto card = cards[i];
        const auto found = std::ranges::count_if(
            std::get<1>(card).begin(),
            std::get<1>(card).end(),
            [=](auto number) { return std::get<0>(card).contains(number); }
        );
        const auto amount_dupes = counts[i];
        for (int j = i + 1; j <= i + found; ++j) { counts[j] += amount_dupes; }
    }

    return std::reduce(
        counts.begin(),
        counts.end(),
        0,
        std::plus()
    );
}

int main(int _, char** argv) {
    ElvenMeasure::Reporter reporter;
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    reporter.add_io_report(io_time);
    auto [result1, solution1_time] = ElvenMeasure::execute([=] { return part1(input); }, 10);
    reporter.add_report(1, result1, solution1_time);
    auto [result2, solution2_time] = ElvenMeasure::execute([=] { return part2(input); }, 10);
    reporter.add_report(2, result2, solution2_time);
    reporter.report();
    return 0;
}
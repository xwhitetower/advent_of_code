#include <iostream>
#include <regex>
#include <set>
#include <__numeric/transform_reduce.h>

#include "elven_io.h"
#include "elven_measure.h"

typedef std::vector<std::tuple<std::set<int>, std::vector<int>>> cards_type;

auto parse_winners(const std::string &line) {
    std::regex number_regex("(\\d+)");
    auto colon_pos = line.find(':');
    auto pipe_pos = line.find('|');
    auto winner_begin = std::regex_iterator(line.begin() + colon_pos, line.begin() + pipe_pos, number_regex);
    auto winner_end = std::sregex_iterator();
    std::set<int> winners;
    for (std::sregex_iterator i = winner_begin; i != winner_end; ++i) {
        winners.emplace(std::stoi(i->str()));
    }
    return std::move(winners);
}

auto parse_card(const std::string &line) {
    std::regex number_regex("(\\d+)");
    auto pipe_pos = line.find('|');
    auto winner_begin = std::regex_iterator(line.begin() + pipe_pos, line.end(), number_regex);
    auto winner_end = std::sregex_iterator();
    std::vector<int> card;
    for (std::sregex_iterator i = winner_begin; i != winner_end; ++i) {
        card.emplace_back(std::stoi(i->str()));
    }
    return std::move(card);
}

auto parse_input(const ElvenIO::input_type &input) {
    cards_type cards;

    std::regex winner_regex(": ((\\d+))+ |");
    std::regex elf_card_regex("| ((\\d+))+");
    for (const auto &line: input) {
        cards.emplace_back(parse_winners(line), parse_card(line));
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
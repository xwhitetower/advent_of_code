#include <sstream>
#include <map>

#include "elven_io.h"
#include "elven_measure.h"

typedef size_t card_value;
enum HandScore {
    five_kind = 6,
    four_kind = 5,
    full_house = 4,
    three_kind = 3,
    two_pair = 2,
    one_pair = 1,
    high_card = 0
};
typedef size_t bid_value;
typedef std::array<card_value, 5> cards;
typedef std::tuple<cards, HandScore, bid_value> play;


card_value to_card_value(const char &c, const bool allow_jokers) {
    switch(c) {
        case 'A': return 14;
        case 'K': return 13;
        case 'Q': return 12;
        case 'J': return allow_jokers ? 0 : 11;
        case 'T': return 10;
        default: return c - '0';
    }
}

HandScore to_hand_score(const cards &hand, const bool allow_jokers) {
    std::map<card_value, uint8_t> tally;
    for (auto card : hand) { tally[card] += 1; }
    if (allow_jokers && tally[0] == 5) {
        return five_kind;
    }
    auto max_value = *std::ranges::max_element(
        tally.begin(), tally.end(),
        [](const auto& l, const auto& r) {
            return l.first == 0 || l.second < r.second;
    });
    if (allow_jokers) {
        max_value.second += tally[0];
        tally.erase(0);
    }
    const auto min_value = *std::ranges::min_element(tally.begin(), tally.end(), [](const auto& l, const auto& r) { return l.second < r.second; });
    switch(tally.size()) {
        case 1: return five_kind;
        case 2:
            if (min_value.second == 1) {
                return four_kind;
            }
        return full_house;
        case 3:
            if (max_value.second == 3) {
                return three_kind;
            }
        return two_pair;
        case 4: return one_pair;
        default: return high_card;
    }
}

auto parse_input(const ElvenIO::input_type &input, const bool allow_jokers) {
    std::vector<play> plays;

    for (const auto &line: input) {
        std::stringstream stream;
        stream << line;
        std::string hand;
        bid_value bid;
        stream >> hand >> bid;
        cards parsed_hand;
        std::transform(
            hand.begin(), hand.end(),
            parsed_hand.begin(),
            [&allow_jokers](auto card) { return to_card_value(card, allow_jokers); }
        );
        plays.emplace_back(parsed_hand, to_hand_score(parsed_hand, allow_jokers), bid);
    }

    return std::move(plays);
}

auto part1(const ElvenIO::input_type &input) {
    auto plays = parse_input(input, false);
    std::ranges::sort(
        plays.begin(), plays.end(),
        [](const auto &left, const auto &right) {
            auto [left_hand, left_score, left_bid] = left;
            auto [right_hand, right_score, left_right] = right;
            return left_score < right_score || left_score == right_score && left_hand < right_hand;
        }
    );
    size_t total_winnings = 0;
    for (int i = 0; i < plays.size(); ++i) {
        total_winnings += (i + 1) * std::get<2>(plays[i]);
    }
    return total_winnings;
}

auto part2(const ElvenIO::input_type &input) {
    auto plays = parse_input(input, true);
    std::ranges::sort(
        plays.begin(), plays.end(),
        [](const auto &left, const auto &right) {
            auto [left_hand, left_score, left_bid] = left;
            auto [right_hand, right_score, left_right] = right;
            return left_score < right_score || left_score == right_score && left_hand < right_hand;
        }
    );
    size_t total_winnings = 0;
    for (int i = 0; i < plays.size(); ++i) {
        total_winnings += (i + 1) * std::get<2>(plays[i]);
    }
    return total_winnings;
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
#include <iostream>
#include <fstream>
#include <map>

typedef uint8_t card_value;
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


card_value to_card_value(const char &c) {
    switch(c) {
        case 'A': return 14;
        case 'K': return 13;
        case 'Q': return 12;
        case 'J': return 11;
        case 'T': return 10;
        default: return c - '0';
    }
}

HandScore to_hand_score(const cards &hand) {
    std::map<card_value, uint8_t> tally;
    for (auto card : hand) { tally[card] += 1; }
    auto min_value = *std::ranges::min_element(tally.begin(), tally.end(), [](const auto& l, const auto& r) { return l.second < r.second; });
    auto max_value = *std::ranges::max_element(tally.begin(), tally.end(), [](const auto& l, const auto& r) { return l.second < r.second; });
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

auto parse_input(const char* filename) {
    std::fstream file(filename);
    std::vector<play> plays;

    for (auto i = std::istream_iterator<std::string>(file); i != std::istream_iterator<std::string>(); ++i) {
        std::string hand = *i;
        ++i;
        bid_value bid = std::stol(*i);
        cards parsed_hand;
        std::transform(
            hand.begin(), hand.end(),
            parsed_hand.begin(),
            [](auto card) { return to_card_value(card); }
        );
        plays.emplace_back(parsed_hand, to_hand_score(parsed_hand), bid);
    }

    return std::move(plays);
}

size_t solve(std::vector<play> &plays) {
    std::sort(
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
    using namespace std::chrono;

    auto plays = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(plays)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}
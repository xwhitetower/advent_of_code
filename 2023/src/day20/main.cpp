#include <sstream>
#include <map>
#include <numeric>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

const std::string BROADCASTER_ID = "broadcaster";
const std::string BUTTON_ID = "button";
const std::string RX_ID = "rx";

enum Pulse { low, high };
enum ModuleKind { flipflop, conjunction, broadcaster, rx};
struct PulseCommand {
    std::string from;
    std::string to;
    Pulse pulse;
};

class Module {
    std::map<std::string, uint8_t> in;
    bool state;

public:
    std::string id;
    ModuleKind kind;
    std::vector<std::string> out;

    Module(const std::string& id, ModuleKind kind) : id(id), kind(kind) {}

    void register_out(const std::string& id) { out.emplace_back(id); }
    void register_in(const std::string& id) { in.emplace(id, low); }

    void trigger(const std::string& from, const Pulse& pulse, std::deque<PulseCommand> &pulses) {
        if (kind == flipflop) {
            if (pulse == low) {
                state = !state;
                for (const auto& module : out) {
                    pulses.push_back({id, module, state ? high : low});
                }
            }
        } else if (kind == conjunction) {
            in[from] = pulse;
            const auto are_all_high = std::ranges::all_of(in.begin(), in.end(), [](const auto& entry) { return entry.second == high; });
            for (const auto& module : out) {
                pulses.push_back({id, module, are_all_high ? low : high});
            }
        } else if (kind == broadcaster) {
            for (const auto &module : out) {
                pulses.push_back({id, module, low});
            }
        }
    }

    void print_as_simple_mermaid() const {
        for (const auto& out_id : out) {
            std::cout << id << " ---> " << out_id << std::endl;
        }
    }

    friend bool operator<(const Module& lhs, const Module& rhs) {
        return lhs.id < rhs.id;
    }
};

std::vector<std::string> parse_out(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream stream;
    stream << line;
    std::string out_label;
    while (getline(stream, out_label, ',')) {
        out.push_back(out_label.substr(1));
    }
    return std::move(out);
}

std::string parse_id(const std::string& line) {
    const auto separator_pos = line.find('-');
    if (line.starts_with('%') || line.starts_with('&')) {
        return std::move(line.substr(1, separator_pos - 2));
    }
    return std::move(line.substr(0, separator_pos - 1));
}

ModuleKind parse_kind(const std::string& line) {
    if (line.starts_with('%')) {
        return flipflop;
    }
    if (line.starts_with('&')) {
        return conjunction;
    }
    return broadcaster;
}

std::map<std::string, Module> parse_input(const ElvenIO::input_type &input) {
    std::map<std::string, Module> modules;
    modules.emplace(RX_ID, Module(RX_ID, rx));
    for (const auto& line : input) {
        const auto id = parse_id(line);
        const auto kind = parse_kind(line);
        modules.emplace(id, Module(id, kind));
    }
    for (const auto& line : input) {
        const auto separator_pos = line.find('-');
        const auto out = parse_out(line.substr(line.find('-') + 2));
        const auto id = parse_id(line);
        for (const auto &out_id : out) {
            modules.at(out_id).register_in(id);
            modules.at(id).register_out(out_id);
        }
    }
    return modules;
}

std::size_t part1(const ElvenIO::input_type &input) {
    auto modules = parse_input(input);
    auto broadcaster = modules.at(BROADCASTER_ID);
    std::size_t low_pulses = 0;
    std::size_t high_pulses = 0;

    std::deque<PulseCommand> pulses;
    for (int i = 0; i < 1000; ++i) {
        pulses.push_back({BUTTON_ID, BROADCASTER_ID, low});
        while (!pulses.empty()) {
            auto pulse = pulses.front();
            pulses.pop_front();
            if (pulse.pulse == low) {
                ++low_pulses;
            } else {
                ++high_pulses;
            }
            modules.at(pulse.to).trigger(pulse.from, pulse.pulse, pulses);
        }
    }
    return low_pulses * high_pulses;
}

long fliflop_path_decode(const std::string &start, const std::map<std::string, Module>& modules) {
    long decoded_value = 0;
    std::string current_node = start;
    int bit_count = 0;
    while (!current_node.empty()) {
        const auto& out_ids = modules.at(current_node).out;
        if (std::ranges::find_if(
            out_ids.begin(), out_ids.end(),
            [&modules](const auto& id) { return modules.at(id).kind == conjunction; }
            ) != out_ids.end()) {
            decoded_value = decoded_value | 1 << bit_count;
        }
        if (auto next_node = std::ranges::find_if(
            out_ids.begin(), out_ids.end(),
            [&modules](const auto& id) { return modules.at(id).kind == flipflop; }
            ); next_node != out_ids.end()) {
            current_node = *next_node;
        } else {
            current_node = "";
        }
        ++bit_count;
    }
    return decoded_value;
}

std::size_t part2(const ElvenIO::input_type &input) {
    const auto modules = parse_input(input);
    const auto& broadcaster = modules.at(BROADCASTER_ID);
    std::vector<long> flipflop_paths;
    flipflop_paths.reserve(broadcaster.out.size());
    for (const auto &out_id : broadcaster.out) {
        flipflop_paths.emplace_back(fliflop_path_decode(out_id, modules));
    }
    return std::accumulate(
        flipflop_paths.begin(), flipflop_paths.end(),
        static_cast<std::size_t>(1),
        [](const auto & a, const auto & b) {
        return std::lcm(a, b);
    });
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
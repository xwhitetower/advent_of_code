#include <map>
#include <sstream>
#include <__numeric/transform_reduce.h>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

const std::string ACCEPTED = "A";
const std::string REJECTED = "R";

typedef std::map<char, size_t> PartSet;
typedef std::vector<PartSet> PartSets;

struct Condition {
    Condition(const char label, const char sign, const std::size_t value, const std::string& next_stage)
        : label(label),
          sign(sign),
          value(value),
          next_stage(next_stage) {
    }

    char label;
    char sign;
    int value;
    std::string next_stage;

    [[nodiscard]] bool evaluate(const PartSet& part_set) const {
        const auto& left = part_set.at(label);
        return sign == '<' ? left < value : left > value;
    }
};

struct Workflow {
    Workflow(const std::string& terminal_state, const std::vector<Condition>& conditions)
        : terminal_state(terminal_state),
          conditions(conditions) {
    }
    Workflow(const Workflow &other) : terminal_state(other.terminal_state), conditions(other.conditions) {}

    std::string terminal_state;
    std::vector<Condition> conditions;

    [[nodiscard]] std::string process(const PartSet& part_set) const {
        for (const auto& condition : conditions) {
            if (condition.evaluate(part_set)) {
                return condition.next_stage;
            }
        }
        return terminal_state;
    }
};


typedef std::map<std::string, Workflow> Workflows;

auto parse_workflow(const std::string& description) {
    std::stringstream stream;
    stream << description;
    std::string condition;
    std::vector<std::string> raw_conditions;
    while (std::getline(stream, condition, ',')) {
        raw_conditions.push_back(condition);
    }
    std::vector<Condition> conditions;
    for (int i = 0; i < raw_conditions.size() - 1; ++i) {
        auto raw_condition = raw_conditions[i];
        auto label = raw_condition[0];
        auto operation = raw_condition[1];
        auto colon_pos = raw_condition.find(':');
        auto next_stage = raw_condition.substr(colon_pos + 1);
        auto tmp = raw_condition.substr(2, colon_pos - 2);
        auto value = std::stol(raw_condition.substr(2, colon_pos - 2));
        conditions.emplace_back(label, operation, value, next_stage);
    }
    return Workflow(raw_conditions.back(), conditions);
}

auto parse_input(const ElvenIO::input_type &input) {
    Workflows workflows;
    int i = 0;
    for(; input[i] != ""; ++i) {
        std::stringstream stream;
        stream << input[i];
        std::string label;
        std::string description;
        std::getline(stream, label, '{');
        std::getline(stream, description, '}');
        workflows.emplace(label, parse_workflow(description));
    }

    PartSets part_sets;
    for (i = i + 1; i < input.size(); ++i) {
        std::stringstream stream;
        stream << input[i].substr(1, input[i].size() - 2);
        std::string piece;
        PartSet part_set;
        while (std::getline(stream, piece, ',')) {
            part_set[piece[0]] = std::stol(piece.substr(2));
        }
        part_sets.emplace_back(part_set);
    }

    return std::make_tuple(workflows, part_sets);
}

bool processed_successfully(const PartSet& part_set, const Workflows& workflows) {
    std::string current_state = "in";
    while (current_state != ACCEPTED && current_state != REJECTED) {
        current_state = workflows.at(current_state).process(part_set);
    }
    return current_state == ACCEPTED;
}

std::size_t part1(const ElvenIO::input_type &input) {
    auto [workflows, part_sets] = parse_input(input);
    std::size_t result = 0;
    for (const auto& part_set: part_sets) {
        if (processed_successfully(part_set, workflows)) {
            for (auto [label, value] : part_set) {
                result += value;
            }
        }
    }
    return result;
}

struct Range {
    int min;
    int max;
    Range() : min(1), max(4000) {}
    Range(const Range& range) : min(range.min), max(range.max) {}
};

typedef std::map<char, Range> AllowedValues;

std::size_t allowed_values_permutation_count(const AllowedValues& allowed_values) {
    return std::transform_reduce(
    allowed_values.begin(), allowed_values.end(),
    static_cast<std::size_t>(1), std::multiplies(),
    [](const auto& entry) { return entry.second.max - entry.second.min + 1;}
    );
}

std::size_t explore_workflows(const Workflows& workflows, const Workflow& workflow, AllowedValues& allowed_values) {
    std::size_t result = 0;
    for (const auto& condition : workflow.conditions) {
        auto& part_range = allowed_values[condition.label];
        if (condition.sign == '<') {
            if (part_range.min < condition.value) {
                AllowedValues condition_allowed_values(allowed_values);
                condition_allowed_values[condition.label].max = condition.value - 1;
                if (condition.next_stage == ACCEPTED) {
                    result += allowed_values_permutation_count(condition_allowed_values);
                } else if (condition.next_stage != REJECTED) {
                    result += explore_workflows(workflows, workflows.at(condition.next_stage), condition_allowed_values);
                }
            }
            part_range.min = std::max(condition.value, part_range.min);
        } else if (condition.sign == '>') {
            if (part_range.max > condition.value) {
                AllowedValues condition_allowed_values(allowed_values);
                condition_allowed_values[condition.label].min = condition.value + 1;
                if (condition.next_stage == ACCEPTED) {
                    result += allowed_values_permutation_count(condition_allowed_values);
                } else if (condition.next_stage != REJECTED) {
                    result += explore_workflows(workflows, workflows.at(condition.next_stage), condition_allowed_values);
                }
            }
            part_range.max = std::min(condition.value, part_range.max);
        }
    }
    if (workflow.terminal_state == ACCEPTED) {
        result += allowed_values_permutation_count(allowed_values);
    } else if (workflow.terminal_state != REJECTED) {
        result += explore_workflows(workflows, workflows.at(workflow.terminal_state), allowed_values);
    }
    return result;
}

std::size_t part2(const ElvenIO::input_type &input) {
    auto [workflows, _] = parse_input(input);
    AllowedValues allowed_values;
    allowed_values.emplace('x', Range());
    allowed_values.emplace('m', Range());
    allowed_values.emplace('a', Range());
    allowed_values.emplace('s', Range());
    return explore_workflows(workflows, workflows.at("in"), allowed_values);
}

int main(int _, char** argv) {
    ElvenMeasure::Reporter reporter;
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    reporter.add_io_report(io_time);
    auto [result1, solution1_time] = ElvenMeasure::execute([=] { return part1(input); }, 100);
    reporter.add_report(1, result1, solution1_time);
    auto [result2, solution2_time] = ElvenMeasure::execute([=] { return part2(input); }, 100);
    reporter.add_report(2, result2, solution2_time);
    reporter.report();
    return 0;
}
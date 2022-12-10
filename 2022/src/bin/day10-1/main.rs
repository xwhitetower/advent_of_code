use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> i32 {
    let mut signal_power = 0;
    let mut register = 1;
    let mut cycles = 1;
    for instruction in lines {
        let (instruction_cycles, value) = parse_instruction(&instruction);
        if is_power_check_cycle(cycles) {
            signal_power += cycles * register;
        } else if instruction_cycles == 2 && is_power_check_cycle(cycles + 1) {
            signal_power += (cycles + 1) * register;
        }
        register += value;
        cycles += instruction_cycles;
    }
    signal_power
}

fn parse_instruction(instruction: &String) -> (i32, i32) {
    if instruction.starts_with("a") {
        (2, instruction[5..].parse::<i32>().unwrap())
    } else {
        (1, 0)
    }
}

fn is_power_check_cycle(cycle: i32) -> bool {
    cycle <= 220 && (cycle - 20) % 40 == 0
}

fn main() -> Result<(), Box<dyn Error>> {
    let timer = Instant::now();
    println!("{}", solve(read_lines("./data/day10/input.txt")?));
    println!("total time [{:.2?}]", timer.elapsed());
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day10/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day10/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<i32>().unwrap();
    assert_eq!(solve(lines), expect);
}

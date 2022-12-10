use std::collections::HashSet;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;


fn solve(lines: Vec<String>) -> u32 {
    lines.iter().map(|line| shared_element_priority(line) ).sum()
}

fn shared_element_priority(line: &str) -> u32 {
    let priorities: Vec<u32> = line.chars().map (|c| char_to_priority(c) ).collect();
    let half_point = priorities.len() / 2;
    let first_compartment :HashSet<&u32> = priorities[..half_point].into_iter().collect();
    let second_compartment :HashSet<&u32> = priorities[half_point..].into_iter().collect();
    let shared_priority = first_compartment.intersection(&second_compartment)
        .next()
        .unwrap();
    **shared_priority
}

fn char_to_priority(c: char) -> u32 {
    match c.is_lowercase() {
        true => c as u32 - 'a' as u32 + 1,
        false => c as u32 - 'A' as u32 + 27
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let timer = Instant::now();
    println!("{}", solve(read_lines("./data/day3/input.txt")?));
    println!("total time [{:.2?}]", timer.elapsed());
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day3/test_input.txt").unwrap();
    let expect = read_lines("./data/day3/part1/test_result.txt")
        .unwrap()
        .first()
        .expect("Missing test input.txt")
        .parse::<u32>()
        .unwrap();
    assert_eq!(solve(lines), expect);
}

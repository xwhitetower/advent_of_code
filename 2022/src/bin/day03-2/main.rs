use std::collections::HashSet;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;


fn solve(lines: Vec<String>) -> u32 {
    lines.chunks(3).map(|elf_squad| badge_priority(elf_squad) ).sum()
}

fn badge_priority(elf_squad: &[String]) -> u32 {
    *elf_squad.iter()
        .map(|bag| bag.chars().map(|c| char_to_priority(c) ))
        .map(|x| x.into_iter().collect::<HashSet<u32>>())
        .reduce(|left, right|
            left.intersection(&right).map(|x| *x).collect::<HashSet<u32>>()
        )
        .unwrap()
        .iter()
        .next()
        .unwrap()
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
    let expect = read_lines("./data/day3/part2/test_result.txt")
        .unwrap()
        .first()
        .expect("Missing test input.txt")
        .parse::<u32>()
        .unwrap();
    assert_eq!(solve(lines), expect);
}

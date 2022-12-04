use std::error::Error;

use advent_of_code::read_lines;


fn solve(lines: Vec<String>) -> u32 {
    lines.iter().filter(|line| does_elf_do_redundant_work(line) ).count().try_into().unwrap()
}

fn does_elf_do_redundant_work(line: &str) -> bool {
    let ids: Vec<u32> = line.split(",").flat_map(|x| parse_range(x) ).collect();
    let left = ids[0]..=ids[1];
    let right = ids[2]..=ids[3];
    ids[..2].iter().all(|x| right.contains(x) ) || ids[2..].iter().all( |x| left.contains(x) )
}

fn parse_range(pair: &str) -> Vec<u32> {
    pair.split('-').map(|x| x.parse::<u32>().unwrap()).collect()
}

fn main() -> Result<(), Box<dyn Error>> {
    println!("{}", solve(read_lines("./data/day4/input.txt")?));
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day4/test_input.txt").unwrap();
    let expect = read_lines("./data/day4/part1/test_result.txt")
        .unwrap()
        .first()
        .expect("Missing test input.txt")
        .parse::<u32>()
        .unwrap();
    assert_eq!(solve(lines), expect);
}

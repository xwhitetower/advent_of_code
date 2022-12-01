use std::cmp;
use std::error::Error;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> Result<i32, Box<dyn Error>> {
    let mut calories = 0;
    let mut max_calories = 0;
    for line in lines {
        if line == "" {
            calories = 0
        } else {
            calories += line.parse::<i32>()?;
            max_calories = cmp::max(max_calories, calories)
        }
    }
    Ok(max_calories)
}

#[test]
fn test() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day1/test_input.txt")?;
    let expect = read_lines("./data/day1/part1/test_result.txt")?
        .first()
        .expect("Missing test input.txt")
        .parse::<i32>()?;
    assert_eq!(solve(lines)?, expect);
    Ok(())
}

fn main() -> Result<(), Box<dyn Error>> {
    println!("{}", solve(read_lines("./data/day1/input.txt")?)?);
    Ok(())
}

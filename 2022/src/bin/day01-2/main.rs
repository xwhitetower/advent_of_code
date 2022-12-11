use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> Result<i32, Box<dyn Error>> {
    let mut calories: Vec<i32> = vec![0];

    for line in lines {
        if line == "" {
            calories.push(0)
        } else {
            *calories.last_mut().unwrap() += line.parse::<i32>()?;
        }
    }
    calories.sort();
    Ok(calories.iter().rev().take(3).sum())
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day1/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines)?;
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day1/test_input.txt")?;
    let expect = read_lines("./data/day1/part2/test_result.txt")?
        .first()
        .expect("Missing test input.txt")
        .parse::<i32>()?;
    assert_eq!(solve(lines)?, expect);
    Ok(())
}

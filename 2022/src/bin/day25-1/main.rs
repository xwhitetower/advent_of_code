use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

const BASE: i64 = 5;
const MINUS: char = '-';
const DOUBLE_MINUS: char = '=';

fn solve(lines: Vec<String>) -> String {
    let decimal_sum = lines.iter().fold(0, |acc, line| acc + to_decimal(line) );
    to_snafu(decimal_sum)
}

fn to_decimal(line: &String) -> i64 {
    line.chars().fold(0, |acc, c| {
        let digit = if c == MINUS {
            -1
        } else if c == DOUBLE_MINUS {
            -2
        } else {
            c.to_digit(10).unwrap() as i64
        };
        acc * BASE + digit
    })
}

fn to_snafu(mut number: i64) -> String {
    let mut snafu = "".to_string();
    while number > 0 {
        let reminder = number % 5;
        number /= 5;
        if reminder < 3 {
            snafu += &reminder.to_string();
        } else if reminder == 3 {
            snafu += "=";
            number += 1
        } else {
            snafu += "-";
            number += 1
        }
    }
    snafu.chars().rev().collect()
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day25/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day25/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day25/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap();
    assert_eq!(solve(lines), *expect);
}
use std::collections::HashSet;
use std::error::Error;

use advent_of_code::read_lines;


fn solve(signal: &str) -> Result<u32, Box<dyn Error>> {
    let chars: Vec<char> = signal.chars().collect();
    for i in 14..chars.len() {
        if chars[i-14..i].into_iter().collect::<HashSet<&char>>().len() == 14 {
            return Ok(i as u32)
        }
    }
    Err(Box::from("No start of packet found"))
}

fn main() -> Result<(), Box<dyn Error>> {
    let signal = &read_lines("./data/day6/input.txt")?[0];
    println!("{}", solve(signal)?);
    Ok(())
}

#[test]
fn test() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day6/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day6/part2/test_result.txt").unwrap();
    let expect = expected_lines[0].parse::<u32>().unwrap();
    assert_eq!(solve(&lines[0])?, expect);
    Ok(())
}

#[test]
fn extra_example_1() -> Result<(), Box<dyn Error>> {
    assert_eq!(solve("bvwbjplbgvbhsrlpgdmjqwftvncz")?, 23);
    Ok(())
}

#[test]
fn extra_example_2() -> Result<(), Box<dyn Error>> {
    assert_eq!(solve("nppdvjthqldpwncqszvftbrmjlhg")?, 23);
    Ok(())
}

#[test]
fn extra_example_3() -> Result<(), Box<dyn Error>> {
    assert_eq!(solve("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg")?, 29);
    Ok(())
}

#[test]
fn extra_example_4() -> Result<(), Box<dyn Error>> {
    assert_eq!(solve("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw")?, 26);
    Ok(())
}


use std::collections::HashMap;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> i64 {
    let mut monke_yells: HashMap<String, String> = HashMap::new();
    for line in lines {
        let parts: Vec<&str> = line.split(": ").collect();
        monke_yells.insert(parts[0].to_string(), parts[1].to_string());
    }
    let mut yell_cache: HashMap<String, i64> = HashMap::new();
    yell(&"root".to_string(), &monke_yells, &mut yell_cache)
}

fn yell(monke: &String, monke_yells: &HashMap<String, String>, yell_cache: &mut HashMap<String, i64>) -> i64 {
    match yell_cache.get(monke) { // Not using entry api due borrow issues on having the recursive calls
        None => {
            let math_string = monke_yells.get(monke).unwrap();
            let words: Vec<&str> = math_string.split(" ").collect();
            if words.len() == 1 {
                words[0].parse().unwrap()
            } else {
                let left = yell(&words[0].to_string(), monke_yells, yell_cache);
                yell_cache.insert(words[0].to_string(), left);
                let right = yell(&words[2].to_string(), monke_yells, yell_cache);
                yell_cache.insert(words[2].to_string(), right);
                match words[1] {
                    "+" => left + right,
                    "-" => left - right,
                    "*" => left * right,
                    "/" => left / right,
                    _ => panic!("Unexpected operator: {}", words[1])
                }
            }
        },
        Some(number) => *number
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day21/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day21/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day21/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<i64>().unwrap();
    assert_eq!(solve(lines), expect);
}

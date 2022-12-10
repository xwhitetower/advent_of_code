use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

static WIDTH: i32 = 40;
static LIT: char = '#';
static DARK: char = '.';

fn solve(lines: Vec<String>) -> Vec<Vec<char>> {
    let mut screen = vec![Vec::new(); 6];
    let mut register: i32 = 1;
    let mut cycle = 0;
    for instruction in lines {
        let (instruction_cycles, value) = parse_instruction(&instruction);
        for _ in 0..instruction_cycles {
            let x = cycle % WIDTH;
            let y = cycle / WIDTH;
            let pixel_value = if x >= register - 1 && x <= register + 1 { LIT } else { DARK };
            screen[y as usize].push(pixel_value);
            cycle += 1;
        }
        register += value;
    }
    screen
}

fn parse_instruction(instruction: &String) -> (i32, i32) {
    if instruction.starts_with("a") {
        (2, instruction[5..].parse::<i32>().unwrap())
    } else {
        (1, 0)
    }
}


fn main() -> Result<(), Box<dyn Error>> {
    let timer = Instant::now();
    let screen =  solve(read_lines("./data/day10/input.txt")?);
    for i in 0..screen.len() {
        for pixel in &screen[i] {
            print!("{}", pixel);
        }
        println!();
    }
    println!("total time [{:.2?}]", timer.elapsed());
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day10/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day10/part2/test_result.txt").unwrap();
    let expect: Vec<Vec<char>> = expected_lines.into_iter().map(|x| x.chars().collect()).collect();
    let screen =  solve(lines);

    for i in 0..expect.len() {
        for j in 0..expect[i].len() {
            assert_eq!(screen[i][j], expect[i][j]);
        }
    }
}

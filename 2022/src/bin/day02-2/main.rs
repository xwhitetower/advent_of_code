use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

static ENEMY_ROCK: u32 = 'A' as u32;
static RESULT_LOSER: u32 = 'X' as u32;
static SCORE_MATRIX: [[u32; 3]; 3] = [
    [3, 1, 2],
    [1, 2, 3],
    [2, 3, 1]
];

fn solve(lines: Vec<String>) -> u32 {
    lines.iter().map(|line| score(line) ).sum()
}

fn score(line: &str) -> u32 {
    let enemy_move = line.chars().next().unwrap() as u32;
    let result = line.chars().last().unwrap() as u32;
    let result_index = result - RESULT_LOSER;
    SCORE_MATRIX[result_index as usize][(enemy_move - ENEMY_ROCK) as usize] + result_index * 3
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day2/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day2/test_input.txt").unwrap();
    let expect = read_lines("./data/day2/part2/test_result.txt")
        .unwrap()
        .first()
        .expect("Missing test input.txt")
        .parse::<u32>()
        .unwrap();
    assert_eq!(solve(lines), expect);
}

#[test]
fn test_enemy_rock_lose() {
    assert_eq!(solve(vec!["A X".to_string()]), 3);
}

#[test]
fn test_enemy_rock_draw() {
    assert_eq!(solve(vec!["A Y".to_string()]), 4);
}

#[test]
fn test_enemy_rock_win() {
    assert_eq!(solve(vec!["A Z".to_string()]), 8);
}

#[test]
fn test_enemy_paper_lose() {
    assert_eq!(solve(vec!["B X".to_string()]), 1);
}

#[test]
fn test_enemy_paper_draw() {
    assert_eq!(solve(vec!["B Y".to_string()]), 5);
}

#[test]
fn test_enemy_paper_win() {
    assert_eq!(solve(vec!["B Z".to_string()]), 9);
}

#[test]
fn test_enemy_scissors_lose() {
    assert_eq!(solve(vec!["C X".to_string()]), 2);
}

#[test]
fn test_enemy_scissors_draw() {
    assert_eq!(solve(vec!["C Y".to_string()]), 6);
}

#[test]
fn test_enemy_scissors_win()  {
    assert_eq!(solve(vec!["C Z".to_string()]), 7);
}


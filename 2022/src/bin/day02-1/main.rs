use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

static ENEMY_ROCK: u32 = 'A' as u32;
static HERO_ROCK: u32 = 'X' as u32;
static SCORE_MATRIX: [[u32; 3]; 3] = [
    [3, 0, 6],
    [6, 3, 0],
    [0, 6, 3]
];

fn solve(lines: Vec<String>) -> u32 {
    lines.iter().map(|line| score(line) ).sum()
}

fn score(line: &str) -> u32 {
    let enemy_move = line.chars().next().unwrap() as u32;
    let hero_move = line.chars().last().unwrap() as u32;
    let hero_index = hero_move - HERO_ROCK;
    SCORE_MATRIX[hero_index as usize][(enemy_move - ENEMY_ROCK) as usize] + hero_index + 1
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
    let expect = read_lines("./data/day2/part1/test_result.txt")
        .unwrap()
        .first()
        .expect("Missing test input.txt")
        .parse::<u32>()
        .unwrap();
    assert_eq!(solve(lines), expect);
}

#[test]
fn test_lose_rock() {
    assert_eq!(solve(vec!["B X".to_string()]), 1);
}

#[test]
fn test_lose_paper() {
    assert_eq!(solve(vec!["C Y".to_string()]), 2);
}

#[test]
fn test_lose_scissors() {
    assert_eq!(solve(vec!["A Z".to_string()]), 3);
}

#[test]
fn test_tie_rock() {
    assert_eq!(solve(vec!["A X".to_string()]), 4);
}

#[test]
fn test_tie_paper() {
    assert_eq!(solve(vec!["B Y".to_string()]), 5);
}

#[test]
fn test_tie_scissors() {
    assert_eq!(solve(vec!["C Z".to_string()]), 6);
}

#[test]
fn test_win_with_rock() {
    assert_eq!(solve(vec!["C X".to_string()]), 7);
}

#[test]
fn test_win_with_paper() {
    assert_eq!(solve(vec!["A Y".to_string()]), 8);
}

#[test]
fn test_win_with_scissors()  {
    assert_eq!(solve(vec!["B Z".to_string()]), 9);
}


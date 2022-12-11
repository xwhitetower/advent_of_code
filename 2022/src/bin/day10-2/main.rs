use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

static WIDTH: usize = 40;
static LETTER_WIDTH: usize = 5;
static LETTER_HEIGHT: usize = 6;
static LETTER_COUNT: usize = WIDTH / LETTER_WIDTH;
static LIT: char = '#';
static DARK: char = '.';

fn solve(lines: Vec<String>) -> Vec<Vec<char>> {
    let mut screen = vec![Vec::new(); LETTER_COUNT as  usize];
    let mut register: i32 = 1;
    let mut cycle = 0;
    for instruction in lines {
        let (instruction_cycles, value) = parse_instruction(&instruction);
        for _ in 0..instruction_cycles {
            let x = cycle % WIDTH as i32;
            let letter = x as usize / LETTER_WIDTH;
            let pixel_value = if x >= register - 1 && x <= register + 1 { LIT } else { DARK };
            screen[letter as usize].push(pixel_value);
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

fn debug_print_screen(screen: &Vec<Vec<char>>) {
    if cfg!(debug_assertions) {
        for row in 0..LETTER_HEIGHT as usize {
            for letter in 0..LETTER_COUNT as usize {
                for column in 0..LETTER_WIDTH as usize {
                    print!("{}", screen[letter][row * LETTER_WIDTH as usize + column]);
                }
            }
            println!();
        }
    }
}

fn display_to_char(letter: &str) -> char {
    match letter {
        ".##..#..#.#..#.####.#..#.#..#." => 'A',
        "####.#....###..#....#....####." => 'E',
        ".##..#..#.#....#.##.#..#..###." => 'G',
        "#..#.#..#.####.#..#.#..#.#..#." => 'H',
        "#..#.#.#..##...#.#..#.#..#..#." => 'K',
        "#....#....#....#....#....####." => 'L',
        "###..#..#.#..#.###..#....#...." => 'P',
        "###..#..#.#..#.###..#.#..#..#." => 'R',
        "#..#.#..#.#..#.#..#.#..#..##.." => 'U',
        "####....#...#...#...#....####." => 'Z',
        &_ => '?',
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day10/input.txt")?;
    let timer = Instant::now();
    let screen =  solve(lines);
    debug_print_screen(&screen);
    let solution = screen.iter()
        .map(|x| display_to_char(&x.iter().collect::<String>()))
        .collect::<String>();
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day10/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day10/part2/test_result.txt").unwrap();
    let expect: Vec<Vec<char>> = expected_lines.into_iter().map(|x| x.chars().collect()).collect();
    let screen =  solve(lines);
    for row in 0..LETTER_HEIGHT as usize {
        for letter in 0..LETTER_COUNT as usize {
            for column in 0..LETTER_WIDTH as usize {
            assert_eq!(screen[letter][row * LETTER_WIDTH as usize + column], expect[row][column + letter * LETTER_WIDTH]);
            }
        }
        println!();
    }
}

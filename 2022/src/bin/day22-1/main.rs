use std::cmp::max;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Map = Vec<Vec<char>>;
type Point = (i32, i32);
type Instructions = Vec<Instruction>;

const EMPTY: char = ' ';
const WALL: char = '#';
const OPEN: char = '.';

const ROTATE_RIGHT: char = 'R';
const ROTATE_LEFT: char = 'L';

#[derive(Debug)]
enum Instruction {
    FORWARD(u32),
    ROTATE(char)
}

#[derive(Debug)]
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
}

impl Direction {
    fn rotate(self: &Self, rotation: char) -> Direction {
        match self {
            Direction::UP => if rotation == ROTATE_RIGHT { Direction::RIGHT } else { Direction::LEFT },
            Direction::RIGHT => if rotation == ROTATE_RIGHT { Direction::DOWN } else { Direction::UP },
            Direction::DOWN => if rotation == ROTATE_RIGHT { Direction::LEFT } else { Direction::RIGHT },
            Direction::LEFT => if rotation == ROTATE_RIGHT { Direction::UP } else { Direction::DOWN }
        }
    }

    fn to_step(self: &Self) -> Point {
        match self {
            Direction::UP => (0, -1),
            Direction::RIGHT => (1, 0),
            Direction::DOWN => (0, 1),
            Direction::LEFT => (-1, 0)
        }
    }

    fn to_solution_value(self: &Self) -> u32 {
        match self {
            Direction::UP => 3,
            Direction::RIGHT => 0,
            Direction::DOWN => 1,
            Direction::LEFT => 2
        }
    }
}

fn solve(lines: Vec<String>) -> u32 {
    let map = parse_map(&lines);
    let instructions = parse_instructions(&lines);
    let mut pos = find_start_position(&map);
    let mut direction = Direction::RIGHT;
    for instruction in instructions {
        match instruction {
            Instruction::FORWARD(steps) => {
                let step_vector = direction.to_step();
                for _ in 0..steps {
                    let mut new_pos = (pos.0 + step_vector.0, pos.1 + step_vector.1);
                    if must_warp(&step_vector, &new_pos, &map) {
                        let warp_pos = match direction {
                            Direction::UP => warp_up(&new_pos, &map),
                            Direction::RIGHT => warp_right(&new_pos, &map),
                            Direction::DOWN => warp_down(&new_pos, &map),
                            Direction::LEFT => warp_left(&new_pos, &map)
                        };
                        new_pos = warp_pos;
                    }
                    if is_wall_tile(&new_pos, &map) {
                        break;
                    } else {
                        pos = new_pos;
                    }
                }
            },
            Instruction::ROTATE(rotation) => direction = direction.rotate(rotation)
        };
    }
    (pos.1 as u32 + 1) * 1000 + (pos.0 as u32 + 1) * 4 + direction.to_solution_value()
}

fn parse_map(lines: &Vec<String>) -> Map {
    let max_x_size = lines[0..lines.len() - 2].iter().fold(0, |acc, x | max(acc, x.chars().count()) );
    lines[0..lines.len() - 2].iter()
        .map(|x| {
            let mut row: Vec<char> = x.chars().collect();
            while row.len() < max_x_size {
                row.push(EMPTY);
            }
            row
        })
        .collect()
}

fn parse_instructions(lines: &Vec<String>) -> Instructions {
    let mut walk = 0;
    let mut instructions = Instructions::new();
    let raw_line: Vec<char> = lines.last().unwrap().chars().collect();
    for c in raw_line {
        if c == ROTATE_RIGHT || c == ROTATE_LEFT {
            if walk != 0 {
                instructions.push(Instruction::FORWARD(walk));
                walk = 0;
            }
            instructions.push(Instruction::ROTATE(c));
        } else {
            walk = walk * 10 + c.to_digit(10).unwrap()
        }
    }
    if walk != 0 {
        instructions.push(Instruction::FORWARD(walk));
    }
    instructions
}

fn find_start_position(map: &Map) -> Point {
    (map[0].iter().position(|x| x == &OPEN).unwrap() as i32, 0)
}

fn must_warp(step_vector: &Point, new_pos: &Point, map: &Map) -> bool {
    (step_vector.0 == -1 && (new_pos.0 < 0 || is_hyperspace_tile(&new_pos, &map)))
      || (step_vector.0 == 1 && (new_pos.0 >= map[new_pos.1 as usize].len() as i32 || is_hyperspace_tile(&new_pos, &map)))
      || (step_vector.1 == -1 && (new_pos.1 < 0 || is_hyperspace_tile(&new_pos, &map)))
      || (step_vector.1 == 1 && (new_pos.1 as usize >= map.len() || is_hyperspace_tile(&new_pos, &map)))
}

fn warp_up(point: &Point, map: &Map) -> Point {
    let mut new_pos = if point.1 >= 0 {
        (point.0, point.1 - 1)
    } else {
        (point.0, (map.len() as i32 - 1 ))
    };
    while is_hyperspace_tile(&new_pos, &map) {
        if new_pos.1 > 0 {
            new_pos.1 -= 1;
        } else {
            new_pos.1 = map.len() as i32 - 1;
        }
    };
    new_pos
}

fn warp_down(point: &Point, map: &Map) -> Point {
    let mut new_pos = (point.0, (point.1 + 1) % map.len() as i32);
    while is_hyperspace_tile(&new_pos, &map) {
        new_pos.1 = (new_pos.1 + 1) % map.len() as i32;
    };
    new_pos
}

fn warp_left(point: &Point, map: &Map) -> Point {
    let x_size = map[point.1 as usize].len() as i32;
    let mut new_pos = if point.0 >= 0 {
        (point.0 - 1, point.1)
    } else {
        (x_size - 1, point.1)
    };
    while is_hyperspace_tile(&new_pos, &map) {
        if new_pos.0 > 0 {
            new_pos.0 -= 1;
        } else {
            new_pos.0 = x_size - 1;
        }
    };
    new_pos
}

fn warp_right(point: &Point, map: &Map) -> Point {
    let x_size = map[point.1 as usize].len() as i32;
    let mut new_pos = ((point.0 + 1) % x_size, point.1);
    while is_hyperspace_tile(&new_pos, &map) {
        new_pos.0 = (new_pos.0 + 1) % x_size;
    };
    new_pos
}

fn is_hyperspace_tile(point: &Point, map: &Map) -> bool {
    map[point.1 as usize][point.0 as usize] == EMPTY
}

fn is_wall_tile(point: &Point, map: &Map) -> bool {
    map[point.1 as usize][point.0 as usize] == WALL
}

#[allow(dead_code)]
fn print_map(map: &Map) {
    for row in map {
        println!("{}", row.iter().collect::<String>());
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day22/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day22/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day22/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Map = Vec<u8>;
type Point = (i32, i32);

const LEFT: char = '<';
const RIGHT: char = '>';
const EMPTY: char = '.';
const BLOCKED: char = '#';
static MAP_BOUND_SIZE: i32 = 7;
static ROUNDS: u32 = 2022;

struct Tetris {
    pixels: Vec<Point>,
    box_max_point: Point
}

fn solve(lines: Vec<String>) -> u32 {
    let flow: Vec<char> = lines[0].chars().collect();
    let pieces: Vec<Tetris> = init_tetris();
    let mut current_flow_idx: usize = 0;
    let mut current_tetris_idx = 0;
    let mut map: Map = Vec::new();
    for _ in 0..ROUNDS {
        let mut can_move = true;
        let tetris = &pieces[current_tetris_idx];
        let mut pos = (2, (map.len() + 3) as i32);
        while can_move {
            let horizontal_movement = (pos.0 + air_flow_movement(flow[current_flow_idx]), pos.1);
            if !collide(&horizontal_movement, &tetris, &map) {
                pos = horizontal_movement;
            }
            let fall_movement = (pos.0, pos.1 - 1);
            if collide(&fall_movement, &tetris, &map) {
                can_move = false;
                rest(&pos, &tetris, &mut map);
            } else {
                pos = fall_movement;
            }
            current_flow_idx = (current_flow_idx + 1) % flow.len();
        }
        current_tetris_idx = (current_tetris_idx + 1) % pieces.len();
    }
    map.len() as u32
}

fn init_tetris() -> Vec<Tetris> {
    vec![
        Tetris {
            pixels: (0..4).map(|x| (x, 0)).collect(),
            box_max_point: (3, 0)
        },
        {
            let mut tmp: Vec<Point> = vec![(1, 0), (1,  2)];
            let tmp2: Vec<Point> = (0..3).map(|x| (x, 1)).collect();
            tmp.extend(tmp2);
            Tetris {
                pixels: tmp,
                box_max_point: (2, 2)
            }
        },
        {
            let mut tmp: Vec<Point> = (0..3).map(|x| (x, 0)).collect();
            let tmp2: Vec<Point> = (1..3).map(|x| (2, x)).collect();
            tmp.extend(tmp2);
            Tetris {
                pixels: tmp,
                box_max_point: (2, 2)
            }
        },
        Tetris {
            pixels: (0..4).map(|x| (0, x)).collect(),
            box_max_point: (0, 3)
        },
        Tetris {
            pixels: vec![(0, 0), (1, 0), (0, 1), (1, 1)],
            box_max_point: (1, 1)
        }
    ]
}

fn air_flow_movement(flow: char) -> i32 {
    match flow {
        LEFT => -1,
        RIGHT => 1,
        _ => panic!("Invalid air flow: {}", flow)
    }
}

fn collide(pos: &Point, tetris: &Tetris, map: &Map) -> bool {
    pos.0 < 0 || pos.1 < 0 || pos.0 + tetris.box_max_point.0 >= MAP_BOUND_SIZE
    || tetris.pixels.iter().any(|x|
        (x.1 + pos.1) < (map.len() as i32) && ((map[(x.1 + pos.1) as usize] & 1 << (x.0 + pos.0)) != 0)
    )
}

fn rest(pos: &Point, tetris: &Tetris, map: &mut Map) {
    while map.len() <= (pos.1 + tetris.box_max_point.1) as usize {
        map.push(0);
    }
    for pixel in &tetris.pixels {
        map[(pixel.1 + pos.1) as usize] |= 1 << (pixel.0 + pos.0);
    }
}

#[allow(dead_code)]
fn print_map(map: &Map) {
    for i in (0..map.len()).rev() {
        for j in 0..MAP_BOUND_SIZE {
            let state = if (map[i as usize] & (1 << j)) == 0 { EMPTY } else { BLOCKED };
            print!("{}", state);
        }
        println!();
    }
    println!();
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day17/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day17/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day17/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

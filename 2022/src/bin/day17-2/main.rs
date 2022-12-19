use std::cmp::max;
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
static ROUNDS: u64 = 1_000_000_000_000;

struct Tetris {
    pixels: Vec<u8>,
    box_max_point: Point
}

#[derive(Eq, PartialEq, Debug)]
struct State {
    tetris_idx: usize,
    flow_idx: usize,
    last_action_changes: Vec<u8>
}

#[derive(Debug)]
struct Metadata {
    state: State,
    height: usize
}

// Using Brent https://en.wikipedia.org/wiki/Cycle_detection#Brent's_algorithm
fn solve(lines: Vec<String>) -> u64 {
    let flows: Vec<char> = lines[0].chars().collect();
    let pieces: Vec<Tetris> = init_tetris();
    let mut current_flow_idx: usize = 0;
    let mut current_tetris_idx = 0;
    let mut map: Map = Vec::new();
    let mut states: Vec<Metadata> = Vec::new();
    let mut lam = 1;
    let mut power = 1;
    let mut tortoise = 0;
    let mut prev = Vec::new();
    let mut hare = 1;
    for _ in 0..2 { // warm up state of 100 + tortoise & hare init
        states.push( Metadata{
            state: State {
                tetris_idx: current_tetris_idx,
                flow_idx: current_flow_idx,
                last_action_changes: prev
            },
            height: map.len()
        });
        let tetris = &pieces[current_tetris_idx];
        prev = next_play(&tetris, &mut map, &flows, &mut current_flow_idx);
        current_tetris_idx = (current_tetris_idx + 1) % pieces.len();
    }
    while states[tortoise].state != states[hare].state {
        if power == lam {
            tortoise = hare;
            power *= 2;
            lam = 0;
        }
        hare += 1;
        lam += 1;
        states.push( Metadata{
            state: State {
                tetris_idx: current_tetris_idx,
                flow_idx: current_flow_idx,
                last_action_changes: prev
            },
            height: map.len()
        });
        let tetris = &pieces[current_tetris_idx];
        prev = next_play(&tetris, &mut map, &flows, &mut current_flow_idx);
        current_tetris_idx = (current_tetris_idx + 1) % pieces.len();
    }
    tortoise = 0;
    hare =  lam;
    let mut mu: usize = 0;
    while states[tortoise].state != states[hare].state {
        tortoise += 1;
        hare += 1;
        mu += 1;
    }
    let delta = states[hare].height - states[tortoise].height;
    let full_cycles_height = delta * ((ROUNDS as usize - mu) / lam);
    let partial_cycle = states[tortoise + ((ROUNDS as usize - mu) % lam)].height - states[tortoise].height;
    (states[tortoise].height + full_cycles_height + partial_cycle) as u64
}

fn init_tetris() -> Vec<Tetris> {
    vec![
        Tetris {
            pixels: vec![0b1111],
            box_max_point: (3, 0)
        },
        Tetris {
            pixels: vec![0b010, 0b111, 0b010],
            box_max_point: (2, 2)
        },
        Tetris {
            pixels: vec![0b111, 0b100, 0b100],
            box_max_point: (2, 2)
        },
        Tetris {
            pixels: vec![0b1, 0b1, 0b1, 0b1],
            box_max_point: (0, 3)
        },
        Tetris {
            pixels: vec![0b11, 0b11],
            box_max_point: (1, 1)
        }
    ]
}

fn next_play(tetris: &Tetris, map: &mut Map, flows: &Vec<char>, current_flow_idx: &mut usize) -> Vec<u8> {
    let mut can_move = true;
    let mut pos = (2, (map.len() + 3) as i32);
    let mut changes: Vec<u8> = Vec::new();
    while can_move {
        let horizontal_movement = (pos.0 + air_flow_movement(flows[*current_flow_idx]), pos.1);
        if !collide(&horizontal_movement, &tetris, map) {
            pos = horizontal_movement;
        }
        let fall_movement = (pos.0, pos.1 - 1);
        if collide(&fall_movement, &tetris, map) {
            can_move = false;
            rest(&pos, &tetris, map);
            (max(0, map.len()-5)..map.len()).rev().for_each(|x| changes.push(map[x]));
        } else {
            pos = fall_movement;
        }
        *current_flow_idx = (*current_flow_idx + 1) % flows.len();
    }
    changes
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
    || tetris.pixels.iter().enumerate().any(|(i, x)|
        (i as i32 + pos.1) < (map.len() as i32) && ((map[(i as i32 + pos.1) as usize] & x << pos.0) != 0)
    )
}

fn rest(pos: &Point, tetris: &Tetris, map: &mut Map) {
    while map.len() <= (pos.1 + tetris.box_max_point.1) as usize {
        map.push(0);
    }
    tetris.pixels.iter().enumerate().for_each(|(i, pixel)| {
        map[i + pos.1 as usize] |= pixel << pos.0;
    });
}

#[allow(dead_code)]
fn print_map(map: &Map) {
    for i in (0..map.len()).rev() {
        print!("{} -", i);
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
    let expected_lines = read_lines("./data/day17/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u64>().unwrap();
    assert_eq!(solve(lines), expect);
}

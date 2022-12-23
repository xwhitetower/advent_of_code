use std::collections::{HashMap, HashSet, VecDeque};
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Point = (i32, i32);
type Map = VecDeque<VecDeque<char>>;

const EMPTY: char = '.';
const ELF: char = '#';
const ROUNDS: usize = 10;

fn solve(lines: Vec<String>) -> u32 {
    let mut map = parse_map(&lines);
    let mut round = 0;
    loop {
        let elves = find_elves_positions(&map);
        let crowded_area_elves = find_crowded_area_elves(&elves, &map);
        let elves_moves = compute_elves_moves(round, &crowded_area_elves, &map);
        if elves_moves.iter().filter(|entry| entry.1.len() == 1).count() == 0 {
            return round as u32 + 1;
        }
        let position_correction = expand_boundaries(&elves_moves, &mut map);
        execute_moves(&elves_moves, &position_correction, &mut map);
        round += 1;
    }
}

fn parse_map(lines: &Vec<String>) -> Map {
    lines.iter()
        .map(|x| { x.chars().collect() })
        .collect()
}

fn find_elves_positions(map: &Map) -> HashSet<Point> {
    let mut elves = HashSet::new();
    for y in 0..map.len() {
        for x in 0..map[0].len() {
            if map[y][x] == ELF {
                elves.insert((x as i32, y as i32));
            }
        }
    }
    elves
}

fn find_crowded_area_elves(elves: &HashSet<Point>, map: &Map) -> HashSet<Point> {
    elves.iter().filter(|(x, y)| {
            vec![(x - 1, y - 1), (x - 1, *y), (x - 1, y + 1), (*x, y - 1), (*x, y + 1), (x + 1, y - 1), (x + 1, *y), (x + 1, y + 1)]
                .iter()
                .any(|(x1, y1)| !out_of_bounds(*x1, *y1, map) && map[*y1 as usize][*x1 as usize] == ELF)
        }).map(|x| *x)
        .collect()
}

fn compute_elves_moves(round: usize, elves: &HashSet<Point>, map: &Map) -> HashMap<Point, Vec<Point>> {
    let move_evaluation: Vec<(Box<dyn Fn(&i32, &i32) -> bool>, Box<dyn Fn(&i32, &i32) -> Point>)> = vec![
        (
            Box::new(|x: &i32, y: &i32| -> bool { are_empty(&vec![(x - 1, y - 1), (*x, y - 1), (x + 1, y - 1)], map) }),
            Box::new(|x: &i32, y: &i32| -> Point {(*x, y - 1)})
        ),
        (
            Box::new(|x: &i32, y: &i32| -> bool { are_empty(&vec![(x - 1, y + 1), (*x, y + 1), (x + 1, y + 1)], map) }),
            Box::new(|x: &i32, y: &i32| -> Point {(*x, y + 1)})
        ),
        (
            Box::new(|x: &i32, y: &i32| -> bool { are_empty(&vec![(x - 1, y - 1), (x - 1, *y), (x - 1, y + 1)], map) }),
            Box::new(|x: &i32, y: &i32| -> Point {(x - 1, *y)})
        ),
        (
            Box::new(|x: &i32, y: &i32| -> bool { are_empty(&vec![(x + 1, y - 1), (x + 1, *y), (x + 1, y + 1)], map) }),
            Box::new(|x: &i32, y: &i32| -> Point {(x + 1, *y)})
        )
    ];
    let mut elves_moves = HashMap::new();
    for (x, y) in elves {
        let mut elf_move = None;
        for i in round..round + move_evaluation.len() {
            let true_index = i % move_evaluation.len();
            if (move_evaluation[true_index].0)(x, y) {
                elf_move = Some((move_evaluation[true_index].1)(x, y));
                break;
            }
        }
        if elf_move != None {
            elves_moves.entry(elf_move.unwrap()).or_insert_with(|| vec![]).push((*x, *y));
        }
    }
    elves_moves
}

fn expand_boundaries(elves_moves: &HashMap<Point, Vec<Point>>, map: &mut Map) -> (i32, i32) {
    let mut corrected_low_x = 0;
    let mut corrected_high_x = 0;
    let mut corrected_low_y = 0;
    let mut corrected_high_y = 0;
    for (elf_move, elves) in elves_moves {
        if elves.len() == 1 {
            if elf_move.0 < 0 && corrected_low_x == 0 {
                corrected_low_x = 1;
                for j in 0..map.len() {
                    map[j].push_front(EMPTY);
                }
            } else if elf_move.0 + corrected_low_x >= map[0].len() as i32 && corrected_high_x == 0 {
                corrected_high_x = 1;
                for j in 0..map.len() {
                    map[j].push_back(EMPTY);
                }
            }
            if elf_move.1 < 0 && corrected_low_y == 0 {
                corrected_low_y = 1;
                map.push_front(VecDeque::from(vec![EMPTY; map[0].len()]));
            } else if elf_move.1 + corrected_low_y >= map.len() as i32 && corrected_high_y == 0 {
                corrected_high_y = 1;
                map.push_back(VecDeque::from(vec![EMPTY; map[0].len()]));
            }
        }
    }
    (corrected_low_x, corrected_low_y)
}

fn execute_moves(elves_moves: &HashMap<Point, Vec<Point>>, pos_correction: &(i32, i32), map: &mut Map) {
    for (elf_move, elves) in elves_moves {
        if elves.len() == 1 {
            let elf_original_position = (elves[0].0 + pos_correction.0, elves[0].1 + pos_correction.1);
            let elf_next_position = (elf_move.0 + pos_correction.0, elf_move.1 + pos_correction.1);
            map[elf_next_position.1 as usize][elf_next_position.0 as usize] = ELF;
            map[elf_original_position.1 as usize][elf_original_position.0 as usize] = EMPTY;
        }
    }
}

fn are_empty(positions: &Vec<Point>, map: &Map) -> bool {
    positions.iter().all(|(x, y)| out_of_bounds(*x, *y, map) || map[*y as usize][*x as usize] != ELF)
}

fn out_of_bounds(x: i32, y: i32, map: &Map) -> bool {
    x < 0 || x >= map[0].len() as i32 || y < 0 || y >= map.len() as i32
}

#[allow(dead_code)]
fn print_map(map: &Map) {
    for row in map {
        println!("{}", row.iter().collect::<String>());
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day23/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day23/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day23/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

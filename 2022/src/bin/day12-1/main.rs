use std::collections::VecDeque;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

const START_VALUE: char = 'S';
const START_HEIGHT: char = 'a';
const END_VALUE: char = 'E';
const END_HEIGHT: char = 'z';
const NOT_VISITED: i32 = -1;

type HeightMap = Vec<Vec<char>>;
type VisitedMap = Vec<Vec<i32>>;
type Point = (usize, usize);

// I wanted to test macros
macro_rules! direction_check {
    ( $map: expr, $visited: expr, $visit_queue: expr, $steps: expr,
      $pos_height: expr, $boundary_check:expr, $lazy_pos: expr) => {
        if $boundary_check {
            let new_pos = $lazy_pos();
            if can_traverse(&$map, &$visited, $pos_height, new_pos) {
                if $map[new_pos.0][new_pos.1] == END_VALUE {
                    return $steps as u32 + 1
                }
                $visited[new_pos.0][new_pos.1] = $steps + 1;
                $visit_queue.push_back(new_pos);
            }
        }
    }
}

fn solve(lines: Vec<String>) -> u32 {
    let map: Vec<Vec<char>> = lines.iter().map(|x| x.chars().collect()).collect();
    let row_count = map.len();
    let column_count = map[0].len();
    let mut visited = vec![vec![NOT_VISITED; map[0].len()]; row_count];
    let mut visit_queue = VecDeque::new();

    let start = find_start_position(&map);
    visited[start.0][start.1] = 0;
    visit_queue.push_back(start);
    while !visit_queue.is_empty() {
        let pos = visit_queue.pop_front().unwrap();
        let pos_height = position_value(&map, pos);
        let steps = visited[pos.0][pos.1];
        direction_check!(map, visited, visit_queue, steps, pos_height, pos.0 > 0, || {(pos.0 - 1, pos.1) });
        direction_check!(map, visited, visit_queue, steps, pos_height, pos.0 < row_count - 1, || {(pos.0 + 1, pos.1) });
        direction_check!(map, visited, visit_queue, steps, pos_height, pos.1 > 0, || {(pos.0, pos.1 - 1) });
        direction_check!(map, visited, visit_queue, steps, pos_height, pos.1 < column_count - 1, || {(pos.0, pos.1 + 1) });
    }
    panic!("End position not found");
}

fn find_start_position(map: &HeightMap) -> Point {
    for i in 0..map.len() {
        for j in 0..map[0].len() {
            if map[i][j] == START_VALUE {
                return (i, j)
            }
        }
    }
    panic!("Start position not found");
}

fn can_traverse(map: &HeightMap, visited: &VisitedMap, height: char, coord: Point) -> bool {
    visited[coord.0][coord.1] == NOT_VISITED && char::from_u32(height as u32 + 1).unwrap() >= position_value(&map, coord)
}

fn position_value(map: &HeightMap, coord: Point) -> char {
    let value = map[coord.0][coord.1];
    match value {
        START_VALUE => START_HEIGHT,
        END_VALUE => END_HEIGHT,
        _ => value
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day12/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day12/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day12/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

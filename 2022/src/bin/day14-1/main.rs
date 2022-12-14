use std::cmp::min;
use std::cmp::max;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Map = Vec<Vec<char>>;
type Point = (usize, usize);

const SAND_SPAWNER_POS: Point = (500, 0);
const SAND_SPAWNER: char = '+';
const AIR: char = '.';
const ROCK: char = '#';
const SAND: char = 'o';

fn solve(lines: Vec<String>) -> u32 {
    let (mut map, sand_spawner) = init_map(&lines);
    let mut simulation_steps = 0;
    while simulate_step(sand_spawner, &mut map) {
        simulation_steps += 1;
    }
    simulation_steps
}

fn simulate_step(mut sand: Point, map: &mut Map) -> bool {
    let max_y = map.len();
    let max_x = map[0].len();
    let mut can_move = true;
    while can_move {
        if sand.1 + 1 == max_y {
            return false;
        } else if map[sand.1 + 1][sand.0] == AIR {
            sand.1 += 1;
        } else if sand.0 == 0 {
            return false;
        } else if map[sand.1 + 1][sand.0 - 1] == AIR {
            sand.0 -= 1;
            sand.1 += 1;
        } else if sand.0 + 1 == max_x {
            return false;
        } else if map[sand.1 + 1][sand.0 + 1] == AIR {
            sand.0 += 1;
            sand.1 += 1;
        } else {
            can_move = false;
            map[sand.1][sand.0] = SAND;
        }
    }
    return true
}

fn init_map(lines: &Vec<String>) -> (Map, Point) {
    let rock_paths: Vec<Vec<Point>> = lines.iter().map(|line| parse_line(line.as_str()) ).collect();
    let (min_point, max_point) = get_boundaries(&rock_paths);
    let mut map: Map = vec!(vec!(AIR; max_point.0 - min_point.0 + 1); max_point.1 + 1);
    let normalized_spawner_pos = (SAND_SPAWNER_POS.0 - min_point.0, SAND_SPAWNER_POS.1);
    map[normalized_spawner_pos.1][normalized_spawner_pos.0] = SAND_SPAWNER;
    for path in rock_paths {
        path.windows(2).for_each(|points| {
            for j in min(points[0].1,points[1].1)..=max(points[0].1, points[1].1) {
                for i in min(points[0].0, points[1].0)..=max(points[0].0, points[1].0) {
                    map[j][i - min_point.0] = ROCK;
                }
            }
        })
    }
    (map, normalized_spawner_pos)
}

fn parse_line(line: &str) -> Vec<Point> {
    line.split(" -> ").into_iter().map(|x| {
        let mut splitted = x.split(',');
        (splitted.next().unwrap().parse().unwrap(), splitted.next().unwrap().parse().unwrap())
    }).collect()
}

fn get_boundaries(rock_paths: &Vec<Vec<Point>>) -> (Point, Point) {
    rock_paths.iter().flatten()
        .fold(((usize::MAX, usize::MAX), (0, 0)), |acc, point| {
            let min_point = acc.0;
            let max_point = acc.1;
            (
                (min(min_point.0, point.0), min(min_point.1, point.1)),
                (max(max_point.0, point.0), max(max_point.1, point.1))
            )
        })
}

#[allow(dead_code)]
fn print_map(map: &Map) {
    for row in map {
        println!("{}", row.iter().collect::<String>())
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day14/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day14/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day14/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

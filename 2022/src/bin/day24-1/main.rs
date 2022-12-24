use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Point = (i32, i32);
type Map = Vec<Vec<bool>>;
type Blizzards = Vec<(Point, Point)>;

const UP: char = '^';
const DOWN: char = 'v';
const LEFT: char = '<';
const RIGHT: char = '>';


fn solve(lines: Vec<String>) -> u32 {
    let y_size = lines.len();
    let x_size = lines[0].chars().count();
    let blizzards = parse_blizzards(&lines);
    let mut map = vec![vec![false; x_size]; y_size];
    let mut minute = 1;
    loop {
        map[0][1] = true;
        if map[y_size - 2][x_size -2] {
            return minute;
        }
        map = paint_map(&map);
        delete_map(minute, &blizzards, &mut map);
        minute += 1;
    }
}

fn parse_blizzards(lines: &Vec<String>) -> Blizzards {
    let mut blizzards =  Blizzards::new();
    for i in 0..lines.len() {
        for (j, char) in lines[i].chars().enumerate() {
            if char == LEFT || char == RIGHT {
                let direction = if char == LEFT { (-1, 0) } else { (1, 0) };
                blizzards.push((direction, (j as i32, i as i32)));
            } else if char == UP || char == DOWN {
                let direction = if char == UP { (0, -1) } else { (0, 1) };
                blizzards.push((direction, (j as i32, i as i32)));
            }
        }
    }
    blizzards
}

fn paint_map(map: &Map) -> Map {
    let y_size = map.len();
    let x_size = map[0].len();
    let mut next_map = vec![vec![false; x_size]; y_size];
    for i in 1..y_size-1 {
        for j in 1..x_size-1 {
            next_map[i][j] = map[i][j] || map[i+1][j] || map[i-1][j] || map[i][j-1] || map[i][j+1]
        }
    }
    next_map
}

fn delete_map(minute: u32, blizzards: &Blizzards, map: &mut Map, ) {
    let y_size = map.len() as i32;
    let x_size = map[0].len() as i32;
    for (direction, origin) in blizzards {
        let mut new_y = (origin.1 - 1 + direction.1 * minute as i32) % (y_size - 2);
        let mut new_x = (origin.0 - 1 + direction.0 * minute as i32) % (x_size - 2);
        if new_x >= 0 {
            new_x += 1;
        } else if new_x < 0 {
            new_x = x_size - 1 + new_x;
        }
        if new_y >= 0 {
            new_y += 1;
        } else if new_y < 0 {
            new_y = y_size - 1 + new_y;
        }
        map[new_y as usize][new_x as usize] = false;
    }
}

#[allow(dead_code)]
fn print_map(map: &Map) {
    for i in 0..map.len() {
        for j in 0..map[i].len() {
            let symbol = if map[i][j] { 'x' } else { '.' };
            print!("{}", symbol);
        }
        println!()
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day24/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day24/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day24/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}
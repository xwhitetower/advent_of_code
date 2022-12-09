use std::collections::HashSet;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

#[derive(PartialEq, Eq, Hash, Debug, Copy, Clone)]
struct Point {
    x: i32,
    y: i32
}

impl Point {
    fn execute_command(&mut self, command: &str) {
        match command {
            "U" => self.y += 1,
            "D" => self.y -= 1,
            "L" => self.x -= 1,
            "R" => self.x += 1,
            _ => panic!("Unxepected command {}", command),
        }
    }
    fn follow(&mut self, head: &Point) {
        let x_distance = self.x - head.x;
        let y_distance = self.y - head.y;
        if x_distance.abs() > 1 || y_distance.abs() > 1 {
            self.x += -x_distance.clamp(-1, 1);
            self.y += -y_distance.clamp(-1, 1);
        }
    }
}

fn solve(lines: Vec<String>) -> u32 {
    let tail_positions = move_rope(&lines);
    if cfg!(debug_assertions) {
        print_path(&tail_positions)
    }
    tail_positions.len() as u32
}

fn move_rope(lines: &Vec<String>) -> HashSet<Point> {
    let mut head = Point{x: 0, y: 0};
    let mut knots = vec![Point{x: 0, y: 0}; 9];
    let mut tail_positions = HashSet::new();
    tail_positions.insert(knots.last().unwrap().clone());
    for line in lines {
        let instruction: Vec<&str> = line.split(' ').collect();
        let command = instruction[0];
        let amount = instruction[1].parse::<usize>().unwrap();
        for _ in 0..amount {
            head.execute_command(command);
            knots.iter_mut().fold(head, |prev, knot| {
                knot.follow(&prev);
                *knot
            });
            tail_positions.insert(knots.last().unwrap().clone());
        }
    }

    tail_positions
}

fn print_path(tail_positions: &HashSet<Point>) {
    let x_min = tail_positions.iter().map(|p| p.x).min().unwrap();
    let x_max = tail_positions.iter().map(|p| p.x).max().unwrap();
    let y_min = tail_positions.iter().map(|p| p.y).min().unwrap();
    let y_max = tail_positions.iter().map(|p| p.y).max().unwrap();
    for y in (y_min..=y_max).rev() {
        for x in x_min..=x_max {
            let char = match tail_positions.get(&Point {x: x as i32, y: y as i32}) {
                None => '.',
                _ => '#'
            };
            print!("{}", char);
        }
        println!();
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let timer = Instant::now();
    println!("{}", solve(read_lines("./data/day9/input.txt")?));
    println!("total time [{:.2?}]", timer.elapsed());
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day9/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day9/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

#[test]
fn extra_example() {
    let lines = vec![
        "R 5",
        "U 8",
        "L 8",
        "D 3",
        "R 17",
        "D 10",
        "L 25",
        "U 20"
    ].into_iter().map(|x| x.to_string()).collect();
    assert_eq!(solve(lines), 36);
}

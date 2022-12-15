use std::cmp::max;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Point = (i32, i32);
type Range = (i32, i32);

fn solve(lines: Vec<String>, search_y: i32) -> u32 {
    let mut free_ranges: Vec<Range> = Vec::new();
    for line in lines {
        let (sensor, beacon) = parse_line(&line.as_str());
        let distance = (sensor.0 - beacon.0).abs() + (sensor.1 - beacon.1).abs();
        if sensor.1 - distance <= search_y && sensor.1 + distance >= search_y {
            let offset = distance - (sensor.1 - search_y).abs();
            free_ranges.push((sensor.0 - offset, sensor.0 + offset));
        }
    }
    merge_ranges(&mut free_ranges).iter()
        .fold(0, |acc, range| acc + (range.1 - range.0) as u32)
}

fn parse_line(line: &str) -> (Point, Point) {
    let first_split: Vec<&str> = line[12..].split(", y=").collect();
    let second_split: Vec<&str> = first_split[1].split(": closest beacon is at x=").collect();

    (
        (first_split[0].parse().unwrap(), second_split[0].parse().unwrap()),
        (second_split[1].parse().unwrap(), first_split[2].parse().unwrap()))
}

fn merge_ranges(free_ranges: &mut Vec<Range>) -> Vec<Range> {
    free_ranges.sort_by(|a, b| a.0.cmp(&b.0));
    let mut merged_ranges: Vec<Range> = Vec::new();
    for range in free_ranges {
        if merged_ranges.is_empty() {
            merged_ranges.push(*range);
        } else {
            let last_range = merged_ranges.pop().unwrap();
            if range.0 <= last_range.1 {
                merged_ranges.push((last_range.0, max(last_range.1, range.1)));
            } else {
                merged_ranges.push(last_range);
                merged_ranges.push(*range);
            }
        }
    }
    merged_ranges
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day15/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines, 2_000_000);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day15/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day15/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines, 10), expect);
}

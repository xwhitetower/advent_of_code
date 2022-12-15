use std::cmp::max;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Point = (i32, i32);
type Range = (i32, i32);

fn solve(lines: Vec<String>, limit: i32) -> u64 {
    let sensors: Vec<(Point, Point)> = lines.iter().map(|line| parse_line(&line.as_str())).collect();
    let x_accessor = |x: &Point| x.0;
    let y_accessor = |x: &Point| x.1;
    let x = find_suspicios_space(limit, &sensors, &y_accessor, &x_accessor);
    let y = find_suspicios_space(limit, &sensors, &x_accessor, &y_accessor);
    (x * 4_000_000 + y) as u64
}

fn parse_line(line: &str) -> (Point, Point) {
    let first_split: Vec<&str> = line[12..].split(", y=").collect();
    let second_split: Vec<&str> = first_split[1].split(": closest beacon is at x=").collect();

    (
        (first_split[0].parse().unwrap(), second_split[0].parse().unwrap()),
        (second_split[1].parse().unwrap(), first_split[2].parse().unwrap()))
}

fn find_suspicios_space(
    limit: i32, sensors: &Vec<(Point, Point)>,
    distance_check_accessor: &dyn Fn(&Point) -> i32,
    range_push_accessor: &dyn Fn(&Point) -> i32,
) -> u64 {
    for i in 0..limit {
        let mut free_ranges: Vec<Range> = get_ranges(sensors, i, distance_check_accessor, range_push_accessor);
        for range in merge_ranges(&mut free_ranges) {
            if range.0 > 0 && range.0 < limit {
                return (range.0 - 1) as u64
            }
        }
    }
    panic!("No suspicios activity");
}

fn get_ranges(
    sensors: &Vec<(Point, Point)>,
    search_value: i32,
    distance_check_accessor: &dyn Fn(&Point) -> i32,
    range_push_accessor: &dyn Fn(&Point) -> i32
) -> Vec<Range> {
    let mut free_ranges: Vec<Range> = Vec::new();
    for (sensor, beacon) in sensors {
        let distance = (sensor.0 - beacon.0).abs() + (sensor.1 - beacon.1).abs();
        if distance_check_accessor(sensor) - distance <= search_value && distance_check_accessor(sensor) + distance >= search_value {
            let offset = distance - (distance_check_accessor(sensor) - search_value).abs();
            free_ranges.push((range_push_accessor(sensor) - offset, range_push_accessor(sensor) + offset));
        }
    }
    free_ranges
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
    let solution =  solve(lines, 4_000_000);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day15/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day15/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines, 20), expect);
}

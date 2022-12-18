use std::cmp::min;
use std::cmp::max;
use std::collections::HashSet;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Point = (i32, i32, i32);

fn solve(lines: Vec<String>) -> u32 {
    let cubes: HashSet<Point> = lines.iter().map(|line| {
        let coords: Vec<i32> = line.split(',').map(|x| x.parse::<i32>().unwrap()).collect();
        (coords[0], coords[1], coords[2])
    }).collect();
    let x_range = cubes.iter().fold((i32::MAX, i32::MIN), |acc, x| {
        (min(acc.0, x.0), max(acc.1, x.0))
    });
    let y_range = cubes.iter().fold((i32::MAX, i32::MIN), |acc, x| {
        (min(acc.0, x.1), max(acc.1, x.1))
    });
    let z_range = cubes.iter().fold((i32::MAX, i32::MIN), |acc, x| {
        (min(acc.0, x.2), max(acc.1, x.2))
    });

    let mut exposed_faces: u32 = 0;
    let mut to_explore_queue: Vec<Point> = Vec::new();
    let mut explored: HashSet<Point> = HashSet::new();
    let start_point = (x_range.0 - 1, y_range.0 - 1, z_range.0 - 1);
    to_explore_queue.push(start_point);
    explored.insert(start_point);
    let x_accessor = |x: &Point| x.0;
    let y_accessor = |x: &Point| x.1;
    let z_accessor = |x: &Point| x.2;
    while !to_explore_queue.is_empty() {
        let cube = to_explore_queue.pop().unwrap();

        exposed_faces += explore_cube((cube.0 - 1, cube.1, cube.2), &cubes, &mut explored, &mut to_explore_queue, &x_accessor, &x_range);
        exposed_faces += explore_cube((cube.0 + 1, cube.1, cube.2), &cubes, &mut explored, &mut to_explore_queue, &x_accessor, &x_range);
        exposed_faces += explore_cube((cube.0, cube.1 - 1, cube.2), &cubes, &mut explored, &mut to_explore_queue, &y_accessor, &y_range);
        exposed_faces += explore_cube((cube.0, cube.1 + 1, cube.2), &cubes, &mut explored, &mut to_explore_queue, &y_accessor, &y_range);
        exposed_faces += explore_cube((cube.0, cube.1, cube.2 - 1), &cubes, &mut explored, &mut to_explore_queue, &z_accessor, &z_range);
        exposed_faces += explore_cube((cube.0, cube.1, cube.2 + 1), &cubes, &mut explored, &mut to_explore_queue, &z_accessor, &z_range);
    }
    exposed_faces
}

fn explore_cube(
    cube: Point, cubes: &HashSet<Point>,
    explored: &mut HashSet<Point>,
    to_explore_queue: &mut Vec<Point>,
    accessor: &dyn Fn(&Point) -> i32,
    axis_range: &(i32, i32)
) -> u32 {
    if cubes.contains(&cube) {
        return 1
    }
    if accessor(&cube) >= axis_range.0 - 1 && accessor(&cube) <= axis_range.1 + 1
        && !explored.contains(&cube)
    {
        explored.insert(cube);
        to_explore_queue.push(cube);
    }
    0
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day18/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day18/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day18/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

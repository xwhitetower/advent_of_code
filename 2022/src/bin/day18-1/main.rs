use std::collections::HashSet;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;


fn solve(lines: Vec<String>) -> u32 {
    let cubes: HashSet<(i32, i32, i32)> = lines.iter().map(|line| {
        let coords: Vec<i32> = line.split(',').map(|x| x.parse::<i32>().unwrap()).collect();
        (coords[0], coords[1], coords[2])
    }).collect();
    let mut exposed_faces = 0;
    for cube in &cubes {
        if !cubes.contains(&(cube.0 - 1, cube.1, cube.2)) {
            exposed_faces += 1;
        }
        if !cubes.contains(&(cube.0 + 1, cube.1, cube.2)) {
            exposed_faces += 1;
        }
        if !cubes.contains(&(cube.0, cube.1 - 1, cube.2)) {
            exposed_faces += 1;
        }
        if !cubes.contains(&(cube.0, cube.1 + 1, cube.2)) {
            exposed_faces += 1;
        }
        if !cubes.contains(&(cube.0, cube.1, cube.2 - 1)) {
            exposed_faces += 1;
        }
        if !cubes.contains(&(cube.0, cube.1, cube.2 + 1)) {
            exposed_faces += 1;
        }
    }
    exposed_faces
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
    let expected_lines = read_lines("./data/day18/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

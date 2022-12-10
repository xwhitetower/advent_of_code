use std::cmp::max;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> u32 {
    let trees: Vec<Vec<char>> = lines.iter().map(|x| x.chars().collect()).collect();
    highest_scenic_score(&trees)
}

fn highest_scenic_score(trees: &Vec<Vec<char>>) -> u32 {
    let mut max_score: u32 = 0;

    let row_count = trees.len();
    let column_count = trees[0].len();
    for i in 0..row_count {
        for j in 0..column_count {
            max_score = max(max_score, tree_scenic_score(trees, i, j));
        }
    }
    max_score
}

fn tree_scenic_score(trees: &Vec<Vec<char>>, row: usize, column: usize) -> u32 {
    let tree = trees[row][column];
    let row_access = |i: usize| -> char { trees[i][column] };
    let colmun_access = |i: usize| -> char { trees[row][i] };
    viewing_distance(tree, (0..row).rev(), &row_access )
        * viewing_distance(tree, row + 1..trees.len(), &row_access )
        * viewing_distance(tree, (0..column).rev(), &colmun_access )
        * viewing_distance(tree, column + 1..trees[0].len(), &colmun_access )
}

fn viewing_distance<T: Iterator<Item = usize>>(
    tree: char,
    iterator: T,
    tree_accessor: &dyn Fn(usize) -> char
) -> u32 {
    let mut distance = 0;
    for i in iterator {
        distance  += 1;
        if tree_accessor(i) >= tree {
            break;
        }
    }
    distance
}

fn main() -> Result<(), Box<dyn Error>> {
    let timer = Instant::now();
    println!("{}", solve(read_lines("./data/day8/input.txt")?));
    println!("total time [{:.2?}]", timer.elapsed());
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day8/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day8/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

use std::cmp::max;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> u32 {
    let trees: Vec<Vec<char>> = lines.iter().map(|x| x.chars().collect()).collect();
    let visible_trees = mark_tree_visibility(trees);
    visible_trees.iter().map(|x| x.iter().filter(|x| **x).count() as u32 ).sum()
}

fn mark_tree_visibility(trees: Vec<Vec<char>>) -> Vec<Vec<bool>> {
    let mut visible_trees = vec![vec![false; trees[0].len()]; trees.len()];

    let row_count = trees.len();
    let column_count = trees[0].len();
    // This is ugly and repeated. Not enought time to DRY it
    for i in 0..row_count {
        let mut min_height = ('0' as u8 - 1) as char;
        for j in 0..column_count {
            let tree = trees[i][j];
            visible_trees[i][j] = visible_trees[i][j] || tree > min_height;
            min_height = max(min_height, tree)
        }
    }
    for i in 0..row_count {
        let mut min_height = ('0' as u8 - 1) as char;
        for j in (0..column_count).rev() {
            let tree = trees[i][j];
            visible_trees[i][j] = visible_trees[i][j] || tree > min_height;
            min_height = max(min_height, tree)
        }
    }
    for j in 0..column_count {
        let mut min_height = ('0' as u8 - 1) as char;
        for i in 0..row_count {
            let tree = trees[i][j];
            visible_trees[i][j] = visible_trees[i][j] || tree > min_height;
            min_height = max(min_height, tree)
        }
    }
    for j in 0..column_count {
        let mut min_height = ('0' as u8 - 1) as char;
        for i in (0..row_count).rev() {
            let tree = trees[i][j];
            visible_trees[i][j] = visible_trees[i][j] || tree > min_height;
            min_height = max(min_height, tree)
        }
    }

    visible_trees
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day8/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day8/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day8/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

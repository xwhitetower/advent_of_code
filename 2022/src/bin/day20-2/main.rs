use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

static DECRPYPTION_KEY: i64 = 811_589_153;
static ROUNDS: usize = 10;

fn solve(lines: Vec<String>) -> i64 {
    let mut mixer: Vec<(usize, i64)> = lines.iter()
        .enumerate()
        .map(|(i, x)| (i, x.parse::<i64>().unwrap() * DECRPYPTION_KEY))
        .collect();
    for _ in 0..ROUNDS {
        for i in 0..mixer.len() {
            let mut index = mixer.iter().position(|x| x.0 == i).unwrap();
            let value = mixer[index].1;
            for _ in 0..(value % (mixer.len() - 1) as i64).abs() {
                let next_pos: usize = if value.is_positive() {
                    (index + 1) % mixer.len()
                } else if index == 0 {
                    mixer.len() - 1
                } else {
                    index - 1
                };
                mixer[index] = mixer[next_pos];
                mixer[next_pos] = (i, value);
                index = next_pos;
            }
        }
    }
    let zero_index = mixer.iter().position(|x| x.1 == 0).unwrap();
    vec![1000, 2000, 3000].iter().fold(0, |acc, x| acc + mixer[(zero_index + x) % mixer.len()].1)
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day20/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day20/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day20/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<i64>().unwrap();
    assert_eq!(solve(lines), expect);
}

use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

fn solve(lines: Vec<String>) -> u32 {
    let mut smaller_than_2 = 0;
    let mut smaller_than_6 = 0;
    lines.iter()
        .filter(|x| !x.is_empty())
        .for_each(|line| {
            let digit_pos = line.find(|c| c >= '0' && c <= '9');
            let close_pos = line.find(']');
            if digit_pos == None || (close_pos != None && close_pos.unwrap() < digit_pos.unwrap()) {
                smaller_than_2 += 1;
                smaller_than_6 += 1;
            } else {
                let pos = digit_pos.unwrap();
                if (close_pos == None || pos < close_pos.unwrap()) && (pos == line.len() - 1
                        || !(line[pos+1..].chars().next().unwrap() >= '0' && line[pos+1..].chars().next().unwrap() <= '9')
                    ) {
                    let current_char = line[pos..].chars().next().unwrap();
                    if current_char < '2' { smaller_than_2 += 1 }
                    if current_char < '6' { smaller_than_6 += 1 }
                }
            }
        });
    (smaller_than_2 + 1) * (smaller_than_6 + 2)
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day13/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day13/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day13/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

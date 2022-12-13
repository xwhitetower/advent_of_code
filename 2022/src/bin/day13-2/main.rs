use std::cmp::min;
use std::error::Error;
use std::time::Instant;
use std::cmp::Ordering;

use advent_of_code::read_lines;

#[derive(Debug)]
enum Signal {
    Integer(i32),
    Vector(Vec<Signal>)
}

impl Signal {
    fn is_divider_signal(self: &Self) -> bool {
        match self {
            Signal::Vector(raw_0) => {
                if raw_0.len() != 1 { return false }
                match &raw_0[0] {
                    Signal::Vector(raw_1) => {
                        if raw_1.len() != 1 { return false }
                         match raw_1[0] {
                             Signal::Integer(raw_2) => raw_2 == 2 || raw_2 == 6,
                             _ => false
                         }
                    },
                    _ => false
                }
            },
            _ => false
        }
    }
}

fn solve(mut lines: Vec<String>) -> u32 {
    lines.push("[[2]]".to_string());
    lines.push("[[6]]".to_string());
    let mut packets: Vec<Signal> = lines.iter()
        .filter(|x| !x.is_empty())
        .map(|x| parse_packet(x))
        .collect();
    packets.sort_by(|a, b| cmp(&a, &b));
    packets.iter().enumerate()
        .filter(|(_, s)| s.is_divider_signal())
        .fold(1, |acc, (i, _)| acc * (i + 1) as u32 )
}

fn parse_packet(packet: &String) -> Signal {
    internal_parsing(packet).0
}

fn internal_parsing(packet: &String) -> (Signal, usize) {
    if packet.chars().next().unwrap() == '[' {
        let mut next_position = 1;
        let mut vector_signal = Vec::new();
        while get_first_char(&packet[next_position..]) != ']' {
            let (signal, end_position) = internal_parsing(&packet[next_position..].to_string());
            next_position += end_position;
            if get_first_char(&packet[next_position..]) == ',' { next_position += 1 };
            vector_signal.push(signal);
        }
        (Signal::Vector(vector_signal), next_position + 1)
    } else {
        let end = get_integer_end(packet);
        (Signal::Integer(packet[..end].parse().unwrap()), end)
    }
}

fn get_first_char(str: &str) -> char {
    str.chars().next().unwrap_or(']')
}

fn get_integer_end(packet: &String) -> usize {
    packet.find(|c| c == ',' || c == ']').unwrap_or(packet.len())
}

fn cmp(left: &Signal, right: &Signal) -> Ordering {
    match left {
        Signal::Integer(raw_left) => {
            match right {
                Signal::Integer(raw_right) => raw_left.cmp(raw_right),
                Signal::Vector(_) => cmp(&Signal::Vector(vec![Signal::Integer(*raw_left)]), &right)
            }
        }
        Signal::Vector(raw_left) => {
            match right {
                Signal::Integer(raw_right) => cmp(&left, &Signal::Vector(vec![Signal::Integer(*raw_right)])),
                Signal::Vector(raw_right) => {
                    for i in 0..min(raw_left.len(), raw_right.len()) {
                        let order = cmp(&raw_left[i], &raw_right[i]);
                        if order != Ordering::Equal { return order }
                    }
                    raw_left.len().cmp(&raw_right.len())
                }
            }
        }
    }
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

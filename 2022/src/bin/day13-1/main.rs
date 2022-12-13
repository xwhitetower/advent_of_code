use std::cmp::min;
use std::error::Error;
use std::time::Instant;
use std::cmp::Ordering;

use advent_of_code::read_lines;

enum Signal {
    Integer(i32),
    Vector(Vec<Signal>)
}

impl Ord for Signal {
    fn cmp(&self, other: &Self) -> Ordering {
        match self {
            Signal::Integer(raw_left) => {
                match other {
                    Signal::Integer(raw_right) => raw_left.cmp(raw_right),
                    Signal::Vector(_) => Signal::Vector(vec![Signal::Integer(*raw_left)]).cmp(&self)
                }
            }
            Signal::Vector(raw_left) => {
                match other {
                    Signal::Integer(raw_right) => self.cmp(&Signal::Vector(vec![Signal::Integer(*raw_right)])),
                    Signal::Vector(raw_right) => {
                        for i in 0..min(raw_left.len(), raw_right.len()) {
                            let order = raw_left[i].cmp(&raw_right[i]);
                            if order != Ordering::Equal { return order }
                        }
                        raw_left.len().cmp(&raw_right.len())
                    }
                }
            }
        }
    }
}

impl PartialOrd for Signal {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for Signal {
    fn eq(&self, other: &Self) -> bool {
        self.cmp(other) == Ordering::Equal
    }
}
impl Eq for Signal {}

fn solve(lines: Vec<String>) -> u32 {
    let mut solution = 0;
    lines.chunks(3).enumerate().for_each(|(i, packets)| {
        let left = parse_packet(&packets[0]);
        let right = parse_packet(&packets[1]);
        if left < right {
            solution += 1 + i as u32;
        }
    });
    solution
}

fn parse_packet(packet: &String) -> Signal {
    internal_parsing(&packet.as_str()).0
}

fn internal_parsing(packet: &str) -> (Signal, usize) {
    if packet.chars().next().unwrap() == '[' {
        let mut next_position = 1;
        let mut vector_signal = Vec::new();
        while get_first_char(&packet[next_position..]) != ']' {
            let (signal, end_position) = internal_parsing(&packet[next_position..]);
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

fn get_integer_end(packet: &str) -> usize {
    packet.find(|c| c == ',' || c == ']').unwrap_or(packet.len())
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
    let expected_lines = read_lines("./data/day13/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

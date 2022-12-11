use std::collections::HashMap;
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

static ROUNDS: usize = 10_000;

struct Monke {
    items: Vec<u64>,
    operation: MonkeOperation,
    tester: MonkeTester,
    inspection_count: u64
}

struct MonkeOperation {
    left: Box<dyn Fn(u64) -> u64>,
    op: String,
    right: Box<dyn Fn(u64) -> u64>,
}

struct MonkeTester {
    divisor: u64,
    true_monkey: usize,
    false_monkey: usize
}

impl Monke {
    fn check_items(&mut self, mcm: u64, thrown_items: &mut HashMap<usize, Vec<u64>>) {
        self.inspection_count += self.items.len() as u64;
        self.items.iter()
            .map(|x| self.operation.execute(*x) )
            .map(|x| x % mcm)
            .for_each(|x| { self.tester.test(x, thrown_items); });
        self.items.clear();
    }
}

impl MonkeOperation {
    fn execute(&self, old: u64) -> u64 {
        let left = (self.left)(old);
        let right =  (self.right)(old);
        match self.op.as_str() {
            "+" => left + right,
            "*" => left * right,
            _ => panic!("Unknown operation: {}", self.op)
        }
    }
}

impl MonkeTester {
    fn test(&self, item: u64, thrown_items: &mut HashMap<usize, Vec<u64>>) {
        let monke = if item % self.divisor == 0 {
            self.true_monkey
        } else {
            self.false_monkey
        };
        thrown_items.get_mut(&monke).unwrap().push(item);
    }
}

fn solve(lines: Vec<String>) -> u64 {
    let mut monkes = lines_to_monkes(&lines);
    let mcm = monkes.iter().fold(1, |acc, m| acc * m.tester.divisor);
    let mut thrown_items = init_thrown_items(&monkes);
    for _ in 0..ROUNDS {
        for i in 0..monkes.len() {
            monkes[i].items.append(thrown_items.get_mut(&i).unwrap());
            monkes[i].check_items(mcm, &mut thrown_items);
        }
    }
    monkes.sort_by(|l, r| r.inspection_count.cmp(&l.inspection_count));
    monkes.iter()
        .take(2)
        .fold(1, |acc, monke| acc * monke.inspection_count )
}

fn lines_to_monkes(lines: &Vec<String>) -> Vec<Monke>{
    (0..lines.len())
        .step_by(7)
        .map(|x| to_monke(x, lines) )
        .collect()
}

fn to_monke(i: usize, lines: &Vec<String>) -> Monke {
    let items = lines[i + 1][18..]
        .split(',')
        .map(|x| x.trim().parse::<u64>().unwrap())
        .collect();
    let operation_items: Vec<String> = lines[i + 2][19..].split(' ')
        .map(|x| x.to_string())
        .collect();
    Monke {
        items,
        operation: MonkeOperation {
            left: parse_operator(operation_items[0].as_str()),
            op: operation_items[1].clone(),
            right: parse_operator(operation_items[2].as_str()),
        },
        tester: MonkeTester {
            divisor: lines[i + 3][21..].parse::<u64>().unwrap(),
            true_monkey: lines[i + 4][29..].parse::<usize>().unwrap(),
            false_monkey: lines[i + 5][30..].parse::<usize>().unwrap(),
        },
        inspection_count: 0
    }
}

fn parse_operator(operator: &str) -> Box<dyn Fn(u64) -> u64> {
    if operator.starts_with("o") {
        Box::new(|x|-> u64 { x })
    } else {
        let value = operator.parse::<u64>().unwrap();
        Box::new(move |_| -> u64 { value })
    }
}

fn init_thrown_items(monkes: &Vec<Monke>) -> HashMap<usize, Vec<u64>> {
    let mut thrown_items = HashMap::new();
    for i in 0..monkes.len() {
        thrown_items.insert(i, Vec::new());
    }
    thrown_items
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day11/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day11/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day11/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u64>().unwrap();
    assert_eq!(solve(lines), expect);
}

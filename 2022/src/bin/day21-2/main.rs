use std::collections::HashMap;
use std::error::Error;
use std::string::ToString;
use std::time::Instant;

use advent_of_code::read_lines;

static HUMAN: &str = "humn";

#[derive(Debug)]
struct Monke {
    number: Option<i64>,
    left: Option<String>,
    op: Option<String>,
    right: Option<String>
}

impl Monke {
    fn has_human(self: &Self) -> bool {
        (self.left != None && self.left.as_ref().unwrap().as_str() == HUMAN)
            || (self.right != None && self.right.as_ref().unwrap().as_str() == HUMAN)
    }

    fn knows_maths(self: &Self) -> bool {
        self.number == None
    }

    fn knows_number(self: &Self) -> bool {
        self.number != None
    }
}

fn solve(lines: Vec<String>) -> i64 {
    let mut monke_yells: HashMap<String, Monke> = HashMap::new();
    for line in lines {
        let parts: Vec<&str> = line.split(": ").collect();
        let words: Vec<&str> = parts[1].split(" ").collect();
        let monke = if words.len() == 1 {
            Monke {
                number: Some(words[0].parse::<i64>().unwrap()),
                left: None,
                op: None,
                right: None
            }
        } else {
            Monke {
                number: None,
                left: Some(words[0].to_string()),
                op: Some(words[1].to_string()),
                right: Some(words[2].to_string())
            }
        };
        monke_yells.insert(parts[0].to_string(), monke);
    }
    let mut yell_cache: HashMap<String, i64> = HashMap::new();
    let mut human_cache: HashMap<String, bool> = HashMap::new();
    let root = monke_yells.get(&"root".to_string()).unwrap();
    if has_human(&root.left.as_ref().unwrap(), &monke_yells, &mut human_cache) {
        let known_value = yell(&root.right.as_ref().unwrap(), &monke_yells, &mut yell_cache);
        human_yell(known_value, &root.left.as_ref().unwrap(), &monke_yells, &mut yell_cache, &mut human_cache)
    } else {
        let known_value = yell(&root.left.as_ref().unwrap(), &monke_yells, &mut yell_cache);
        human_yell(known_value, &root.right.as_ref().unwrap(), &monke_yells, &mut yell_cache, &mut human_cache)
    }
}

fn has_human(monke_name: &String, monke_yells: &HashMap<String, Monke>, human_cache: &mut HashMap<String, bool>) -> bool {
    match human_cache.get(monke_name) {
        None => {
            let monke = monke_yells.get(monke_name).unwrap();
            let humanity_check = monke.has_human()
                || (monke.knows_maths()
                  && (has_human(&monke.left.as_ref().unwrap(), monke_yells, human_cache) || has_human(&monke.right.as_ref().unwrap(), monke_yells, human_cache)));
            human_cache.insert(monke_name.clone(), humanity_check);
            humanity_check
        },
        Some(humanity_check) => *humanity_check
    }
}

fn yell(monke_name: &String, monke_yells: &HashMap<String, Monke>, yell_cache: &mut HashMap<String, i64>) -> i64 {
    match yell_cache.get(monke_name) { // Not using entry api due borrow issues on having the recursive calls
        None => {
            let monke = monke_yells.get(monke_name).unwrap();
            if monke.knows_number() {
                monke.number.unwrap()
            } else {
                let left_name = monke.left.as_ref().unwrap();
                let left = yell(left_name, monke_yells, yell_cache);
                yell_cache.insert(left_name.clone(), left);
                let right_name = monke.right.as_ref().unwrap();
                let right = yell(right_name, monke_yells, yell_cache);
                yell_cache.insert(right_name.clone(), right);
                match monke.op.as_ref().unwrap().as_str() {
                    "+" => left + right,
                    "-" => left - right,
                    "*" => left * right,
                    "/" => left / right,
                    _ => panic!("Unexpected operator: {:?}", monke.op)
                }
            }
        },
        Some(number) => *number
    }
}

fn human_yell(math_result: i64, monke_name: &String, monke_yells: &HashMap<String, Monke>, yell_cache: &mut HashMap<String, i64>, human_cache: &mut HashMap<String, bool>) -> i64 {
    if monke_name.as_str() == HUMAN {
        return math_result;
    }
    let monke = monke_yells.get(monke_name).unwrap();
    let left = monke.left.as_ref().unwrap();
    let op = monke.op.as_ref().unwrap();
    let right = monke.right.as_ref().unwrap();
    if left.as_str() == HUMAN || has_human(left, monke_yells, human_cache) {
        let known_value = yell(right, monke_yells, yell_cache);
        let sub_result = match op.as_str() {
            "+" => math_result - known_value,
            "-" => math_result + known_value,
            "*" => math_result / known_value,
            "/" => math_result * known_value,
            _ => panic!("Unexpected operator: {:?}", op)
        };
        human_yell(sub_result, left, monke_yells, yell_cache, human_cache)
    } else {
        let known_value = yell(left, monke_yells, yell_cache);
        let sub_result = match op.as_str() {
            "+" => math_result - known_value,
            "-" => known_value - math_result,
            "*" => math_result / known_value,
            "/" => known_value / math_result,
            _ => panic!("Unexpected operator: {:?}", op)
        };
        human_yell(sub_result, right, monke_yells, yell_cache, human_cache)
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day21/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day21/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day21/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<i64>().unwrap();
    assert_eq!(solve(lines), expect);
}

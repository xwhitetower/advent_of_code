use std::error::Error;

use advent_of_code::read_lines;


fn solve(lines: Vec<String>) -> String {
    let position = lines.iter().position(|line| line == "").unwrap();
    let mut cargo = init_cargo(&lines[..position]);
    move_cargo(&mut cargo, &lines[position+1..]);
    get_solution(&cargo)
}

fn init_cargo(lines: &[String]) -> Vec<Vec<char>> {
    let mut line_iter = lines.iter().rev();
    let stack_count = line_iter.next().unwrap().split_whitespace().rev().next().unwrap().parse::<usize>().unwrap();
    let mut stacks: Vec<Vec<char>> = vec![Vec::new(); stack_count];

    for line in line_iter {
        for (index, char) in line.chars()
            .skip(1)
            .step_by(4)
            .enumerate()
            .filter(|(index, _)| index < &stack_count) {

            if char.is_alphabetic() {
                stacks[index].push(char);
            }
        }
    }
    stacks
}

fn move_cargo(cargo: &mut Vec<Vec<char>>, instructions: &[String]) {
    for instruction in instructions {
        let instruction_data: Vec<usize> = instruction.split(' ')
            .skip(1)
            .step_by(2)
            .take(3)
            .map(|x| x.parse::<usize>().unwrap() )
            .collect();
        (0..instruction_data[0]).map(|_| cargo[instruction_data[1] - 1].pop().unwrap())
            .collect::<Vec<char>>()
            .iter()
            .rev()
            .for_each(|container| cargo[instruction_data[2] - 1].push(*container) );
    }
}

fn get_solution(cargo: &Vec<Vec<char>>) -> String {
    let mut code = String::new();
    for stack in cargo {
        code.push(*stack.last().unwrap());
    }
    code
}

fn main() -> Result<(), Box<dyn Error>> {
    println!("{}", solve(read_lines("./data/day5/input.txt")?));
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day5/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day5/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap();
    assert_eq!(&solve(lines), expect);
}

extern crate core;

use std::cmp::max;
use std::collections::{HashMap, HashSet};
use std::error::Error;
use std::time::Instant;

use advent_of_code::read_lines;

type Layout = HashMap<String, Room>;

#[derive(Debug)]
struct Room {
    valve: String,
    flow_rate: u32,
    tunnels: Vec<String>
}

static START_ROOM: &str = "AA";
static TIME_LIMIT: i32 = 26;

fn solve(lines: Vec<String>) -> u32 {
    let rooms = parse_rooms(&lines);
    let shortests_paths = calculate_paths(&rooms);
    let mut valves_with_value: Vec<&str> = Vec::new();
    rooms.iter()
        .filter(|(_, data)| data.flow_rate != 0)
        .for_each(|(name, _)| { valves_with_value.push(name); });

    explore_layout(
        &rooms, &shortests_paths, &valves_with_value, &mut HashSet::new(),
        (START_ROOM, TIME_LIMIT), (START_ROOM, TIME_LIMIT), 0, true
    )
}

fn parse_rooms(lines: &Vec<String>) -> Layout {
    let mut rooms = Layout::new();
    for line in lines {
        let words: Vec<&str> = line.split(' ').collect();
        let raw_flow_rate = words[4].split('=').last().unwrap();
        rooms.insert(words[1].to_string(), Room {
            valve: words[1].to_string(),
            flow_rate: raw_flow_rate[0..raw_flow_rate.len() - 1].parse().unwrap(),
            tunnels: words[9..].iter().map(|x| x.split(',').next().unwrap().to_string()).collect()
        });
    }
    rooms
}

fn calculate_paths(rooms: &Layout) -> HashMap<&str, HashMap<&str, i32>> {
    let mut shortests_paths = HashMap::new();
    for room in rooms.values() {
        let mut distances: HashMap<&str, i32> = HashMap::new();
        let mut queue = Vec::new();
        for room in rooms.values() {
            queue.push(room);
        }
        distances.insert(room.valve.as_str(), 0);
        queue.sort_by(|a, b| distances.get(&b.valve.as_str()).unwrap_or(&i32::MAX).cmp(distances.get(&a.valve.as_str()).unwrap_or(&i32::MAX)));
        while !queue.is_empty() {
            let node = queue.pop().unwrap();
            let node_dist = distances.get(&node.valve.as_str()).unwrap().clone();
            for next_room in &node.tunnels {
                if node_dist + 1 < *distances.get(next_room.as_str()).unwrap_or(&i32::MAX) {
                    distances.insert(next_room.as_str(), node_dist + 1);
                }
            }
            queue.sort_by(|a, b| distances.get(&b.valve.as_str()).unwrap_or(&i32::MAX).cmp(distances.get(&a.valve.as_str()).unwrap_or(&i32::MAX)));
        }
        shortests_paths.insert(room.valve.as_str(), distances);
    }
    shortests_paths
}

fn explore_layout<'a>(
    rooms: &'a Layout, shortests_paths: &HashMap<&'a str, HashMap<&'a str, i32>>,
    valves_with_value: &Vec<&'a str>, open_valves: &mut HashSet<&'a str>,
    agent1: (&'a str, i32), agent2: (&'a str, i32), pressure: u32, can_invert: bool
) -> u32 {
    if open_valves.len() == valves_with_value.len() || agent1.1 <= 0 { // seems redundant but better be safe
        return pressure
    }

    let mut max_pressure = pressure;
    let mut next_pressure = pressure;
    let mut next_time_limit = agent1.1;
    if agent1.0 != START_ROOM && agent1.1 == agent2.1 && can_invert {
        max_pressure = max(max_pressure, explore_layout(
            rooms, shortests_paths, valves_with_value, open_valves,
                agent2, agent1, next_pressure, false
        ));
    }
    if agent1.0 != START_ROOM { // This can only happen on the starting node
        next_time_limit -= 1;
        next_pressure = pressure + rooms[agent1.0].flow_rate * next_time_limit as u32;
        max_pressure = max(max_pressure, next_pressure);
        open_valves.insert(agent1.0);
    }
    let all_distances = shortests_paths.get(agent1.0).unwrap();
    if open_valves.len() + 1 == valves_with_value.len() {
        max_pressure = max(max_pressure, explore_layout(
            rooms, shortests_paths, valves_with_value, open_valves,
            agent2, (agent1.0, 0), next_pressure, true
        ));
    } else {
        for next_room in valves_with_value {
            if !open_valves.contains(next_room) && next_room != &agent2.0 {
                let next_room_distance = all_distances[next_room];
                let (next_agent1, next_agent2) = if next_time_limit - next_room_distance >= agent1.1 {
                    ((*next_room, next_time_limit - next_room_distance), agent2)
                } else {
                    (agent2, (*next_room, next_time_limit - next_room_distance))
                };
                max_pressure = max(max_pressure, explore_layout(
                    rooms, shortests_paths, valves_with_value, open_valves,
                    next_agent1, next_agent2, next_pressure, true
                ));
            }
        }
    }
    open_valves.remove(agent1.0);

    max_pressure
}

fn main() -> Result<(), Box<dyn Error>> {
    let lines = read_lines("./data/day16/input.txt")?;
    let timer = Instant::now();
    let solution =  solve(lines);
    println!("total time [{:.2?}]", timer.elapsed());
    println!("{}", solution);
    Ok(())
}

#[test]
fn test() {

    let lines = read_lines("./data/day16/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day16/part2/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}

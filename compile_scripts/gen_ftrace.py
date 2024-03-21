from pathlib import Path
import re

script_dir = Path(__file__).resolve().parent
proj_dir = script_dir.parent
root_dir = proj_dir.parent


def read_func_map(input_path: Path):
    with input_path.open() as input_file:
        lines = input_file.readlines()

    result = {}
    for line in lines:
        words = line.strip().split()
        key = int(words[0], 16)
        func_name = words[2]
        result[key] = func_name

    return result


def find_func_name(addr: int, base1: int, base2: int, func_map: dict):
    if addr < base1:
        return None
    orig_addr = addr - base1 + base2
    if orig_addr in func_map:
        return func_map[orig_addr]
    else:
        return None


def read_trace(input_path: Path, base1: int, base2: int, func_map: dict):
    with input_path.open() as input_file:
        lines = input_file.readlines()

    curr_addr = 0
    trace = []
    i = 0
    for line in lines:
        i += 1
        x = re.match(r"@@@ \(([\da-z]+)=>([\da-z]+)\)", line)
        if not x:
            continue
        addr = int(x.group(1), 16)
        if curr_addr == addr:
            continue
        curr_addr = addr
        func_name = find_func_name(addr, base1, base2, func_map)
        if func_name:
            trace.append((i, addr, func_name))

    output_dir = input_path.parent
    output_name = input_path.name.split("-")[0] + "-ftrace.txt"
    with (output_dir / output_name).open(mode="w") as output_file:
        for x in trace:
            output_file.write(f"{x[0]}\t{hex(x[1])}\t{x[2]}\n")

    return trace


def main():
    func_map = read_func_map(proj_dir / "System.map")
    read_trace(root_dir / "gem5-new" / "incorrect-grep.txt",
               0xffffff8000000000, 0xffffffff80000000,
               func_map)

    read_trace(root_dir / "gem5-new" / "correct-grep.txt",
               0xffffff8000000000, 0xffffffff80000000,
               func_map)


if __name__ == '__main__':
    main()


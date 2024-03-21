from pathlib import Path

script_dir = Path(__file__).resolve().parent
proj_dir = script_dir.parent

def read_binary(input_path: Path):
    with input_path.open(mode="rb") as input_file:
        while True:
            hex_data = input_file.read(4)[::-1].hex()
            if len(hex_data) == 0:
                break
            print(hex_data)


def main():
    read_binary(proj_dir / "arch/x86/boot/compressed/vmlinux.relocs")


if __name__ == '__main__':
    main()

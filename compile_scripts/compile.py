import click
import subprocess
import shutil
import re
from pathlib import Path
import tomli

script_dir = Path(__file__).resolve().parent
proj_dir = script_dir.parent


def read_config_file(input_path: Path):
    with input_path.open(mode="rb") as input_file:
        data = tomli.load(input_file)

    default = None
    if "default" in data:
        default = data["default"]
    assert default is not None

    result = {}
    for key, val in data.items():
        if key != "default":
            result[key] = {**default}
            result[key].update(val)

    return result


def generate_full_config(input_path: Path, output_path: Path, config: dict):
    with input_path.open() as input_file:
        s = input_file.read()

    for key, val in config.items():
        s, count = re.subn(
            rf"{key}=[\da-z]*",
            f"{key}={val}",
            s
        )
        if count == 0:
            s = s + f"{key}={val}\n"

    output_path.parent.mkdir(exist_ok=True)
    with output_path.open(mode="w") as output_file:
        output_file.write(s)


def compile_one_config(config_name: str, config_dict: dict, compile_num_cores: int = 0):
    template_path = script_dir / "gem5_template.config"
    orig_config_path = script_dir / "generated" / f"{config_name}.config"
    config_path = proj_dir / f".config"

    # Prepare config files
    if config_name not in config_dict:
        print(f"Error: {config_name} not specified!!!")
    generate_full_config(template_path, orig_config_path, config_dict[config_name])
    print(f"@@@ Copy .config from {orig_config_path}")
    shutil.copy(orig_config_path, config_path)
    target_vmlinux_name = f"vmlinux_{config_name}"

    # Make
    make_cmd = "make -j" if compile_num_cores == 0 else f"make -j{compile_num_cores}"
    print("@@@ First", make_cmd)
    p = subprocess.run(make_cmd, shell=True)
    if p.returncode:
        # Sometimes it fails on the first try, so allow to try make again
        print("@@@ First try failed, second", make_cmd)
        p = subprocess.run(make_cmd, shell=True)
        if p.returncode:
            print(f"Compile with config {config_name} failed")
            return

    print(f"@@@ Compile success, mv vmlinux to {target_vmlinux_name}")
    shutil.move(proj_dir / "vmlinux", proj_dir / target_vmlinux_name)
    reloc_dir = proj_dir / "arch/x86/boot/compressed"
    shutil.move(reloc_dir / "vmlinux.relocs", proj_dir / f"{target_vmlinux_name}.relocs")


@click.command()
@click.option(
    "--config-delta",
    is_flag=True,
)
@click.option(
    "--default-delta",
    is_flag=True,
)
@click.option(
    "--delta-list",
    type=click.STRING,
    default=""
)
@click.option(
    "--num-cores",
    type=click.INT,
    default=0
)
def main(config_delta: bool, default_delta: bool, delta_list: str, num_cores: int):
    config_dict = read_config_file(script_dir / "linux_config.toml")

    config_name_list = [
        # "gem5",
        # "gem5_protect",
        # "gem5_protect_module",
        # "gem5_protect_both",
        # "gem5_protect_all",
        # "gem5_protect_kvm",
        "gem5_protect_both_kvm",
        # "gem5_protect_all_kvm",
    ]

    for name in config_name_list:
        compile_one_config(
            name,
            config_dict,
            num_cores
        )


if __name__ == '__main__':
    main()

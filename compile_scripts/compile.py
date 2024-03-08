import click
import subprocess
import shutil
import re
from pathlib import Path

script_dir = Path(__file__).resolve().parent
proj_dir = script_dir.parent


def generate_config(text_delta: int, module_delta: int, input_path: Path, output_path: Path):
    with input_path.open() as input_file:
        s = input_file.read()

    s = re.sub(
        r"CONFIG_GEM5_KASLR_DELTA=\d*",
        f"CONFIG_GEM5_KASLR_DELTA={text_delta}",
        s
    )

    s = re.sub(
        r"CONFIG_GEM5_KASLR_MODULE_DELTA=\d*",
        f"CONFIG_GEM5_KASLR_MODULE_DELTA={module_delta}",
        s
    )

    with output_path.open(mode="w") as output_file:
        output_file.write(s)


def compile_one_config(config_name: str, delta_config, compile_num_cores: int = 0):
    orig_config_path = script_dir / f"{config_name}.config"
    config_path = proj_dir / f".config"
    # config_path = script_dir / f"{config_name}.config"

    # Prepare config files
    if delta_config:
        assert len(delta_config) == 2
        text_delta, module_delta = delta_config
        generate_config(text_delta, module_delta, orig_config_path, config_path)
        print(f"@@@ Generate .config from {orig_config_path} with "
              f"text_delta {text_delta}, module_delta {module_delta}")
        target_vmlinux_name = f"vmlinux_{config_name}_{text_delta}_{module_delta}"
    else:
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
    config_name_list = [
        "gem5",
        # "gem5_protect",
        "gem5_protect_module",
        # "gem5_protect_both"
    ]

    delta_configs = []
    if config_delta:
        delta_configs.extend([
            [6, 16],
            # [1, 7],
            # [25, 7],
            # [8, 7]
        ])

    if default_delta:
        delta_configs.append(None)

    for name in config_name_list:
        for delta_config in delta_configs:
            compile_one_config(
                name,
                delta_config,
                num_cores
            )


if __name__ == '__main__':
    main()

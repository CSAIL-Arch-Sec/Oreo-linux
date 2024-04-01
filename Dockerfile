# syntax=docker/dockerfile:1

FROM python:3.9-bookworm

RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && \
    rm -rf /var/cache/apt/archives/lock && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf automake autotools-dev  \
    curl git wget less nano zsh \
    cmake gdb \
    libmpc-dev libmpfr-dev libgmp-dev  \
    gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev  \
    device-tree-compiler \
    libelf-dev \
    qemu-system-x86 qemu-utils
#    kmod \
#    iasl \
#    sparse \
#    libaio-dev libnfs-dev libseccomp-dev \
#    libxkbcommon-dev libcap-ng-dev libvdeplug-dev libpulse-dev \
#    libncurses-dev libssl-dev


# Install and use oh-my-zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
ENV THEME="agnoster"
RUN sed -i s/^ZSH_THEME=".\+"$/ZSH_THEME=\"$THEME\"/g ~/.zshrc
RUN echo "zsh" >> ~/.bashrc

WORKDIR /root

# Install Poetry
RUN --mount=type=cache,target=/root/.cache pip3 install poetry
# Create virtualenv
ENV VIRTUAL_ENV=/root/.venv
RUN python3 -m virtualenv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
# Install dependencies
COPY pyproject.toml poetry.lock README.md /root/
# Install dependencies
RUN --mount=type=cache,target=/root/.cache poetry install

RUN git config --global --add safe.directory '*'

RUN ln -s /usr/local/bin/python3.9 /usr/bin/python3.9
# TODO: Download a linux image somewhere

#RUN poetry init

ENV TZ="America/New_York"

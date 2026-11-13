FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:3d68548441b79a7db65a238fb212fd2cc50563e9c2e0c0b41faf5f19843c0c2d

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

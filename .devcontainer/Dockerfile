FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:a59408646bb04726c365fcf2031bd057b824bcb0dcfc1d683ffc504757b6f025

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

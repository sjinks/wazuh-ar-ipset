FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:f412a4fb27403940532fd15c5cc482a16eb2cd199e5544f5f748bc913a65e2d8

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:a5eb5a1e9109af88bf82ebb0f71903608a68144851ed1e4b852e31b251ac59c6

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

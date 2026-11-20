FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:504cf3a779c559c4941fbe91cec5bc3b3714bc349f81a50aaeec9eab4b8c2b98

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

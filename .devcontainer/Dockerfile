FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:fe57de20805248a5060fd46c71a79f36266369043f97c88adf949e1bc38308c4

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

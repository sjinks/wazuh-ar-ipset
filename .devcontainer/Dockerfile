FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:06e59c756f0b90728dd87e4b96cb93d923a6c7863d580976363fb492f77ba001

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

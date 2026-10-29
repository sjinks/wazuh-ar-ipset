FROM mcr.microsoft.com/devcontainers/cpp:debian13@sha256:31589121f1ee85a9346cef403b154ba160e2411de360edca25d193507c4510f2

RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y --no-install-recommends libcjson-dev libipset-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

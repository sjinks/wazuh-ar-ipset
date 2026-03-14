# syntax=docker/dockerfile:1
# check=skip=CopyIgnoredFile

FROM --platform=${BUILDPLATFORM} tonistiigi/xx:latest@sha256:c64defb9ed5a91eacb37f96ccc3d4cd72521c4bd18d5442905b95e2226b0e707 AS xx

FROM --platform=${BUILDPLATFORM} alpine:3.23.3@sha256:25109184c71bdad752c8312a8623239686a9a2071e8825f20acb8f2198c3f659 AS build-alpine
ARG TARGETPLATFORM
COPY --from=xx / /
RUN \
    apk add --no-cache clang llvm lld make cmake file pkgconf && \
    xx-apk add --no-cache cjson-dev cjson-static gcc ipset-dev libmnl-static musl-dev

WORKDIR /src
COPY . .

RUN \
    set -x && \
    export ARCHITECTURE=$(xx-info alpine-arch) && \
    export XX_CC_PREFER_LINKER=ld && \
    export SYSROOT=$(xx-info sysroot) && \
    export HOSTSPEC=$(xx-info triple) && \
    xx-clang --setup-target-triple && \
    cmake -S . -B build $(xx-clang --print-cmake-defines) -DCMAKE_BUILD_TYPE=MinSizeRel -DBUILD_STATIC=on && \
    cmake --build build -j 2 && \
    ${HOSTSPEC}-strip build/firewall-ipset && \
    install -D -m 0755 build/firewall-ipset /static/firewall-ipset

FROM --platform=${BUILDPLATFORM} debian:oldoldstable-slim@sha256:45382a2ad0eecdbc81af0fddf502a5e86c71bb05b5d38e2d054a39c13376ab75 AS build-debian
ARG TARGETPLATFORM
COPY --from=xx / /
RUN \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && apt-get install -y --no-install-recommends ca-certificates clang curl llvm lld make file pkgconf && \
    xx-apt-get install -y --no-install-recommends gcc libc6-dev libcjson-dev libipset-dev && \
    apt-get clean && xx-apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN \
    xx-clang --setup-target-triple && \
    curl -fsSL https://github.com/Kitware/CMake/releases/download/v4.2.1/cmake-4.2.1-linux-$(arch).sh > cmake-install.sh && \
    chmod +x cmake-install.sh && \
    ./cmake-install.sh --skip-license --prefix=/usr/local && \
    rm cmake-install.sh

WORKDIR /src
COPY . .

RUN \
    set -x && \
    export ARCHITECTURE=$(xx-info alpine-arch) && \
    export XX_CC_PREFER_LINKER=ld && \
    export SYSROOT=$(xx-info sysroot) && \
    export HOSTSPEC=$(xx-info triple) && \
    xx-clang --setup-target-triple && \
    cmake -S . -B build $(xx-clang --print-cmake-defines) -DCMAKE_BUILD_TYPE=MinSizeRel -DBUILD_STATIC=off && \
    cmake --build build -j 2 && \
    ${HOSTSPEC}-strip build/firewall-ipset && \
    install -D -m 0755 build/firewall-ipset /dynamic/firewall-ipset

FROM scratch
ARG TARGETARCH
COPY --from=build-alpine /static /static
COPY --from=build-debian /dynamic /dynamic

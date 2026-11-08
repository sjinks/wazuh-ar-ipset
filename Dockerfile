# syntax=docker/dockerfile:1@sha256:4edf897a3ffa55b89f906fc8cc78afdb3f1834cc9c7083565e611a8a7d5fe99e
# check=skip=CopyIgnoredFile

FROM --platform=${BUILDPLATFORM} tonistiigi/xx:latest@sha256:40fe21f44bb8cddf1eb1226db190d0a984118f62ee02a91b77d99448d28264a6 AS xx

FROM --platform=${BUILDPLATFORM} alpine:3.24.4@sha256:f59abe656b692d901e184be3445e084786602108599d2a51d229c0f5a48c71bf AS build-alpine
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

FROM --platform=${BUILDPLATFORM} debian:bookworm-slim@sha256:734a72b3d6142a71993b4bc580ca01d8ca687f77a2c422d96a961d3376b179bc AS build-debian
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
    export ARCHITECTURE=$(xx-info debian-arch) && \
    export XX_CC_PREFER_LINKER=ld && \
    export SYSROOT=$(xx-info sysroot) && \
    export HOSTSPEC=$(xx-info triple) && \
    export PKG_CONFIG_LIBDIR="/usr/lib/${HOSTSPEC}/pkgconfig:/usr/share/pkgconfig" && \
    xx-clang --setup-target-triple && \
    cmake -S . -B build $(xx-clang --print-cmake-defines) -DCMAKE_BUILD_TYPE=MinSizeRel -DBUILD_STATIC=off && \
    cmake --build build -j 2 && \
    ${HOSTSPEC}-strip build/firewall-ipset && \
    install -D -m 0755 build/firewall-ipset /dynamic/firewall-ipset

FROM scratch
ARG TARGETARCH
COPY --from=build-alpine /static /static
COPY --from=build-debian /dynamic /dynamic

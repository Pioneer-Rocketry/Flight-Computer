FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
        git \
        curl \
        ca-certificates \
        build-essential \
        libusb-1.0-0-dev \
        software-properties-common \
        kicad python3 && \
    add-apt-repository -y ppa:team-gcc-arm-embedded/ppa && \
    apt-get install -y gcc-arm-none-eabi && \
    apt-get clean && \
    # python -m pip install InteractiveHtmlBom && \
    rm -rf /var/lib/apt/lists/*
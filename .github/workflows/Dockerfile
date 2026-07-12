FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository -y ppa:team-gcc-arm-embedded/ppa && \
    add-apt-repository -y ppa:kicad/kicad-10.0-releases && \
    apt-get update && \
    apt-get install -y \
        git \
        curl \
        ca-certificates \
        build-essential \
        libusb-1.0-0-dev \
        python3 \
        kicad \
        gcc-arm-none-eabi && \
    apt-get clean && \
    # python -m pip install InteractiveHtmlBom && \
    rm -rf /var/lib/apt/lists/*
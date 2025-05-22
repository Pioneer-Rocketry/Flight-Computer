FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository -y ppa:team-gcc-arm-embedded/ppa && \
    apt-get install -y gcc-arm-none-eabi git build-essential \
    libusb-1.0.0-dev cmake stlink-tools cppcheck gdb-multiarch
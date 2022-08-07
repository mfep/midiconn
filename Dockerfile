FROM ubuntu:20.04
WORKDIR /src
RUN export DEBIAN_FRONTEND=noninteractive \
	&& apt-get update \
	&& apt-get install -y \
		git \
		build-essential \
		wget \
		xorg-dev \
		libspdlog-dev \
		libfmt-dev \
		libasound2-dev \
		libfreetype-dev \
	&& apt-get autoremove --purge -y \
	&& apt-get autoclean \
	&& rm -rf /var/cache/apt/*

RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.sh \
	&& chmod +x cmake-3.22.2-linux-x86_64.sh \
	&& cd / \
	&& mkdir cmake \
	&& cd cmake \
	&& /src/cmake-3.22.2-linux-x86_64.sh --skip-license
	
ENV PATH /cmake/bin:$PATH

RUN wget https://www.libsdl.org/release/SDL2-2.0.20.tar.gz \
	&& tar -xf SDL2-2.0.20.tar.gz \
	&& cd SDL2-2.0.20 \
	&& mkdir build \
	&& cd build \
	&& ../configure \
	&& make \
	&& make install
	
RUN rm -rf /src/*

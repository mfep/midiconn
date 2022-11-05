FROM ubuntu:22.04
WORKDIR /src
RUN export DEBIAN_FRONTEND=noninteractive \
	&& apt-get update \
	&& apt-get install -y \
		build-essential \
		git \
		libasound2-dev \
		libfmt-dev \
		libfreetype-dev \
		libsdl2-dev \
		libspdlog-dev \
		wget \
		xorg-dev \
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

RUN git clone --depth 1 https://github.com/thestk/rtmidi.git /src/rtmidi \
	&& cd /src/rtmidi \
	&& git checkout 806e18f575b68c23b26f9398e1b6866b335b5308 \
	&& cmake -S . -B build \
	  -D BUILD_SHARED_LIBS=ON \
	  -D CMAKE_BUILD_TYPE=Release \
	  -D RTMIDI_BUILD_TESTING=OFF \
	&& cmake --build build --target install -j `nproc`
	
RUN rm -rf /src/*


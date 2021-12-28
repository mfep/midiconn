cmake -S ./deps/rtmidi -B ./deps/rtmidi/build -D RTMIDI_BUILD_STATIC_LIBS=ON
cmake --build ./deps/rtmidi/build
cmake -S . -B ./build
cmake --build ./build

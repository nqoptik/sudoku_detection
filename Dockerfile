# Build stage
FROM ubuntu:focal AS build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    libopencv-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/sudoku_detection

COPY src src
COPY CMakeLists.txt .

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

# Production stage
FROM ubuntu:focal AS production

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/sudoku_detection

COPY --from=build /root/sudoku_detection/build build

CMD [ "build/sudoku_detection" ]

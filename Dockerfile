FROM ubuntu:latest AS build-stage

# install build dependencies
RUN apt-get update && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
   git \
   make \
   cmake \
   libstdc++-10-dev \
   g++-10 \
   autoconf \
   automake \
   libtool \
   nasm \
   libssl-dev \
   libssl1.1


# Copy source files
COPY . /torrenttools
WORKDIR /torrenttools

# Generate build files
RUN cmake -S . -B cmake-build-relwithdebinfo \
          -DCMAKE_CXX_COMPILER=g++-10 \
          -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -DTORRENTTOOLS_BUILD_TESTS=OFF \
          -DTORRENTTOOLS_BUILD_DOCS=OFF \
          -DDOTTORRENT_CRYPTO_MULTIBUFFER=ON

# Build
RUN cd cmake-build-relwithdebinfo && make -j$(nproc) torrenttools


FROM ubuntu:latest AS runtime

RUN apt-get update && DEBIAN_FRONTEND="noninteractive" apt-get install -y libssl1.1
COPY --from=build-stage /torrenttools/cmake-build-relwithdebinfo/torrenttools /usr/bin/
RUN chmod +x "/usr/bin/torrenttools"

ENTRYPOINT ["/usr/bin/torrenttools"]
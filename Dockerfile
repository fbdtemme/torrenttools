FROM alpine:latest AS build-stage

RUN apk add --update-cache \
    git \
    make \
    cmake \
    autoconf \
    automake \
    libtool \
    g++ \
    nasm \
    openssl-dev \
    libtbb-dev

# Copy source files
#COPY . /torrenttools
#WORKDIR /torrenttools
ENV VERSION="0.5.0"
RUN wget "https://github.com/fbdtemme/torrenttools/releases/download/v$VERSION/torrenttools-$VERSION.tar.gz"
RUN tar -xzf "torrenttools-$VERSION.tar.gz"
RUN mv torrenttools-$VERSION torrenttools

# Generate build files
RUN cmake -S torrenttools -B cmake-build-relwithdebinfo \
          -DCMAKE_CXX_COMPILER=g++ \
          -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -DTORRENTTOOLS_BUILD_TESTS=OFF \
          -DTORRENTTOOLS_BUILD_DOCS=OFF \
          -DDOTTORRENT_MB_CRYPTO_LIB=isal

# Build
RUN cd cmake-build-relwithdebinfo && make -j$(nproc) torrenttools


FROM alpine:latest AS runtime

RUN apk add --update-cache openssl libtbb
COPY --from=build-stage cmake-build-relwithdebinfo/torrenttools /usr/bin/
RUN chmod +x "/usr/bin/torrenttools"

ENTRYPOINT ["/usr/bin/torrenttools"]
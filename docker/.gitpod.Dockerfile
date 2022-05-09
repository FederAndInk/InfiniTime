FROM gitpod/workspace-full

USER root
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qq \
    && apt-get install -y \
# x86_64 / generic packages
      bash \
      build-essential \
      cmake \
      git \
      make \
      python3 \
      python3-pip \
      tar \
      unzip \
      wget \
# ccls build
      llvm-15-dev \
      clang-15 \
      libclang-15-dev \
      libclang-cpp15-dev \
# aarch64 packages
      libffi-dev \
      libssl-dev \
      python3-dev \
      git \
    && rm -rf /var/cache/apt/* /var/lib/apt/lists/*;

# Git needed for PROJECT_GIT_COMMIT_HASH variable setting

# Needs to be installed as root
RUN pip3 install adafruit-nrfutil
RUN pip3 install -Iv cryptography==3.3
RUN pip3 install cbor

COPY docker/build.sh /opt/

# Lets get each in a separate docker layer for better downloads
# GCC
RUN bash -c "source /opt/build.sh; GetGcc;"
# NrfSdk
RUN bash -c "source /opt/build.sh; GetNrfSdk;"
# McuBoot
RUN bash -c "source /opt/build.sh; GetMcuBoot;"

# build&install latest ccls
RUN git clone --recursive https://github.com/MaskRay/ccls.git \
    && cd ccls \
    && mkdir build \
    && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_INSTALL_PREFIX=/usr/local \
                -DCMAKE_CXX_COMPILER=clang++-15 \
                -DCLANG_LINK_CLANG_DYLIB=on \
                -DUSE_SYSTEM_RAPIDJSON=off \
    && cmake --build . -j \
    && cmake --build . --target install

# Link the default checkout workspace in to the default $SOURCES_DIR
RUN ln -s /workspace/InfiniTime /sources

USER gitpod

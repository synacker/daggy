FROM debian:12

RUN apt-get update -y
RUN apt-get install -y \
                        gcc \
                        git \
                        make \
                        python3-pip \
                        wget \
                        doxygen \
                        graphviz \
                        strace \
                        zip \
                        pkg-config \
                        rpm \
                        build-essential \
                        libfontconfig1-dev \
                        libfreetype6-dev \
                        libx11-dev \
                        libx11-xcb-dev \
                        libxext-dev \
                        libxfixes-dev \
                        libxi-dev \
                        libxrender-dev \
                        libxcb1-dev \
                        libxcb-glx0-dev \
                        libxcb-keysyms1-dev \
                        libxcb-image0-dev \
                        libxcb-shm0-dev \
                        libxcb-icccm4-dev \
                        libxcb-sync-dev \
                        libxcb-xfixes0-dev \
                        libxcb-shape0-dev \
                        libxcb-randr0-dev \
                        libxcb-render-util0-dev \
                        libxcb-xinerama0-dev \
                        libxcb-xkb-dev \
                        libxkbcommon-dev \
                        libxkbcommon-x11-dev \
                        libgl1-mesa-dev \
                        libvulkan-dev \
                        libssl-dev \
                        libxcb-util0-dev \ 
                        xorg-dev \
                        cmake \
                        libxcb-dri3-dev \
                        iputils-ping \
                        libx11-dev \
                        libx11-xcb-dev \
                        libfontenc-dev \ 
                        libice-dev \ 
                        libsm-dev \ 
                        libxau-dev \ 
                        libxaw7-dev 
                     

RUN wget https://github.com/Kitware/CMake/releases/download/v3.27.7/cmake-3.27.7.tar.gz -O /tmp/cmake-3.27.7.tar.gz
RUN tar -xvf /tmp/cmake-3.27.7.tar.gz -C /tmp && cd /tmp/cmake-3.27.7 && ./bootstrap -- && make && make install
RUN rm -rf /tmp/cmake-3.27.7.tar.gz
RUN rm -rf /tmp/cmake-3.27.7

RUN mv /usr/lib/python3.11/EXTERNALLY-MANAGED /usr/lib/python3.11/EXTERNALLY-MANAGED.old
RUN pip3 install conan

RUN setcap cap_net_raw+ep /bin/ping

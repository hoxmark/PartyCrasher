if ! [ -x "$(command -v gcc)" ]; then
        sudo apt-get update && \
        sudo apt-get install build-essential software-properties-common -y && \
        sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
        sudo apt-get update && \
        sudo apt-get install gcc-snapshot -y && \
        sudo apt-get update && \
        sudo apt-get install gcc-6 g++-6 -y && \
        sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6 && \
        sudo apt-get install gcc-4.8 g++-4.8 -y && \
        sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8;
        wget https://raw.githubusercontent.com/hoxmark/PartyCrasher/master/clique-count.c?token=AFLKRyMWWp4cMqRWIFqRZzj3L9idLv8Yks5ZInf0wA%3D%3D
        mv clique-count.c?token=AFLKRyMWWp4cMqRWIFqRZzj3L9idLv8Yks5ZInf0wA%3D%3D clique-count.c
    fi   
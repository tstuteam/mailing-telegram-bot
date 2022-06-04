FROM gcc:latest
LABEL Name=mailingtelegrambot Version=0.1

RUN apt-get update && \
    apt-get install -y g++ make binutils cmake \
    libssl-dev libboost-system-dev libcurl4-openssl-dev zlib1g-dev

COPY . /usr/src/bot

WORKDIR /usr/src/bot/tgbot-cpp
COPY tgbot-cpp/include include
COPY tgbot-cpp/src src
COPY tgbot-cpp/CMakeLists.txt .

RUN cmake . && \
    make -j$(nproc) && \
    make install && \
    rm -rf /usr/src/bot/tgbot-cpp/*

WORKDIR /usr/src/bot
RUN make -j$(nproc)
RUN touch user.db

CMD ["./bot", "user.db"]
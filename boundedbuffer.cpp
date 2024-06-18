#include "boundedbuffer.h"
#include <iostream>

BoundedBuffer::BoundedBuffer(size_t size)
    : size(size), producerIndex(0), consumerIndex(0) {
    buffer.resize(size);
}

void BoundedBuffer::produce(const std::string &item) {
    std::unique_lock<std::mutex> lock(mutex);
    notFull.wait(lock, [this]() { return buffer[producerIndex].empty(); });

    buffer[producerIndex] = item;
    producerIndex = (producerIndex + 1) % size;

    notEmpty.notify_all();
}

std::string BoundedBuffer::consume(const std::string &type) {
    std::unique_lock<std::mutex> lock(mutex);
    notEmpty.wait(lock, [this, &type]() {
        for (const auto &item : buffer) {
            if (item == type) return true;
        }
        return false;
    });

    std::string item;
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (buffer[i] == type) {
            item = buffer[i];
            buffer[i].clear();
            consumerIndex = (i + 1) % size;
            break;
        }
    }

    notFull.notify_all();
    return item;
}

std::vector<std::string> BoundedBuffer::getBuffer() {
    std::unique_lock<std::mutex> lock(mutex);
    return buffer;
}

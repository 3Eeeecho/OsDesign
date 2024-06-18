#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

class BoundedBuffer {
public:
    BoundedBuffer(size_t size = 20);
    void produce(const std::string &item);
    std::string consume(const std::string &type);
    std::vector<std::string> getBuffer();

private:
    std::vector<std::string> buffer;
    size_t size;
    std::mutex mutex;
    std::condition_variable notFull;
    std::condition_variable notEmpty;
    size_t producerIndex;
    size_t consumerIndex;
};

#endif // BOUNDEDBUFFER_H

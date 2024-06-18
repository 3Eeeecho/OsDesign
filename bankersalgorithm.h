#ifndef BANKERSALGORITHM_H
#define BANKERSALGORITHM_H

#include <QVector>

class BankersAlgorithm
{
public:
    BankersAlgorithm(int resourceCount, int processCount);
    bool requestResource(int processIndex, QVector<int> &request);
    void releaseResource(int processIndex);

    QVector<QVector<int>> getAllocation() const;
    QVector<QVector<int>> getMax() const;
    QVector<QVector<int>> getNeed() const;
    QVector<int> getAvailable() const;

    void setAllocation(const QVector<QVector<int>> &allocation);
    void setMax(const QVector<QVector<int>> &max);
    void setNeed(const QVector<QVector<int>> &need);
    void setAvailable(const QVector<int> &available);

    bool checkSafety();
    QVector<int> getSafetySequence();

private:
    int resourceCount;
    int processCount;

    QVector<QVector<int>> allocation;
    QVector<QVector<int>> max;
    QVector<QVector<int>> need;
    QVector<int> available;
};

#endif // BANKERSALGORITHM_H

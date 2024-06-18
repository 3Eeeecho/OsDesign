#include "BankersAlgorithm.h"

BankersAlgorithm::BankersAlgorithm(int resourceCount, int processCount)
    : resourceCount(resourceCount), processCount(processCount),
    allocation(processCount, QVector<int>(resourceCount, 0)),
    max(processCount, QVector<int>(resourceCount, 0)),
    need(processCount, QVector<int>(resourceCount, 0)),
    available(resourceCount,0)
{
}

bool BankersAlgorithm::requestResource(int processIndex, QVector<int> &request)
{
    for (int i = 0; i < resourceCount; ++i) {
        if (request[i] > need[processIndex][i] || request[i] > available[i]) {
            return false;
        }
    }

    for (int i = 0; i < resourceCount; ++i) {
        available[i] -= request[i];
        allocation[processIndex][i] += request[i];
        need[processIndex][i] -= request[i];
    }

    if (!checkSafety()) {
        for (int i = 0; i < resourceCount; ++i) {
            available[i] += request[i];
            allocation[processIndex][i] -= request[i];
            need[processIndex][i] += request[i];
        }
        return false;
    }

    return true;
}

void BankersAlgorithm::releaseResource(int processIndex)
{
    for (int i = 0; i < resourceCount; ++i) {
        available[i] += allocation[processIndex][i];
        allocation[processIndex][i] = 0;
        need[processIndex][i] = max[processIndex][i];
    }
}

QVector<QVector<int>> BankersAlgorithm::getAllocation() const
{
    return allocation;
}

QVector<QVector<int>> BankersAlgorithm::getMax() const
{
    return max;
}

QVector<QVector<int>> BankersAlgorithm::getNeed() const
{
    return need;
}

QVector<int> BankersAlgorithm::getAvailable() const
{
    return available;
}

void BankersAlgorithm::setAllocation(const QVector<QVector<int>> &alloc)
{
    allocation = alloc;
}

void BankersAlgorithm::setMax(const QVector<QVector<int>> &m)
{
    max = m;
}

void BankersAlgorithm::setNeed(const QVector<QVector<int>> &n)
{
    need = n;
}

void BankersAlgorithm::setAvailable(const QVector<int> &avail)
{
    available = avail;
}

bool BankersAlgorithm::checkSafety()
{
    QVector<int> work = available;
    QVector<bool> finish(processCount, false);
    QVector<int> safeSequence;

    while (true) {
        bool found = false;
        for (int i = 0; i < processCount; ++i) {
            if (!finish[i]) {
                bool canProceed = true;
                for (int j = 0; j < resourceCount; ++j) {
                    if (need[i][j] > work[j]) {
                        canProceed = false;
                        break;
                    }
                }
                if (canProceed) {
                    for (int j = 0; j < resourceCount; ++j) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    safeSequence.append(i);
                    found = true;
                }
            }
        }
        if (!found) {
            break;
        }
    }

    for (bool f : finish) {
        if (!f) {
            return false;
        }
    }

    return true;
}

QVector<int> BankersAlgorithm::getSafetySequence()
{
    QVector<int> work = available;
    QVector<bool> finish(processCount, false);
    QVector<int> safeSequence;

    while (true) {
        bool found = false;
        for (int i = 0; i < processCount; ++i) {
            if (!finish[i]) {
                bool canProceed = true;
                for (int j = 0; j < resourceCount; ++j) {
                    if (need[i][j] > work[j]) {
                        canProceed = false;
                        break;
                    }
                }
                if (canProceed) {
                    for (int j = 0; j < resourceCount; ++j) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    safeSequence.append(i);
                    found = true;
                }
            }
        }
        if (!found) {
            break;
        }
    }

    for (bool f : finish) {
        if (!f) {
            return QVector<int>(); // 返回空向量表示不安全
        }
    }

    return safeSequence;
}

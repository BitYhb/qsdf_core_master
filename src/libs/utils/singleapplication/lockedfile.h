#ifndef LOCKEDFILE_H
#define LOCKEDFILE_H

#include <QFile>

namespace Utils {
namespace Internal {

class LockedFile : public QFile
{
public:
    enum LockMode { NoLock = 0, ReadLock, WriteLock };

    LockedFile();
    LockedFile(const QString &strFileName);
    ~LockedFile();

    bool lock(LockMode eMode, bool block = true);
    bool unlock();
    bool isLocked() const;
    LockMode lockMode() const;

private:
    Qt::HANDLE m_pSemaphoreHnd = nullptr;
    Qt::HANDLE m_pMutexHnd = nullptr;
    LockMode m_eLockMode;
};

} // namespace Internal
} // namespace Utils

#endif // LOCKEDFILE_H

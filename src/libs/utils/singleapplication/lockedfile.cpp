#include "lockedfile.h"

#include <QFileInfo>
#include <qt_windows.h>

namespace Utils {
namespace Internal {

#define SEMAPHORE_PREFIX "LockedFile semaphore "
#define MUTEX_PREFIX "LockedFile mutex "
#define SEMAPHORE_MAX 100

static QString errorCodeToString(DWORD errorCode)
{
    QString strResult;
    char *data = 0;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL,
                   errorCode,
                   0,
                   (char *) &data,
                   0,
                   NULL);
    strResult = QString::fromLocal8Bit(data);
    if (data != 0)
        LocalFree(data);

    if (strResult.endsWith(QLatin1Char('\n')))
        strResult.truncate(strResult.length() - 1);

    return strResult;
}

LockedFile::LockedFile()
    : QFile()
{
    m_eLockMode = NoLock;
}

LockedFile::LockedFile(const QString &strFileName)
    : QFile(strFileName)
{
    m_eLockMode = NoLock;
}

LockedFile::~LockedFile()
{
    if (isOpen())
        unlock();
    if (m_pMutexHnd != nullptr) {
        DWORD ret = CloseHandle(m_pMutexHnd);
        if (ret == 0) {
            qWarning("LockedFile::~LockedFile(): CloseHandle (mutex): %s",
                     errorCodeToString(GetLastError()).toLatin1().constData());
        }
        m_pMutexHnd = nullptr;
    }
    if (m_pSemaphoreHnd != nullptr) {
        DWORD ret = CloseHandle(m_pSemaphoreHnd);
        if (ret == 0) {
            qWarning("LockedFile::~LockedFile(): CloseHandle (semaphore): %s",
                     errorCodeToString(GetLastError()).toLatin1().constData());
        }
        m_pSemaphoreHnd = nullptr;
    }
}

bool LockedFile::lock(LockMode eMode, bool block)
{
    if (!isOpen()) {
        qWarning("LockedFile::lock(): file is not opened");
        return false;
    }

    if (eMode == m_eLockMode)
        return true;

    if (m_eLockMode != NoLock)
        unlock();

    if (m_pSemaphoreHnd == nullptr) {
        QFileInfo fileInfo(*this);
        QString strSemaphorName = QString::fromLatin1(SEMAPHORE_PREFIX) + fileInfo.absoluteFilePath().toLower();

        m_pSemaphoreHnd = CreateSemaphoreW(0, SEMAPHORE_MAX, SEMAPHORE_MAX, (TCHAR *) strSemaphorName.utf16());
        if (m_pSemaphoreHnd == nullptr) {
            qWarning("LockedFile::lock(): CreateSemaphore: %s",
                     errorCodeToString(GetLastError()).toLatin1().constData());
            return false;
        }
    }

    bool bGotMutex = false;
    int nDecrement;
    if (eMode == ReadLock) {
        nDecrement = 1;
    } else {
        nDecrement = SEMAPHORE_MAX;
        if (m_pMutexHnd == nullptr) {
            QFileInfo fileInfo(*this);
            QString strMutexName = QString::fromLatin1(MUTEX_PREFIX) + fileInfo.absoluteFilePath().toLower();

            m_pMutexHnd = CreateMutexW(NULL, FALSE, (TCHAR *) strMutexName.utf16());
            if (m_pMutexHnd == nullptr) {
                qWarning("LockedFile::lock(): CreateMutex: %s",
                         errorCodeToString(GetLastError()).toLatin1().constData());
                return false;
            }
        }

        DWORD res = WaitForSingleObject(m_pMutexHnd, block ? INFINITE : 0);
        if (res == WAIT_TIMEOUT)
            return false;
        if (res == WAIT_FAILED) {
            qWarning("LockedFile::lock(): WaitForSingleObject (mutex): %s",
                     errorCodeToString(GetLastError()).toLatin1().constData());
            return false;
        }
        bGotMutex = true;
    }

    for (int i = 0; i < nDecrement; ++i) {
        DWORD res = WaitForSingleObject(m_pSemaphoreHnd, block ? INFINITE : 0);
        if (res == WAIT_TIMEOUT) {
            if (i) {
                // A failed nonblocking rw locking. Undo changes to semaphore.
                if (ReleaseSemaphore(m_pSemaphoreHnd, i, NULL) == 0) {
                    qWarning("LockedFile::unlock(): ReleaseSemaphore: %s",
                             errorCodeToString(GetLastError()).toLatin1().constData());
                }
            }
            if (bGotMutex)
                ReleaseMutex(m_pMutexHnd);
            return false;
        }
        if (res != WAIT_OBJECT_0) {
            if (bGotMutex)
                ReleaseMutex(m_pMutexHnd);
            qWarning("LockedFile::lock(): WaitForSingleObject (semaphore): %s",
                     errorCodeToString(GetLastError()).toLatin1().constData());
            return false;
        }
    }

    m_eLockMode = eMode;
    if (bGotMutex)
        ReleaseMutex(m_pMutexHnd);
    return true;
}

bool LockedFile::unlock()
{
    if (!isOpen()) {
        qWarning("LockedFile::unlock(): file is not opened");
        return false;
    }

    if (!isLocked())
        return true;

    int nIncrement;
    if (m_eLockMode == ReadLock)
        nIncrement = 1;
    else
        nIncrement = SEMAPHORE_MAX;

    DWORD ret = ReleaseSemaphore(m_pSemaphoreHnd, nIncrement, NULL);
    if (ret == 0) {
        qWarning("LockedFile::unlock(): ReleaseSemaphore: %s", errorCodeToString(GetLastError()).toLatin1().constData());
        return false;
    }

    m_eLockMode = LockedFile::NoLock;
    remove();
    return true;
}

bool LockedFile::isLocked() const
{
    return m_eLockMode != NoLock;
}

LockedFile::LockMode LockedFile::lockMode() const
{
    return m_eLockMode;
}

} // namespace Internal
} // namespace Utils

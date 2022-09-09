#include "singleapplication.h"
#include "localpeer.h"

#include <QDir>
#include <QFileOpenEvent>
#include <QSharedMemory>
#include <QWidget>

namespace Utils {

static const int instancesSize = 1024;

SingleApplication::SingleApplication(const QString &strAppId, int &argc, char **argv)
    : QApplication(argc, argv)
{
    m_strAppId = strAppId;

    const QString strAppSessionId = Internal::LocalPeer::appSessionId(strAppId);

    // This shared memory holds a zero-terminated array of active (or crashed) instances
    m_pInstances = new QSharedMemory(strAppSessionId, this);

    // First instance creates the shared memory, later instances attach to it
    const bool bCreated = m_pInstances->create(instancesSize);
    if (!bCreated) {
        if (!m_pInstances->attach()) {
            qWarning() << "Failed to initialize instances shared memory: " << m_pInstances->errorString();
            delete m_pInstances;
            m_pInstances = nullptr;
            return;
        }
    }

    Internal::LockedFile lockfile(instancesLockFileName(strAppSessionId));
    lockfile.open(QIODeviceBase::ReadWrite);
    lockfile.lock(Internal::LockedFile::WriteLock);
    auto ids = static_cast<qint64 *>(m_pInstances->data());
    if (!bCreated) {
        // Find the first instance that it still running
        // The whole list needs to be iterated in order to append to it
        for (; *ids; ++ids) {
            if (m_nFirstPeer == -1 && isRunning(*ids))
                m_nFirstPeer = *ids;
        }
    }
    // Add current pid to list and terminate it
    *ids++ = QCoreApplication::applicationPid();
    *ids = 0;
    m_pPidPeer = new Internal::LocalPeer(this,
                                         strAppId + QLatin1Char('-')
                                             + QString::number(QCoreApplication::applicationPid()));
    connect(m_pPidPeer, &Internal::LocalPeer::messageReceived, this, &SingleApplication::messageReceived);
    m_pPidPeer->isClient();
    lockfile.unlock();
}

SingleApplication::~SingleApplication()
{
    if (!m_pInstances)
        return;
    const qint64 nAppId = QCoreApplication::applicationPid();
    Internal::LockedFile lockfile(instancesLockFileName(Internal::LocalPeer::appSessionId(m_strAppId)));
    lockfile.open(QIODeviceBase::ReadWrite);
    lockfile.lock(Internal::LockedFile::WriteLock);
    // Rewrite array, removing current pid and previously crashed ones
    auto ids = static_cast<qint64 *>(m_pInstances->data());
    qint64 *newIds = ids;
    for (; *ids; ++ids) {
        if (*ids != nAppId && isRunning(*ids))
            *newIds++ = *ids;
    }
    *newIds = 0;
    lockfile.unlock();
}

bool SingleApplication::isRunning(qint64 nPid)
{
    if (nPid == -1) {
        nPid = m_bBlock;
        if (nPid == -1)
            return false;
    }

    Internal::LocalPeer peer(this, m_strAppId + QLatin1Char('-') + QString::number(nPid, 10));
    return peer.isClient();
}

void SingleApplication::setActivationWindow(QWidget *pActiveWidget, bool bActivateOnMessage)
{
    m_pActiveWidget = pActiveWidget;
    if (!m_pPidPeer)
        return;
    if (bActivateOnMessage)
        connect(m_pPidPeer, &Internal::LocalPeer::messageReceived, this, &SingleApplication::activateWindow);
    else
        disconnect(m_pPidPeer, &Internal::LocalPeer::messageReceived, this, &SingleApplication::activateWindow);
}

QWidget *SingleApplication::activationWindow() const
{
    return m_pActiveWidget;
}

bool SingleApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        auto fileOpenEvent = dynamic_cast<QFileOpenEvent *>(event);
        qDebug() << fileOpenEvent->file();
        emit fileOpenRequest(fileOpenEvent->file());
        return true;
    }
    return QApplication::event(event);
}

QString SingleApplication::applicationId() const
{
    return m_strAppId;
}

void SingleApplication::setBlock(bool bValue)
{
    m_bBlock = bValue;
}

bool SingleApplication::sendMessage(const QString &strMessage, int timeout, qint64 pid)
{
    if (pid == -1) {
        pid = m_nFirstPeer;
        if (pid == -1)
            return false;
    }

    Internal::LocalPeer peer(this, m_strAppId + QLatin1Char('-') + QString::number(pid, 10));
    return peer.sendMessage(strMessage, timeout, m_bBlock);
}

void SingleApplication::activateWindow()
{
    if (m_pActiveWidget) {
        m_pActiveWidget->setWindowState(m_pActiveWidget->windowState() & ~Qt::WindowMinimized);
        m_pActiveWidget->raise();
        m_pActiveWidget->activateWindow();
    }
}

QString SingleApplication::instancesLockFileName(const QString &strAppSessionId)
{
    const QChar cSlash(QLatin1Char('/'));
    QString res = QDir::tempPath();
    if (!res.endsWith(cSlash))
        res += cSlash;
    return res + strAppSessionId + QLatin1String("-instances");
}

} // namespace Utils

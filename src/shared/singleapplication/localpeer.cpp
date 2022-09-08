#include "localpeer.h"

#include <QCoreApplication>
#include <QLibrary>
#include <qt_windows.h>

typedef BOOL(WINAPI *PProcessIdToSessionId)(DWORD, DWORD *);
static PProcessIdToSessionId pProcessIdToSessionId = 0;

namespace SharedTools {

static const char ack[] = "ack";

LocalPeer::LocalPeer(QObject *parent, const QString &strAppId)
    : QObject(parent)
    , m_strId(strAppId)
{
    if (m_strId.isEmpty())
        m_strId = QCoreApplication::applicationFilePath();

    m_strSocketName = appSessionId(m_strId);
    m_pLocalServer = new QLocalServer(this);

    QString strLockName = QDir(QDir::tempPath()).absolutePath() + QLatin1Char('/') + m_strSocketName
                          + QLatin1String("-lockfile");
    m_lockFile.setFileName(strLockName);
    m_lockFile.open(QIODevice::ReadWrite);
}

bool LocalPeer::isClient()
{
    if (m_lockFile.isLocked())
        return false;

    if (!m_lockFile.lock(LockedFile::WriteLock, false))
        return true;

    if (!QLocalServer::removeServer(m_strSocketName))
        qWarning("SingleCoreApplication: could not cleanup socket");
    bool res = m_pLocalServer->listen(m_strSocketName);
    if (!res)
        qWarning("SingleCoreApplication: listen on local socket failed, %s", qPrintable(m_pLocalServer->errorString()));
    QObject::connect(m_pLocalServer, &QLocalServer::newConnection, this, &LocalPeer::receiveConnection);
    return false;
}

bool LocalPeer::sendMessage(const QString &strMessage, int nTimeout, bool block)
{
    if (!isClient())
        return false;

    QLocalSocket socket;
    bool connOk = false;
    for (int i = 0; i < 2; ++i) {
        // Try twice, in case the other instance is just starting up
        socket.connectToServer(m_strSocketName);
        connOk = socket.waitForConnected(nTimeout / 2);
        if (connOk || i)
            break;
        int ms = 250;
        Sleep(DWORD(ms));
    }
    if (!connOk)
        return false;

    QByteArray uMsg(strMessage.toUtf8());
    QDataStream ds(&socket);
    ds.writeBytes(uMsg.constData(), uMsg.size());
    bool res = socket.waitForBytesWritten(nTimeout);
    res &= socket.waitForReadyRead(nTimeout); // wait for ack
    res &= (socket.read(qstrlen(ack)) == ack);
    if (block)
        socket.waitForDisconnected(-1);
    return res;
}

QString LocalPeer::appSessionId(const QString &strAppId)
{
    QByteArray idc = strAppId.toUtf8();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    quint16 idNum = qChecksum(idc);
#else
    quint16 idNum = qChecksum(idc.constData(), idc.size());
#endif

    QString res = QLatin1String("singleapplication-") + QString::number(idNum, 16);
    if (!pProcessIdToSessionId) {
        QLibrary lib(QLatin1String("kernel32"));
        pProcessIdToSessionId = (PProcessIdToSessionId) lib.resolve("ProcessIdToSessionId");
    }
    if (pProcessIdToSessionId) {
        DWORD sessionId = 0;
        pProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
        res += QLatin1Char('-') + QString::number(sessionId, 16);
    }
    return res;
}

void LocalPeer::receiveConnection()
{
    QLocalSocket *socket = m_pLocalServer->nextPendingConnection();
    if (!socket)
        return;

    while (socket->bytesAvailable() < static_cast<int>(sizeof(quint32))) {
        if (!socket->isValid())
            return;
        socket->waitForReadyRead(1000);
    }

    QDataStream ds(socket);
    QByteArray uMsg;
    quint32 remaining;
    ds >> remaining;
    uMsg.resize(remaining);
    int got = 0;
    char *uMsgBuf = uMsg.data();
    do {
        got = ds.readRawData(uMsgBuf, remaining);
        remaining -= got;
        uMsgBuf += got;
    } while (remaining && got >= 0 && socket->waitForReadyRead(2000));

    if (got < 0) {
        qWarning() << "LocalPeer: Message reception failed" << socket->errorString();
        delete socket;
        return;
    }

    QString message = QString::fromUtf8(uMsg.constData(), uMsg.size());
    socket->write(ack, qstrlen(ack));
    socket->waitForBytesWritten(1000);
    emit messageReceived(message, socket);
}

} // namespace SharedTools

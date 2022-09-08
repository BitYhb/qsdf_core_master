#ifndef LOCALPEER_H
#define LOCALPEER_H

#include "lockedfile.h"

#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>

namespace Utils {
namespace Internal {

class LocalPeer : public QObject
{
    Q_OBJECT

public:
    explicit LocalPeer(QObject *parent = nullptr, const QString &strAppId = QString());
    bool isClient();
    bool sendMessage(const QString &strMessage, int nTimeout, bool block);
    QString applicationId() const { return m_strId; }
    static QString appSessionId(const QString &strAppId);

signals:
    void messageReceived(const QString &strMessage, QObject *pSocket);

private:
    void receiveConnection();

    QString m_strId;
    QString m_strSocketName;
    QLocalServer *m_pLocalServer;
    LockedFile m_lockFile;
};

} // namespace Internal
} // namespace Utils

#endif // LOCALPEER_H

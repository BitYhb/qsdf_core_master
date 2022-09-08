#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include "../utils_global.h"

#include <QApplication>

QT_FORWARD_DECLARE_CLASS(QSharedMemory)

namespace Utils {

namespace Internal {
class LocalPeer;
}

class MIPS_UTILS_EXPORT SingleBaseApplication : public QApplication
{
    Q_OBJECT

public:
    SingleBaseApplication(const QString &strAppId, int &argc, char **argv);
    ~SingleBaseApplication() override;

    bool isRunning(qint64 nPid = -1);

    void setActivationWindow(QWidget *pActiveWidget, bool bActivateOnMessage = true);
    [[nodiscard]] QWidget *activationWindow() const;
    bool event(QEvent *event) override;

    [[nodiscard]] QString applicationId() const;
    void setBlock(bool bValue);

    bool sendMessage(const QString &strMessage, int timeout = 5000, qint64 pid = -1);
    void activateWindow();

signals:
    void messageReceived(const QString &message, QObject *socket);
    void fileOpenRequest(const QString &strFile);

private:
    static QString instancesLockFileName(const QString &strAppSessionId);

    qint64 m_nFirstPeer = -1;
    QSharedMemory *m_pInstances = nullptr;
    Internal::LocalPeer *m_pPidPeer = nullptr;
    QWidget *m_pActiveWidget = nullptr;
    QString m_strAppId;
    bool m_bBlock = false;
};



} // namespace Utils

#endif // SINGLEAPPLICATION_H

#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>

QT_FORWARD_DECLARE_CLASS(QSharedMemory)

namespace SharedTools {

class LocalPeer;

class SingleApplication : public QApplication
{
    Q_OBJECT

public:
    SingleApplication(const QString &strAppId, int &argc, char **argv);
    virtual ~SingleApplication();

    bool isRunning(qint64 nPid = -1);

    void setActivationWindow(QWidget *pActiveWidget, bool bActivateOnMessage = true);
    QWidget *activationWindow() const;
    bool event(QEvent *event) override;

    QString applicationId() const;
    void setBlock(bool bValue);

    bool sendMessage(const QString &strMessage, int timeout = 5000, qint64 pid = -1);
    void activateWindow();

signals:
    void messageReceived(const QString &message, QObject *socket);
    void fileOpenRequest(const QString &strFile);

private:
    QString instancesLockFileName(const QString &strAppSessionId);

    qint64 m_nFirstPeer = -1;
    QSharedMemory *m_pInstances = nullptr;
    LocalPeer *m_pPidPeer = nullptr;
    QWidget *m_pActiveWidget = nullptr;
    QString m_strAppId;
    bool m_bBlock = false;
};

} // namespace SharedTools

#endif // SINGLEAPPLICATION_H

#pragma once

#include <QQuickWindow>

namespace Core::Internal {

class FramelessQuickWindowPrivate;

class FramelessQuickWindow : public QQuickWindow, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString domainViewURI READ domainViewURI NOTIFY domainViewURIChanged)
public:
    explicit FramelessQuickWindow(QWindow *parent = nullptr);
    ~FramelessQuickWindow() override;

    void classBegin() override;
    void componentComplete() override;

    [[nodiscard]] QString domainViewURI() const;

signals:
    void domainViewURIChanged();

private slots:
    void eventUpdateFullScreenMode();

private:
    std::unique_ptr<FramelessQuickWindowPrivate> d;
};

} // namespace Core::Internal

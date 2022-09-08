#ifndef CORE_ICORE_H
#define CORE_ICORE_H

#include "core_global.h"

#include <utils/mipssettings.h>

#include <QDir>
#include <QObject>

namespace Core {

class IContext;
class ICorePrivate;

namespace Internal {
class MainWindow;
}

/// <summary>
/// ICore 类用于访问组成 MIP Software 系统基本功能的不同部分.
/// 永远不要创建这个类的子类以及实例, 这个类唯一的实例由 Core Plugin 创建, 可以通过 ICore::instance() 获取.
/// </summary>
class MIPS_COREPLUGIN_EXPORT ICore : public QObject
{
    Q_OBJECT
    explicit ICore(QObject *parent = nullptr);

public:
    ~ICore() override;

    void setMainWindow(Internal::MainWindow *mainWindow);

    // 返回指向 ICore 实例的指针, 可用于信号的连接
    static ICore *instance();

    // 返回相对路径 relativePath 的绝对路径, 用于各类资源文件
    static QFileInfo resourcePath(const QString &relativePath = QString());

    // 返回用户目录中相对路径 relativePath 的绝对路径, 用于各类资源文件
    static QFileInfo userResourcePath(const QString &relativePath = QString());

    // 返回应用程序的主配置对象
    // 如果是 QSettings::UserScope, 则将从用户的的设置中读取, 并回退到系统的全局设置
    // 如果是 QSettings::SystemScope, 则只会读取系统当前版本的安装设置
    static Utils::MipsSettings *settings(QSettings::Scope scope = QSettings::UserScope);

    // 添加 context 到已注册的 IContext 列表
    void addContextObject(IContext *context);

signals:
    void aboutContextToChange(const QList<IContext *> &context);
    void contextChanged();

protected:
    QScopedPointer<ICorePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(ICore)
    Q_DISABLE_COPY(ICore)
};

} // namespace Core

#endif // CORE_ICORE_H

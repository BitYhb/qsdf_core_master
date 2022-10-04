#pragma once
#include "core_export.h"

#include <utils/id.h>

#include <QObject>
#include <QScopedPointer>

namespace Core {

/// <summary>
/// Context 类维护一个上下文 Id 的列表, 可用于在 ActionManager 中注册提供 UI 元素
/// </summary>
class QUICK_PLUGIN_CORE_EXPORT Context
{
public:
    Context() = default;
    explicit Context(Utils::Id id) { add(id); }

    void add(Utils::Id id) { m_ids.append(id); }
    void add(const Context &context) { m_ids.append(context.m_ids); }

    bool isEmpty() const { return m_ids.isEmpty(); };

    using const_iterator = QList<Utils::Id>::const_iterator;
    const_iterator begin() const { return m_ids.begin(); }
    const_iterator end() const { return m_ids.end(); }
    Utils::Id at(int index) const { return m_ids.at(index); }
    qsizetype size() const { return m_ids.size(); }

private:
    QList<Utils::Id> m_ids;
};

class IContextPrivate;

/// <summary>
/// IContext 类主要用于将 widget 与 context list 关联起来
/// </summary>
class QUICK_PLUGIN_CORE_EXPORT IContext : public QObject
{
    Q_OBJECT
public:
    explicit IContext(QObject *parent = nullptr);
    ~IContext() override;

    virtual QWidget *widget() const;

    virtual void setWidget(QWidget *widget);

protected:
    QScopedPointer<IContextPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(IContext)
    Q_DISABLE_COPY(IContext)
};

} // namespace Core

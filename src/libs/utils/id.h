#ifndef CORE_ID_H
#define CORE_ID_H

#include "utils_export.h"

#include <QByteArray>
#include <QList>
#include <QMetaType>

namespace Utils {

class QSDF_UTILS_EXPORT Id
{
public:
    Id() = default;
    Id(const char *name);
    Id(const QLatin1String &) = delete;

    QByteArray name() const;

    QString toString() const;

    bool isValid() const { return m_id; }
    bool operator==(const Id &id) const { return m_id == id.m_id; }
    bool operator<(const Id &id) const { return m_id < id.m_id; }

    quintptr uniqueIdentifier() const { return m_id; }

    static Id fromString(const QString &str);
    static Id fromSetting(const QVariant &variant);

    friend size_t qHash(Id id) { return static_cast<size_t>(id.uniqueIdentifier()); }

private:
    explicit Id(quintptr uid)
        : m_id(uid)
    {}

    quintptr m_id = 0;
};

} // namespace Utils

#endif // CORE_ID_H

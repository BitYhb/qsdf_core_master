#include "id.h"

#include <utils/mipsassert.h>

#include <QHash>
#include <QVariant>

namespace Utils {

class StringHolder
{
public:
    StringHolder() = default;

    StringHolder(const char *s, int length)
        : n(length)
        , str(s)
    {
        if (!n)
            length = n = static_cast<int>(strlen(s));
        h = 0;
        while (length--) {
            h = (h << 4) + *s++;
            h ^= (h & 0xf0000000) >> 23;
            h &= 0x0fffffff;
        }
    }

    friend auto qHash(const StringHolder &sh) { return QT_PREPEND_NAMESPACE(qHash)(sh.h, 0); }

    friend bool operator==(const StringHolder &sh1, const StringHolder &sh2)
    {
        return sh1.h == sh2.h && sh1.str && sh2.str && strcmp(sh1.str, sh2.str) == 0;
    }

    int n = 0;
    const char *str = nullptr;
    quintptr h;
};

struct IdCache : public QHash<StringHolder, quintptr>
{
#ifndef MIPS_ALLOW_STATIC_LEAKS
    ~IdCache()
    {
        for (IdCache::iterator it = begin(); it != end(); ++it)
            delete[](const_cast<char *>(it.key().str));
    }
#endif // !MIPS_ALLOW_STATIC_LEAKS
};

static QHash<quintptr, StringHolder> stringFromId;
static IdCache idFromString;

static quintptr theId(const char *str, int n = 0)
{
    static quintptr firstUnusedId = 10 * 1000 * 1000;
    QUICK_ASSERT(str && *str, return 0);
    StringHolder sh(str, n);
    int res = idFromString.value(sh, 0);
    if (res == 0) {
        res = firstUnusedId++;
        sh.str = qstrdup(sh.str);
        idFromString[sh] = res;
        stringFromId[res] = sh;
    }
    return res;
}

static quintptr theId(const QByteArray &ba)
{
    return theId(ba.constData(), ba.size());
}

Id::Id(const char *name)
    : m_id(theId(name, 0))
{}

QByteArray Id::name() const
{
    return stringFromId.value(m_id).str;
}

QString Id::toString() const
{
    return QString::fromUtf8(stringFromId.value(m_id).str);
}

Id Id::fromString(const QString &str)
{
    if (str.isEmpty())
        return {};
    return Id(theId(str.toUtf8()));
}

Id Id::fromSetting(const QVariant &variant)
{
    const QByteArray ba = variant.toString().toUtf8();
    if (ba.isEmpty())
        return Id();
    return Id(theId(ba));
}

} // namespace Utils

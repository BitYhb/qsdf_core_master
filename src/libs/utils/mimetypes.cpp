#include "mimetypes.h"

#include <QHash>
#include <QMimeDatabase>
#include <QMutex>

namespace Utils {

class MimeTypesDatabasePrivate : public QMimeDatabase
{
public:
    static MimeTypesDatabasePrivate *instance();

    enum StartupPhase {
        BeforeInitialize,
        PluginsLoading,
        PluginsInitializing,        // Register up to here.
        PluginsDelayedInitializing, // Use from here on.
        UpAndRunning
    };

    static void setStartupPhase(StartupPhase startupPhase);

    void addData(const QString &id, const QByteArray &data);

    QHash<QString, QByteArray> m_additionalData;
    QMutex mutex;
    int m_startupPhase = BeforeInitialize;
};

Q_GLOBAL_STATIC(MimeTypesDatabasePrivate, staticMimeTypesDatabase)

MimeTypesDatabasePrivate *MimeTypesDatabasePrivate::instance()
{
    return staticMimeTypesDatabase();
}

void MimeTypesDatabasePrivate::addData(const QString &id, const QByteArray &data)
{
    if (m_additionalData.contains(id))
        qWarning("Overwriting data in mime database, id '%s'", qPrintable(id));
    m_additionalData.insert(id, data);
}

void setMimeStartupPhase(StartupPhase startupPhase)
{
    auto d = MimeTypesDatabasePrivate::instance();
    QMutexLocker locker(&d->mutex);
    if (int(startupPhase) != d->m_startupPhase + 1)
        qWarning("Unexpected jump in MimedDatabase lifetime from %d to %d", d->m_startupPhase, int(startupPhase));
    d->m_startupPhase = int(startupPhase);
}

void addMimeTypes(const QString &id, const QByteArray &data)
{
    auto d = MimeTypesDatabasePrivate::instance();
    QMutexLocker locker(&d->mutex);
    if (d->m_startupPhase >= MimeTypesDatabasePrivate::PluginsDelayedInitializing)
        qWarning("Adding items from %s to MimeTypesDatabase after initialization time", qPrintable(id));
    d->addData(id, data);
}

} // namespace Utils

#include "quickapplication.h"

namespace Utils {

/// \cond INTERNAL
namespace Internal {

class QuickApplicationPrivate
{
    friend class Utils::QuickApplication;
public:
    explicit QuickApplicationPrivate(QuickApplication &object)
        : q(&object)
    {}

protected:
    QuickApplication *q;
};

} // namespace Utils::Internal
/// \endcond

/*!
 * \class Utils::QuickApplication
 * \brief The QuickApplication class manages the message distribution and primary setup of the platform.
 */

QuickApplication::QuickApplication(const QString &appId, int &argc, char **argv)
    : SingleApplication(appId, argc, argv)
    , d(new Internal::QuickApplicationPrivate(*this))
{}

QuickApplication::~QuickApplication() = default;

} // namespace Utils

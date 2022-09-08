#include "mipsapplication.h"

namespace Utils {

/// \cond INTERNAL

class MIPSApplicationPrivate
{
public:
    explicit MIPSApplicationPrivate(MIPSApplication &object)
        : q_ptr(&object)
    {}

    MIPSApplication *q_ptr;

private:
    Q_DECLARE_PUBLIC(MIPSApplication)
};

/// \endcond

/*!
 * \class Utils::MIPSApplication
 * \brief The MIPSApplication class manages the message distribution and primary setup of the platform.
 *
 * BBB
 */

MIPSApplication::MIPSApplication(const QString &appId, int &argc, char **argv)
    : SingleApplication(appId, argc, argv)
    , d_ptr(new MIPSApplicationPrivate(*this))
{}

MIPSApplication::~MIPSApplication() = default;

} // namespace Utils

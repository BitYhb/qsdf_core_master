#include "optionsparser.h"
#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <utils/quickapplication.h>

#include <QDir>
#include <QMetaEnum>

using namespace ExtensionSystem;

const char *END_OF_OPTIONS = "--";
const char *OptionsParser::PROFILE_OPTION = "-profile";
const char *OptionsParser::PATIENT_ID_OPTION = "-patient-id";
const char *OptionsParser::CASE_ID_OPTION = "-case-id";

OptionsParser::OptionsParser(QStringList lstArgs,
                             const QMap<QString, bool> &appOptions,
                             QMap<QString, QString> *pFoundOptions,
                             QString *pErrorMessage,
                             PluginManagerPrivate *pmPrivate)
    : m_lstArgs(std::move(lstArgs))
    , m_appOptions(appOptions)
    , m_foundOptions(pFoundOptions)
    , m_errorMessage(pErrorMessage)
    , m_pmPrivate(pmPrivate)
    , m_it(m_lstArgs.constBegin())
    , m_end(m_lstArgs.constEnd())
{
    ++m_it;
    if (m_errorMessage)
        m_errorMessage->clear();
    if (m_foundOptions)
        m_foundOptions->clear();
    m_pmPrivate->arguments.clear();
    m_pmPrivate->argumentsForRestart.clear();
}

bool OptionsParser::parse()
{
    while (!m_hasError) {
        if (!nextToken()) // move forward
            break;
        if (checkForEndOfOption())
            break;
        if (checkForProfilingOption())
            continue;
        if (checkForAppOption())
            continue;
        if (checkForPatientIdOption())
            continue;
        if (checkForCaseIdOption())
            continue;
        if (checkForUnknownOption())
            break;
    }

    return !m_hasError;
}

bool OptionsParser::checkForEndOfOption()
{
    if (m_currentArg != QLatin1String(END_OF_OPTIONS))
        return false;
    while (nextToken())
        m_pmPrivate->arguments << m_currentArg;
    return true;
}

static void setPluginOption(PluginSpec *spec, const QString &option, const QString &parameter)
{
    spec->addArguments({option, parameter});
}

bool OptionsParser::checkForPatientIdOption()
{
    if (m_currentArg != QLatin1String(PATIENT_ID_OPTION))
        return false;
    if (nextToken(RequiredToken)) {
        m_pmPrivate->argumentsForRestart << QLatin1String(PATIENT_ID_OPTION) << m_currentArg;
        m_pmPrivate->arguments << QLatin1String(PATIENT_ID_OPTION) << m_currentArg;

        PluginSpec *spec = m_pmPrivate->pluginByName(QLatin1String("Core"));
        setPluginOption(spec, QLatin1String(PATIENT_ID_OPTION), m_currentArg);
    }
    return true;
}

bool OptionsParser::checkForCaseIdOption()
{
    if (m_currentArg != QLatin1String(CASE_ID_OPTION))
        return false;
    if (nextToken(RequiredToken)) {
        m_pmPrivate->argumentsForRestart << QLatin1String(CASE_ID_OPTION) << m_currentArg;
        m_pmPrivate->arguments << QLatin1String(CASE_ID_OPTION) << m_currentArg;

        PluginSpec *spec = m_pmPrivate->pluginByName(QLatin1String("Core"));
        setPluginOption(spec, QLatin1String(CASE_ID_OPTION), m_currentArg);
    }
    return true;
}

bool OptionsParser::checkForAppOption()
{
    if (!m_appOptions.contains(m_currentArg))
        return false;
    const QString option = m_currentArg;
    QString argument;
    if (m_appOptions.value(option) && nextToken(RequiredToken))
        argument = m_currentArg;
    if (m_foundOptions)
        m_foundOptions->insert(option, argument);
    return true;
}

bool OptionsParser::checkForProfilingOption()
{
    if (m_currentArg != QLatin1String(PROFILE_OPTION))
        return false;
    m_pmPrivate->initProfiling();
    return true;
}

bool OptionsParser::checkForUnknownOption()
{
    if (!m_currentArg.startsWith(QLatin1Char('-')))
        return false;
    if (m_errorMessage)
        *m_errorMessage = QCoreApplication::translate("PluginManager", "Unknown option %1").arg(m_currentArg);
    m_hasError = true;
    return true;
}

bool OptionsParser::nextToken(TokenType type)
{
    if (m_it == m_end) {
        if (type == OptionsParser::RequiredToken) {
            m_hasError = true;
            if (m_errorMessage) {
                *m_errorMessage = QCoreApplication::translate("PluginManager", "Option %1 must have a parameter")
                                      .arg(m_currentArg);
            }
        }
        return false;
    }

    m_currentArg = *m_it;
    ++m_it;
    return true;
}

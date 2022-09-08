#ifndef EXTENSIONSYSTEM_OPTIONSPARSER_H
#define EXTENSIONSYSTEM_OPTIONSPARSER_H

#include <QMap>
#include <QStringList>

namespace ExtensionSystem {

class PluginManagerPrivate;

class OptionsParser
{
public:
    OptionsParser(QStringList lstArgs,
                  const QMap<QString, bool> &appOptions,
                  QMap<QString, QString> *pFoundOptions,
                  QString *pErrorMessage,
                  PluginManagerPrivate *pmPrivate);

    bool parse();

    static const char *PROFILE_OPTION;
    static const char *PATIENT_ID_OPTION;
    static const char *CASE_ID_OPTION;
    static const char *CATEGORY_OPTION;

private:
    // 返回值指示是否已处理该选项
    // 它并不表示成功 (--> m_bHasError)
    bool checkForEndOfOption();
    bool checkForPatientIdOption();
    bool checkForCaseIdOption();
    bool checkForAppOption();
    bool checkForProfilingOption();
    bool checkForCategoryOption();
    bool checkForUnknownOption();

    enum TokenType { OptionalToken, RequiredToken };

    bool nextToken(TokenType type = OptionalToken);

    const QStringList m_lstArgs;
    const QMap<QString, bool> m_appOptions;
    QMap<QString, QString> *m_foundOptions = nullptr;
    QString *m_errorMessage = nullptr;
    PluginManagerPrivate *m_pmPrivate = nullptr;

    QString m_currentArg;
    QStringList::const_iterator m_it;
    QStringList::const_iterator m_end;
    bool m_bDependencyRefreshNeeded = false;
    bool m_hasError = false;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_OPTIONSPARSER_H

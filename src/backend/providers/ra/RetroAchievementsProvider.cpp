#include "RetroAchievementsProvider.h"

#include <QByteArray>
#include <QtGlobal>

namespace providers {
namespace retroAchievements {

RetroAchievementsProvider::RetroAchievementsProvider(QObject* parent)
  : Provider(QLatin1String("ra"), QStringLiteral("RetroAchievements"), parent)
{
  setEnabled(false);
}

Provider& RetroAchievementsProvider::run(SearchContext&)
{
    // TODO: Integrate RetroAchievements metadata processing.
    return *this;
}

QString RetroAchievementsProvider::webApiKey() const
{
    return QString::fromUtf8(qgetenv("PEGASUS_RA_WEB_API_KEY"));
}

QString RetroAchievementsProvider::userUlid() const
{
    const auto& opts = options();
    const auto ulid = opts.find(QStringLiteral("ulid"));

    if (ulid == opts.cend() || ulid->second.empty())
        return {};

    return ulid->second.front().trimmed();
}

} // namespace retroAchievements
} // namespace providers
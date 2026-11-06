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

} // namespace retroAchievements
} // namespace providers
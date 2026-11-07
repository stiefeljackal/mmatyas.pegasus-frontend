#include "RetroAchievementsJsonParser.h"

#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QUrl>

#include <utility>

namespace providers {
namespace retroAchievements {

namespace {

bool fail(QString* errorMessage, const QString& message)
{
    if (errorMessage)
        *errorMessage = message;

    return false;
}

QUrl badgeUrl(const QString& badgeName)
{
    if (badgeName.isEmpty())
        return {};

    QUrl url(QStringLiteral("https://i.retroachievements.org"));
    url.setPath(QStringLiteral("/Badge/") + badgeName + QStringLiteral(".png"));

    return url;
}

bool parseAchievement(const QJsonObject& entry, bool includeProgress, model::Achievement& achievement)
{
    const QJsonValue id = entry.value(QStringLiteral("ID"));
    const QJsonValue title = entry.value(QStringLiteral("Title"));

    if (!id.isDouble() || id.toInt() <= 0 || !title.isString())
        return false;

    achievement.id = QString::number(id.toInt());
    achievement.provider = QStringLiteral("ra");
    achievement.title = title.toString();
    achievement.description = entry.value(QStringLiteral("Description")).toString();
    achievement.points = entry.value(QStringLiteral("Points")).toInt();
    achievement.display_order = entry.value(QStringLiteral("DisplayOrder")).toInt();
    achievement.badge_url = badgeUrl(entry.value(QStringLiteral("BadgeName")).toString());

    if (!includeProgress)
        return true;

    const QJsonValue earned = entry.value(QStringLiteral("DateEarned"));

    if (earned.isUndefined() || earned.isNull() || (earned.isString() && earned.toString().isEmpty())) {
        achievement.unlock_state = model::AchievementUnlockState::Locked;
        return true;
    }

    if (!earned.isString())
        return false;

    QDateTime date = QDateTime::fromString(earned.toString(), QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    if (!date.isValid())
        return false;

    date.setTimeSpec(Qt::UTC);

    achievement.unlock_state = model::AchievementUnlockState::Unlocked;
    achievement.earned_at = date;

    return true;
}

bool parseAchievements(
    const QJsonDocument& document,
    int expectedGameId,
    bool includeProgress,
    std::vector<model::Achievement>& achievements,
    QString* errorMessage)
{
    if (errorMessage)
        errorMessage->clear();

    if (expectedGameId <= 0 || !document.isObject())
        return fail(errorMessage, QStringLiteral("Invalid game response."));

    const QJsonObject root = document.object();

    const QJsonValue gameId = root.value(QStringLiteral("ID"));

    if (!gameId.isDouble() || gameId.toInt() != expectedGameId)
        return fail(errorMessage, QStringLiteral("Unexpected game ID."));

    const QJsonValue value = root.value(QStringLiteral("Achievements"));

    if (!value.isObject())
        return fail(errorMessage, QStringLiteral("Missing achievements object."));

    const QJsonObject entries = value.toObject();

    std::vector<model::Achievement> parsed;
    parsed.reserve(entries.size());

    for (auto it = entries.constBegin(); it != entries.constEnd(); ++it) {
        if (!it.value().isObject())
            return fail(errorMessage, QStringLiteral("Invalid achievement: %1").arg(it.key()));

        model::Achievement achievement;

        if (!parseAchievement(it.value().toObject(), includeProgress, achievement))
            return fail(errorMessage, QStringLiteral("Could not parse achievement: %1").arg(it.key()));

        parsed.push_back(std::move(achievement));
    }

    achievements = std::move(parsed);
    return true;
}

} // anonymous namespace

bool JsonParser::parseGameExtended(
    const QJsonDocument& document,
    int expectedGameId,
    std::vector<model::Achievement>& achievements,
    QString* errorMessage)
{
    return parseAchievements(document, expectedGameId, false, achievements, errorMessage);
}

bool JsonParser::parseGameInfoAndUserProgress(
    const QJsonDocument& document,
    int expectedGameId,
    std::vector<model::Achievement>& achievements,
    QString* errorMessage)
{
    return parseAchievements(document, expectedGameId, true, achievements, errorMessage);
}

} // namespace retroAchievements
} // namespace providers

#pragma once

#include "model/gaming/Achievement.h"

#include <QJsonDocument>
#include <QString>

#include <vector>

namespace providers {
namespace retroAchievements {

class JsonParser final
{
public:
    static bool parseGameExtended(
        const QJsonDocument& document,
        int expectedGameId,
        std::vector<model::Achievement>& achievements,
        QString* errorMessage = nullptr);

    static bool parseGameInfoAndUserProgress(
        const QJsonDocument& document,
        int expectedGameId,
        std::vector<model::Achievement>& achievements,
        QString* errorMessage = nullptr);
};

} // namespace retroAchievements
} // namespace providers

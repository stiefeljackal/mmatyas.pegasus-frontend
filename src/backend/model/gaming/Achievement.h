#pragma once

#include <QDateTime>
#include <QString>
#include <QUrl>

namespace model {

enum class AchievementUnlockState {
    Unknown,
    Locked,
    Unlocked
};

struct Achievement
{

    QString id;
    QString provider;

    QString title;
    QString description;

    int points = 0;
    int display_order = 0;

    QUrl badge_url;
    QUrl locked_badge_url;

    AchievementUnlockState unlock_state = AchievementUnlockState::Unknown;
    
    QDateTime earned_at;
};

} // namespace model
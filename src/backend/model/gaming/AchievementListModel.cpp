#include "AchievementListModel.h"

#include <algorithm>
#include <utility>

namespace model {

AchievementListModel::AchievementListModel(QObject* parent)
    : QAbstractListModel(parent)
{}

int AchievementListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : count();
}

int AchievementListModel::count() const
{
    return static_cast<int>(m_entries.size());
}

QHash<int, QByteArray> AchievementListModel::roleNames() const
{
    return {
        { AchievementIdRole, "achievementId" },
        { ProviderRole, "provider" },
        { TitleRole, "title" },
        { DescriptionRole, "description" },
        { PointsRole, "points" },
        { DisplayOrderRole, "displayOrder" },
        { BadgeUrlRole, "badgeUrl" },
        { LockedBadgeUrlRole, "lockedBadgeUrl" },
        { UnlockStateRole, "unlockState" },
        { EarnedAtRole, "earnedAt" }
    };
}

QVariant AchievementListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= count())
        return {};

    const auto& entry = m_entries.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
        case TitleRole:
            return entry.title;
        case AchievementIdRole:
            return entry.id;
        case ProviderRole:
            return entry.provider;
        case DescriptionRole:
            return entry.description;
        case PointsRole:
            return entry.points;
        case DisplayOrderRole:
            return entry.display_order;
        case BadgeUrlRole:
            return entry.badge_url;
        case LockedBadgeUrlRole:
            return entry.locked_badge_url;
        case UnlockStateRole:
            switch (entry.unlock_state) {
                case AchievementUnlockState::Locked:
                    return QStringLiteral("locked");
                case AchievementUnlockState::Unlocked:
                    return QStringLiteral("unlocked");
                default:
                    return QStringLiteral("unknown");
            }
        case EarnedAtRole:
            return entry.earned_at;
        default:
            return {};
    }
}

void AchievementListModel::replaceProvider(const QString& provider, std::vector<Achievement> achievements)
{
    if (provider.isEmpty())
        return;

    const int oldCount = count();

    std::stable_sort(achievements.begin(), achievements.end(), [](const Achievement& a, const Achievement& b) {
        return a.display_order < b.display_order;
    });

    beginResetModel();

    const auto first = std::remove_if(m_entries.begin(), m_entries.end(), [&provider](const Achievement& entry) {
        return entry.provider == provider;
    });

    m_entries.erase(first, m_entries.end());

    for (auto& entry : achievements) {
        entry.provider = provider;
        m_entries.emplace_back(std::move(entry));
    }

    endResetModel();

    if (oldCount != count())
        emit countChanged();
}

} // namespace model

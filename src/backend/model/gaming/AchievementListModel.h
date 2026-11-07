
#pragma once

#include "Achievement.h"

#include <QAbstractListModel>

#include <vector>

namespace model {

class AchievementListModel final : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        AchievementIdRole = Qt::UserRole + 1,
        ProviderRole,
        TitleRole,
        DescriptionRole,
        PointsRole,
        DisplayOrderRole,
        BadgeUrlRole,
        LockedBadgeUrlRole,
        UnlockStateRole,
        EarnedAtRole
    };

    explicit AchievementListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    void replaceProvider(const QString& provider, std::vector<Achievement> achievements);

signals:
    void countChanged();

private:
    std::vector<Achievement> m_entries;
};

} // namespace model

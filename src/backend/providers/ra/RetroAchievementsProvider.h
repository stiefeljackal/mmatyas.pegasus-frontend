#pragma once

#include "providers/Provider.h"

namespace providers {
namespace retroAchievements {

class RetroAchievementsProvider final : public Provider {
    Q_OBJECT

public:
    explicit RetroAchievementsProvider(QObject* parent = nullptr);
    Provider& run(SearchContext&) final;

private:
    QString webApiKey() const;
    QString userUlid() const;
};

} // namespace retroAchievements
} // namespace providers
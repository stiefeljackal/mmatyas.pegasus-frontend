ENABLED_COMPATS += RetroAchievements

DEFINES *= WITH_COMPAT_RA

QT += network

HEADERS += \
    $$PWD/RetroAchievementsProvider.h \
    $$PWD/RetroAchievementsApiClient.h \

SOURCES += \
    $$PWD/RetroAchievementsProvider.cpp \
    $$PWD/RetroAchievementsApiClient.cpp \

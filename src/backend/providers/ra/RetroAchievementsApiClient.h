
#pragma once

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class QNetworkReply;
class QUrl;

namespace providers {
namespace retroAchievements {

class ApiClient final : public QObject
{
    Q_OBJECT

public:
    enum class RequestType {
        GameExtended,
        UserGameProgress
    };
    Q_ENUM(RequestType)

    explicit ApiClient(QString apiKey, QString userUlid, QObject* parent = nullptr);
    void getGameExtended(int gameId);
    void getGameInfoAndUserProgress(int gameId);

signals:
    void responseReceived(RequestType type, int gameId, const QJsonDocument& response);
    void requestFailed(RequestType type, int gameId, const QString& message);

private:
    bool isValidRequest(RequestType type, int gameId);
    void sendRequest(RequestType type, int gameId, const QUrl& url);
    void handleReply(RequestType type, int gameId, QNetworkReply* reply);

    QString m_apiKey;
    QString m_userUlid;

    QNetworkAccessManager m_networkManager;
};

} // namespace retroAchievements
} // namespace providers

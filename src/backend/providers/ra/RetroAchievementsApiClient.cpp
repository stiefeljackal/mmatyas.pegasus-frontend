
#include "RetroAchievementsApiClient.h"

#include <QJsonParseError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

#include <utility>

namespace providers {
namespace retroAchievements {

namespace {

constexpr int RequestTimeoutMs = 10000;

}

ApiClient::ApiClient(QString apiKey, QString userUlid, QObject* parent)
    : QObject(parent), m_apiKey(std::move(apiKey.trimmed())), m_userUlid(std::move(userUlid.trimmed()))
{}

void ApiClient::getGameExtended(int gameId)
{
    if (!isValidRequest(RequestType::GameExtended, gameId))
        return;

    QUrl url(QStringLiteral("https://retroachievements.org/API/API_GetGameExtended.php"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("y"), m_apiKey);
    query.addQueryItem(QStringLiteral("i"), QString::number(gameId));
    url.setQuery(query);

    sendRequest(RequestType::GameExtended, gameId, url);
}

void ApiClient::getGameInfoAndUserProgress(int gameId)
{
    if (!isValidRequest(RequestType::UserGameProgress, gameId))
        return;

    QUrl url(QStringLiteral("https://retroachievements.org/API/API_GetGameInfoAndUserProgress.php"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("y"), m_apiKey);
    query.addQueryItem(QStringLiteral("u"), m_userUlid);
    query.addQueryItem(QStringLiteral("g"), QString::number(gameId));
    
    url.setQuery(query);

    sendRequest(RequestType::UserGameProgress, gameId, url);
}

bool ApiClient::isValidRequest(RequestType type, int gameId)
{
    if (m_apiKey.isEmpty()) {
        emit requestFailed(type, gameId, QStringLiteral("Missing API key."));
        return false;
    }
    if (m_userUlid.isEmpty()) {
        emit requestFailed(type, gameId, QStringLiteral("Missing ULID."));
        return false;
    }
    if (gameId <= 0 || m_apiKey.isEmpty()) {
        emit requestFailed(type, gameId, QStringLiteral("Invalid game ID."));
        return false;
    }

    return true;
}

void ApiClient::sendRequest(RequestType type, int gameId, const QUrl& url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");

    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    QNetworkReply* reply = m_networkManager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, gameId, reply, type]() {
          handleReply(type, gameId, reply);
      });

    QTimer::singleShot(10000, reply, [reply]() {
        if (reply->isFinished())
            return;

        reply->setProperty("raTimedOut", true);
        reply->abort();
    });
}

void ApiClient::handleReply(RequestType type, int gameId, QNetworkReply* reply)
{
    const bool timedOut = reply->property("raTimedOut").toBool();

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    const auto networkError = reply->error();
    const QByteArray response = reply->readAll();

    reply->deleteLater();

    if (timedOut) {
        emit requestFailed(type, gameId, QStringLiteral("RetroAchievements request timed out."));
        return;
    }
    if (status > 0 && status != 200) {
        emit requestFailed(type, gameId, QStringLiteral("RetroAchievements HTTP error: %1").arg(status));
        return;
    }
    if (networkError != QNetworkReply::NoError) {
        emit requestFailed(type, gameId, QStringLiteral("RetroAchievements network error: %1").arg(static_cast<int>(networkError)));
        return;
    }
    if (status != 200) {
        emit requestFailed(type, gameId, QStringLiteral("No valid HTTP response received."));
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(response, &parseError);

    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        emit requestFailed(type, gameId, QStringLiteral("Invalid RetroAchievements JSON response."));
        return;
    }

    emit responseReceived(type, gameId, document);
}

} // namespace retroAchievements
} // namespace providers

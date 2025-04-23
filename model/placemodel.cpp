// placemodel.cpp
#include "placemodel.h"
#include <QUrl>

PlaceModel::PlaceModel(QObject* parent)
    : QObject(parent)
{
}

void PlaceModel::searchPlaces(const QString& searchText)
{
    if (searchText.trimmed().isEmpty())
        return;

    // Nettoyer les données existantes
    _placeNames.clear();
    _placeCoordinates.clear();

    // Construire l'URL de recherche
    QString encoded = QString::fromUtf8(QUrl::toPercentEncoding(searchText));
    QString url = QString("https://nominatim.openstreetmap.org/search?format=json&q=%1")
                      .arg(encoded);

    QNetworkRequest request((QUrl(url)));
    // IMPORTANT : ajouter un User-Agent, sinon OSM peut refuser la requête
    request.setHeader(QNetworkRequest::UserAgentHeader,
        "Qt Nominatim Example/1.0");

    // Envoyer la requête
    QNetworkReply* reply = _networkManager.get(request);

    // Connecter la réponse au slot
    connect(reply, &QNetworkReply::finished, this,
        [this, reply]() { this->onSearchReply(reply); });
}

QPointF PlaceModel::getPlaceCoordinates(const QString& placeName) const
{
    return _placeCoordinates.value(placeName, QPointF(0, 0));
}

QStringList PlaceModel::getPlaceNames() const
{
    return _placeNames;
}

bool PlaceModel::hasPlace(const QString& placeName) const
{
    return _placeCoordinates.contains(placeName);
}

void PlaceModel::onSearchReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit searchError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        emit searchError(error.errorString());
        return;
    }

    QJsonArray results = doc.array();
    for (const QJsonValue& value : results) {
        QJsonObject obj = value.toObject();
        QString displayName = obj.value("display_name").toString();
        double lat = obj.value("lat").toString().toDouble();
        double lon = obj.value("lon").toString().toDouble();

        // Ajouter à la liste des noms
        _placeNames.append(displayName);
        
        // Sauvegarder les coordonnées associées
        _placeCoordinates[displayName] = QPointF(lon, lat);
    }

    // Émettre le signal pour informer que les données ont été mises à jour
    emit placesUpdated(_placeNames);
}


// placemodel.h
#ifndef PLACEMODEL_H
#define PLACEMODEL_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointF>
#include <QString>
#include <QStringList>

/**
 * @class PlaceModel
 * @brief Modèle de données pour la gestion des lieux.
 *
 * Cette classe gère les données des lieux recherchés, y compris
 * leurs noms et coordonnées géographiques.
 */
class PlaceModel : public QObject {
    Q_OBJECT

private:
    QNetworkAccessManager _networkManager; ///< Pour les requêtes HTTP
    QMap<QString, QPointF> _placeCoordinates; ///< Associe chaque lieu à ses coordonnées
    QStringList _placeNames; ///< Liste des noms de lieux

public:
    /**
     * @brief Constructeur du modèle de lieux.
     * @param parent Objet parent
     */
    explicit PlaceModel(QObject* parent = nullptr);

    /**
     * @brief Recherche des lieux à partir d'un terme de recherche.
     * @param searchText Texte de recherche
     */
    void searchPlaces(const QString& searchText);

    /**
     * @brief Récupère les coordonnées d'un lieu.
     * @param placeName Nom du lieu
     * @return Coordonnées du lieu (longitude, latitude)
     */
    QPointF getPlaceCoordinates(const QString& placeName) const;

    /**
     * @brief Récupère la liste des noms de lieux.
     * @return Liste des noms de lieux
     */
    QStringList getPlaceNames() const;

    /**
     * @brief Vérifie si un lieu existe dans le modèle.
     * @param placeName Nom du lieu
     * @return Vrai si le lieu existe
     */
    bool hasPlace(const QString& placeName) const;

private slots:
    /**
     * @brief Traite la réponse de la recherche de lieux.
     * @param reply Réponse du serveur
     */
    void onSearchReply(QNetworkReply* reply);

signals:
    /**
     * @brief Signal émis lorsque la liste des lieux est mise à jour.
     * @param placeNames Liste des noms de lieux
     */
    void placesUpdated(const QStringList& placeNames);

    /**
     * @brief Signal émis en cas d'erreur lors de la recherche.
     * @param errorMessage Message d'erreur
     */
    void searchError(const QString& errorMessage);
};

#endif // PLACEMODEL_H


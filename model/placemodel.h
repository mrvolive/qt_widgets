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
 * @brief Mod�le de donn�es pour la gestion des lieux.
 *
 * Cette classe g�re les donn�es des lieux recherch�s, y compris
 * leurs noms et coordonn�es g�ographiques.
 */
class PlaceModel : public QObject {
    Q_OBJECT

private:
    QNetworkAccessManager _networkManager; ///< Pour les requ�tes HTTP
    QMap<QString, QPointF> _placeCoordinates; ///< Associe chaque lieu � ses coordonn�es
    QStringList _placeNames; ///< Liste des noms de lieux

public:
    /**
     * @brief Constructeur du mod�le de lieux.
     * @param parent Objet parent
     */
    explicit PlaceModel(QObject* parent = nullptr);

    /**
     * @brief Recherche des lieux � partir d'un terme de recherche.
     * @param searchText Texte de recherche
     */
    void searchPlaces(const QString& searchText);

    /**
     * @brief R�cup�re les coordonn�es d'un lieu.
     * @param placeName Nom du lieu
     * @return Coordonn�es du lieu (longitude, latitude)
     */
    QPointF getPlaceCoordinates(const QString& placeName) const;

    /**
     * @brief R�cup�re la liste des noms de lieux.
     * @return Liste des noms de lieux
     */
    QStringList getPlaceNames() const;

    /**
     * @brief V�rifie si un lieu existe dans le mod�le.
     * @param placeName Nom du lieu
     * @return Vrai si le lieu existe
     */
    bool hasPlace(const QString& placeName) const;

private slots:
    /**
     * @brief Traite la r�ponse de la recherche de lieux.
     * @param reply R�ponse du serveur
     */
    void onSearchReply(QNetworkReply* reply);

signals:
    /**
     * @brief Signal �mis lorsque la liste des lieux est mise � jour.
     * @param placeNames Liste des noms de lieux
     */
    void placesUpdated(const QStringList& placeNames);

    /**
     * @brief Signal �mis en cas d'erreur lors de la recherche.
     * @param errorMessage Message d'erreur
     */
    void searchError(const QString& errorMessage);
};

#endif // PLACEMODEL_H


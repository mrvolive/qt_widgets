// searchcontroller.h
#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include "model/placemodel.h"
#include "model/mapmodel.h"
#include <QObject>

/**
 * @class SearchController
 * @brief Contrôleur pour la recherche de lieux.
 *
 * Cette classe coordonne les interactions entre l'interface utilisateur
 * et le modèle de données pour la recherche de lieux.
 */
class SearchController : public QObject {
    Q_OBJECT

private:
    PlaceModel* _placeModel; ///< Modèle de données pour les lieux
    MapModel* _mapModel; ///< Modèle de données pour la carte

public:
    /**
     * @brief Constructeur du contrôleur de recherche.
     * @param placeModel Modèle de données pour les lieux
     * @param mapModel Modèle de données pour la carte
     * @param parent Objet parent
     */
    explicit SearchController(PlaceModel* placeModel, MapModel* mapModel, QObject* parent = nullptr);

public slots:
    /**
     * @brief Recherche des lieux à partir d'un terme de recherche.
     * @param searchText Texte de recherche
     */
    void search(const QString& searchText);

    /**
     * @brief Sélectionne un lieu dans la liste.
     * @param placeName Nom du lieu sélectionné
     */
    void selectPlace(const QString& placeName);
};

#endif // SEARCHCONTROLLER_H


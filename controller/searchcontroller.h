// searchcontroller.h
#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include "model/placemodel.h"
#include "model/mapmodel.h"
#include <QObject>

/**
 * @class SearchController
 * @brief Contr�leur pour la recherche de lieux.
 *
 * Cette classe coordonne les interactions entre l'interface utilisateur
 * et le mod�le de donn�es pour la recherche de lieux.
 */
class SearchController : public QObject {
    Q_OBJECT

private:
    PlaceModel* _placeModel; ///< Mod�le de donn�es pour les lieux
    MapModel* _mapModel; ///< Mod�le de donn�es pour la carte

public:
    /**
     * @brief Constructeur du contr�leur de recherche.
     * @param placeModel Mod�le de donn�es pour les lieux
     * @param mapModel Mod�le de donn�es pour la carte
     * @param parent Objet parent
     */
    explicit SearchController(PlaceModel* placeModel, MapModel* mapModel, QObject* parent = nullptr);

public slots:
    /**
     * @brief Recherche des lieux � partir d'un terme de recherche.
     * @param searchText Texte de recherche
     */
    void search(const QString& searchText);

    /**
     * @brief S�lectionne un lieu dans la liste.
     * @param placeName Nom du lieu s�lectionn�
     */
    void selectPlace(const QString& placeName);
};

#endif // SEARCHCONTROLLER_H


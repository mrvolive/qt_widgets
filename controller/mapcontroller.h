// mapcontroller.h
#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include "model/mapmodel.h"
#include <QObject>
#include <QPoint>

/**
 * @class MapController
 * @brief Contrôleur pour les interactions avec la carte.
 *
 * Cette classe coordonne les interactions entre l'interface utilisateur
 * et le modèle de données pour la carte.
 */
class MapController : public QObject {
    Q_OBJECT

private:
    MapModel* _mapModel; ///< Modèle de données pour la carte

public:
    /**
     * @brief Constructeur du contrôleur de carte.
     * @param mapModel Modèle de données pour la carte
     * @param parent Objet parent
     */
    explicit MapController(MapModel* mapModel, QObject* parent = nullptr);

public slots:
    /**
     * @brief Définit le centre de la carte.
     * @param lon Longitude du centre
     * @param lat Latitude du centre
     */
    void setCenter(double lon, double lat);

    /**
     * @brief Définit le niveau de zoom de la carte.
     * @param zoom Niveau de zoom
     */
    void setZoom(int zoom);

    /**
     * @brief Gère le déplacement de la carte.
     * @param deltaX Déplacement horizontal en pixels
     * @param deltaY Déplacement vertical en pixels
     * @param zoom Niveau de zoom actuel
     */
    void panMap(int deltaX, int deltaY, int zoom);

    /**
     * @brief Gère le zoom de la carte.
     * @param delta Valeur de la molette (positif pour zoom in, négatif pour zoom out)
     */
    void zoomMap(int delta);
};

#endif // MAPCONTROLLER_H


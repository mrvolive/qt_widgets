// mapcontroller.h
#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include "model/mapmodel.h"
#include <QObject>
#include <QPoint>

/**
 * @class MapController
 * @brief Contr�leur pour les interactions avec la carte.
 *
 * Cette classe coordonne les interactions entre l'interface utilisateur
 * et le mod�le de donn�es pour la carte.
 */
class MapController : public QObject {
    Q_OBJECT

private:
    MapModel* _mapModel; ///< Mod�le de donn�es pour la carte

public:
    /**
     * @brief Constructeur du contr�leur de carte.
     * @param mapModel Mod�le de donn�es pour la carte
     * @param parent Objet parent
     */
    explicit MapController(MapModel* mapModel, QObject* parent = nullptr);

public slots:
    /**
     * @brief D�finit le centre de la carte.
     * @param lon Longitude du centre
     * @param lat Latitude du centre
     */
    void setCenter(double lon, double lat);

    /**
     * @brief D�finit le niveau de zoom de la carte.
     * @param zoom Niveau de zoom
     */
    void setZoom(int zoom);

    /**
     * @brief G�re le d�placement de la carte.
     * @param deltaX D�placement horizontal en pixels
     * @param deltaY D�placement vertical en pixels
     * @param zoom Niveau de zoom actuel
     */
    void panMap(int deltaX, int deltaY, int zoom);

    /**
     * @brief G�re le zoom de la carte.
     * @param delta Valeur de la molette (positif pour zoom in, n�gatif pour zoom out)
     */
    void zoomMap(int delta);
};

#endif // MAPCONTROLLER_H


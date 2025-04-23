// mapmodel.h
#ifndef MAPMODEL_H
#define MAPMODEL_H

#include <QObject>
#include <QPointF>

/**
 * @class MapModel
 * @brief Modèle de données pour la carte.
 *
 * Cette classe gère les données de la carte, comme le centre,
 * le niveau de zoom, etc.
 */
class MapModel : public QObject {
    Q_OBJECT

private:
    int _zoom; ///< Niveau de zoom de la carte
    double _centerLon; ///< Longitude du centre de la carte
    double _centerLat; ///< Latitude du centre de la carte

public:
    /**
     * @brief Constructeur du modèle de carte.
     * @param parent Objet parent
     */
    explicit MapModel(QObject* parent = nullptr);

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
     * @brief Récupère le centre de la carte.
     * @return Coordonnées du centre (longitude, latitude)
     */
    QPointF getCenter() const;

    /**
     * @brief Récupère le niveau de zoom de la carte.
     * @return Niveau de zoom
     */
    int getZoom() const;

signals:
    /**
     * @brief Signal émis lorsque le centre de la carte change.
     * @param lon Longitude du centre
     * @param lat Latitude du centre
     */
    void centerChanged(double lon, double lat);

    /**
     * @brief Signal émis lorsque le niveau de zoom change.
     * @param zoom Niveau de zoom
     */
    void zoomChanged(int zoom);
};

#endif // MAPMODEL_H

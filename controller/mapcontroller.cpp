// mapcontroller.cpp
#include "mapcontroller.h"
#include <cmath>

MapController::MapController(MapModel* mapModel, QObject* parent)
    : QObject(parent)
    , _mapModel(mapModel)
{
}

void MapController::setCenter(double lon, double lat)
{
    _mapModel->setCenter(lon, lat);
}

void MapController::setZoom(int zoom)
{
    _mapModel->setZoom(zoom);
}

void MapController::panMap(int deltaX, int deltaY, int zoom)
{
    // Récupérer le centre actuel
    QPointF center = _mapModel->getCenter();
    double centerLon = center.x();
    double centerLat = center.y();

    // Calcul précis pour la conversion pixels -> degrés
    int n = 1 << zoom;
    double worldSize = 256.0 * n;

    // Pour la longitude, c'est une relation linéaire
    double deltaLon = 360.0 * deltaX / worldSize;

    // Pour la latitude, c'est plus complexe à cause de la projection Mercator
    double latRad = centerLat * M_PI / 180.0;
    double factor = 1.0 / cos(latRad); // Facteur de correction pour la latitude
    double deltaLat = -360.0 * deltaY / (worldSize * factor);

    // Mettre à jour le centre
    _mapModel->setCenter(centerLon + deltaLon, centerLat + deltaLat);
}

void MapController::zoomMap(int delta)
{
    int currentZoom = _mapModel->getZoom();

    // Déterminer le nouveau niveau de zoom
    int newZoom = delta > 0 ? currentZoom + 1 : currentZoom - 1;

    // Mettre à jour le zoom
    _mapModel->setZoom(newZoom);
}

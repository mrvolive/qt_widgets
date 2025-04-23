// mapmodel.cpp
#include "mapmodel.h"

MapModel::MapModel(QObject* parent)
    : QObject(parent)
    , _zoom(10)
    , _centerLon(6.839349) // Belfort, France
    , _centerLat(47.64263)
{
}

void MapModel::setCenter(double lon, double lat)
{
    // Limiter la longitude à [-180, 180] et la latitude à [-85, 85]
    lon = qBound(-180.0, lon, 180.0);
    lat = qBound(-85.0, lat, 85.0);

    if (_centerLon != lon || _centerLat != lat) {
        _centerLon = lon;
        _centerLat = lat;
        emit centerChanged(lon, lat);
    }
}

void MapModel::setZoom(int zoom)
{
    // Limiter le zoom entre 5 et 15
    zoom = qBound(5, zoom, 15);

    if (_zoom != zoom) {
        _zoom = zoom;
        emit zoomChanged(zoom);
    }
}

QPointF MapModel::getCenter() const
{
    return QPointF(_centerLon, _centerLat);
}

int MapModel::getZoom() const
{
    return _zoom;
}

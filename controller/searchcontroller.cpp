// searchcontroller.cpp
#include "searchcontroller.h"

SearchController::SearchController(PlaceModel* placeModel, MapModel* mapModel, QObject* parent)
    : QObject(parent)
    , _placeModel(placeModel)
    , _mapModel(mapModel)
{
}

void SearchController::search(const QString& searchText)
{
    _placeModel->searchPlaces(searchText);
}

void SearchController::selectPlace(const QString& placeName)
{
    if (_placeModel->hasPlace(placeName)) {
        QPointF coords = _placeModel->getPlaceCoordinates(placeName);
        _mapModel->setCenter(coords.x(), coords.y());
    }
}


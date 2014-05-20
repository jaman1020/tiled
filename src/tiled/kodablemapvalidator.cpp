/*
 * kodablemapvalidator.cpp
 * Copyright 2014, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "kodablemapvalidator.h"

#include "documentmanager.h"
#include "map.h"
#include "tilelayer.h"

using namespace Tiled;
using namespace Tiled::Internal;

KodableMapValidator::KodableMapValidator(QObject *parent) :
    QObject(parent)
{
}

static bool isSmallMap(const Map *map)
{
    return map->width() == 15 &&
            map->height() == 8 &&
            map->tileWidth() == 138 &&
            map->tileHeight() == 138;
}

static bool isBigMap(const Map *map)
{
    return map->width() == 30 &&
            map->height() == 14 &&
            map->tileWidth() == 69 &&
            map->tileHeight() == 69;
}

void KodableMapValidator::validateCurrentMap()
{
    const DocumentManager *documentManager = DocumentManager::instance();
    const MapDocument *mapDocument = documentManager->currentDocument();
    if (!mapDocument) {
        setError(QString());
        return;
    }

    const Map *map = mapDocument->map();

    if (! (isSmallMap(map) || isBigMap(map))) {
        setError(tr("Map dimensions or tile size wrong. It is neither a small map nor a big map."));
        return;
    }

    const int tileLayerIndex = map->indexOfLayer(QLatin1String("Tile Layer 1"), Layer::TileLayerType);
    if (tileLayerIndex == -1) {
        setError(tr("No \"Tile Layer 1\" tile layer found."));
        return;
    }

    const TileLayer *tileLayer = static_cast<TileLayer*>(map->layerAt(tileLayerIndex));

    if (!tileLayer->hasProperty(QLatin1String("NumCommands"))) {
        setError(tr("NumCommands property on \"Tile Layer 1\" is missing."));
        return;
    }

    const int commandCount = tileLayer->property(QLatin1String("NumCommands")).toInt();
    if (commandCount < 1 || commandCount > 8) {
        setError(tr("Invalid value for NumCommands property. Should be a value from 1 to 8."));
        return;
    }

    // TODO:
    // * Validate layers
    // * Search for starting tile
    // * Search for finishing tile
    // * Count the number of different colors and verify with looper:
    //   Looper + up to 3 colors or up to 4 colors without looper

    setError(QString());
}

void KodableMapValidator::setError(const QString &error)
{
    if (mError == error)
        return;

    mError = error;

    emit errorChanged(error);
}

/*
 * newmapdialog.cpp
 * Copyright 2009-2010, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
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

#include "newmapdialog.h"
#include "ui_newmapdialog.h"

#include "isometricrenderer.h"
#include "map.h"
#include "mapdocument.h"
#include "objectgroup.h"
#include "orthogonalrenderer.h"
#include "preferences.h"
#include "tilelayer.h"

#include <QSettings>
#include <QMessageBox>

static const char * const MAP_SIZE_KEY = "Map/Size";
static const char * const MAP_LOOPER_KEY = "Map/Looper";
static const char * const MAP_NUMCOMMANDS_KEY = "Map/NumCommands";

using namespace Tiled;
using namespace Tiled::Internal;

NewMapDialog::NewMapDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::NewMapDialog)
{
    mUi->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Restore previously used settings
    Preferences *prefs = Preferences::instance();
    QSettings *s = prefs->settings();
    const QString mapSize = s->value(QLatin1String(MAP_SIZE_KEY),
                                     QLatin1String("Small")).toString();

    if (mapSize == QLatin1String("Small"))
        mUi->radioButtonSmallMap->setChecked(true);

    mUi->looper->setChecked(s->value(QLatin1String(MAP_LOOPER_KEY), true).toBool());
    mUi->commandCount->setValue(s->value(QLatin1String(MAP_NUMCOMMANDS_KEY), 8).toInt());
}

NewMapDialog::~NewMapDialog()
{
    delete mUi;
}

MapDocument *NewMapDialog::createMap()
{
    if (exec() != QDialog::Accepted)
        return 0;

    int mapWidth;
    int mapHeight;
    int tileWidth;
    int tileHeight;

    if (mUi->radioButtonBigMap->isChecked()) {
        mapWidth = 30;
        mapHeight = 24;
        tileWidth = 69;
        tileHeight = 69;
    } else {
        mapWidth = 15;
        mapHeight = 12;
        tileWidth = 138;
        tileHeight = 138;
    }

    const Map::Orientation orientation = Map::Orthogonal;
    const Map::LayerDataFormat layerFormat = Map::Base64Zlib;

    Map *map = new Map(orientation,
                       mapWidth, mapHeight,
                       tileWidth, tileHeight);

    map->setLayerDataFormat(layerFormat);

    TileLayer *tileLayer1 = new TileLayer(tr("Tile Layer 1"), 0, 0,
                                          mapWidth, mapHeight);

    // Set some commonly used properties
    const int commandCount = mUi->commandCount->value();
    const bool enableLooper = mUi->looper->isChecked();
    tileLayer1->setProperty(QLatin1String("NumCommands"), QString::number(commandCount));
    if (enableLooper)
        tileLayer1->setProperty(QLatin1String("Looper"), QLatin1String("true"));

    // Add the default tile layers
    map->addLayer(tileLayer1);

    QLatin1String mapSize("Big");
    if (mUi->radioButtonSmallMap->isChecked())
        mapSize = QLatin1String("Small");

    // Store settings for next time
    QSettings *s = Preferences::instance()->settings();
    s->setValue(QLatin1String(MAP_SIZE_KEY), mapSize);
    s->setValue(QLatin1String(MAP_LOOPER_KEY), enableLooper);
    s->setValue(QLatin1String(MAP_NUMCOMMANDS_KEY), commandCount);

    return new MapDocument(map);
}

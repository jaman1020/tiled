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

    // Make the font of the pixel size label smaller
    QFont font = mUi->pixelSizeLabel->font();
    const qreal size = QFontInfo(font).pointSizeF();
    font.setPointSizeF(size - 1);
    mUi->pixelSizeLabel->setFont(font);

    connect(mUi->radioButtonSmallMap, SIGNAL(toggled(bool)),
            this, SLOT(applyMapType(bool)));
    connect(mUi->radioButtonBigMap, SIGNAL(toggled(bool)),
            this, SLOT(applyMapType(bool)));

    applyMapType(true);
}

NewMapDialog::~NewMapDialog()
{
    delete mUi;
}

MapDocument *NewMapDialog::createMap()
{
    if (exec() != QDialog::Accepted)
        return 0;

    const int mapWidth = mUi->mapWidth->value();
    const int mapHeight = mUi->mapHeight->value();
    const int tileWidth = mUi->tileWidth->value();
    const int tileHeight = mUi->tileHeight->value();

    const Map::Orientation orientation = Map::Orthogonal;
    const Map::LayerDataFormat layerFormat = Map::Base64Zlib;

    Map *map = new Map(orientation,
                       mapWidth, mapHeight,
                       tileWidth, tileHeight);

    map->setLayerDataFormat(layerFormat);

    TileLayer *metaLayer = new TileLayer(tr("Meta"), 0, 0,
                                         mapWidth, mapHeight);

    // Set the Meta layer to invisible by default
    metaLayer->setVisible(false);

    // Set some commonly used properties
    metaLayer->setProperty(QLatin1String("Up"), QString());
    metaLayer->setProperty(QLatin1String("Down"), QString());
    metaLayer->setProperty(QLatin1String("Left"), QString());
    metaLayer->setProperty(QLatin1String("Right"), QString());
    metaLayer->setProperty(QLatin1String("NumCommands"), QLatin1String("5"));

    // Add the default tile layers
    map->addLayer(metaLayer);
    map->addLayer(new TileLayer(tr("Tile Layer 1"), 0, 0,
                                mapWidth, mapHeight));
    map->addLayer(new TileLayer(tr("Coins"), 0, 0,
                                mapWidth, mapHeight));
    map->addLayer(new ObjectGroup(tr("Objects"), 0, 0,
                                  mapWidth, mapHeight));

    QLatin1String mapSize("Big");
    if (mUi->radioButtonSmallMap->isChecked())
        mapSize = QLatin1String("Small");

    // Store settings for next time
    QSettings *s = Preferences::instance()->settings();
    s->setValue(QLatin1String(MAP_SIZE_KEY), mapSize);

    return new MapDocument(map);
}

void NewMapDialog::applyMapType(bool selected)
{
    if (!selected)
        return;

    if (mUi->radioButtonBigMap->isChecked()) {
        mUi->mapWidth->setValue(30);
        mUi->mapHeight->setValue(24);
        mUi->tileWidth->setValue(69);
        mUi->tileHeight->setValue(69);
    } else {
        mUi->mapWidth->setValue(15);
        mUi->mapHeight->setValue(12);
        mUi->tileWidth->setValue(138);
        mUi->tileHeight->setValue(138);
    }

    refreshPixelSize();
}

void NewMapDialog::refreshPixelSize()
{
    const Map map(Map::Orthogonal,
                  mUi->mapWidth->value(),
                  mUi->mapHeight->value(),
                  mUi->tileWidth->value(),
                  mUi->tileHeight->value());

    QSize size = OrthogonalRenderer(&map).mapSize();

    mUi->pixelSizeLabel->setText(tr("%1 x %2 pixels")
                                 .arg(size.width())
                                 .arg(size.height()));
}

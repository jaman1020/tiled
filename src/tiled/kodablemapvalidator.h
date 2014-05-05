/*
 * kodablemapvalidator.h
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

#ifndef KODABLEMAPVALIDATOR_H
#define KODABLEMAPVALIDATOR_H

#include <QObject>

/**
 * A class that determines whether the current map is a valid Kodable level,
 * and emits warnings when not.
 */
class KodableMapValidator : public QObject
{
    Q_OBJECT

public:
    explicit KodableMapValidator(QObject *parent = 0);

signals:
    void errorChanged(const QString &error);

public slots:
    void validateCurrentMap();

private:
    void setError(const QString &error);

    QString mError;
};

#endif // KODABLEMAPVALIDATOR_H

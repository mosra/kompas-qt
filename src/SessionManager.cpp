/*
    Copyright © 2007, 2008, 2009, 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "SessionManager.h"

#include "MainWindow.h"
#include "PluginManager.h"
#include "PluginManagerStore.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

SessionManager::SessionManager(const QString& filename): conf(filename.toStdString()), loaded(false) {
    /* Default session, create it if doesn't exist */
    defaultSession = conf.group("default");
    if(!defaultSession) defaultSession = conf.addGroup("default");

    /* Named sessions, extract their names */
    sessions = conf.groups("session");
    for(vector<ConfigurationGroup*>::const_iterator it = sessions.begin(); it != sessions.end(); ++it)
        _names << QString::fromStdString((*it)->value<string>("name"));

    /* Current session ID */
    _current = conf.value<unsigned int>("current");
}

SessionManager::~SessionManager() {
    /* Save current session ID */
    conf.setValue<unsigned int>("current", _current);
}

void SessionManager::load(unsigned int id) {
    if(id > sessions.size()) return;

    /* Save previous session, if it is not saved */
    if(id != _current) save();

    /* Session configuration group */
    ConfigurationGroup* g;
    if(id == 0) g = defaultSession;
    else g = sessions[id-1];

    _current = id;
    loaded = true;
    emit currentChanged(id);

    /* Load map view, if it is not the same as current */
    AbstractMapView* mapView = MainWindow::instance()->mapView();
    if(!mapView || mapView->plugin() != g->value<string>("mapView")) {
        mapView = MainWindow::instance()->pluginManagerStore()->mapViews()->manager()->instance(g->value<string>("mapView"));

        /* If the map view doesn't exist, cleanup raster model and exit */
        if(!mapView) {
            MainWindow::instance()->setRasterModel(0);
            return;
        }

        /* Set the configured map view */
        MainWindow::instance()->setMapView(mapView);
    }

    /* Load raster model (even if it is the same as current, we need clean
       state) */
    AbstractRasterModel* rasterModel = MainWindow::instance()->pluginManagerStore()->rasterModels()->manager()->instance(g->value<string>("rasterModel"));

    /* If no raster model is defined, set it to zero and return */
    if(!rasterModel) {
        MainWindow::instance()->setRasterModel(0);
        return;
    }

    /* Online maps, loaded packages */
    rasterModel->setOnline(g->value<bool>("online"));
    vector<string> packages = g->values<string>("package");
    for(vector<string>::const_iterator it = packages.begin(); it != packages.end(); ++it)
        rasterModel->addPackage(*it);

    /* Set the already filled model */
    MainWindow::instance()->setRasterModel(rasterModel);

    /* Current coordinates, zoom */
    mapView->zoomTo(g->value<Zoom>("zoom"));
    mapView->setCoords(g->value<LatLonCoords>("coordinates"));

    /* Map layer, overlays */
    mapView->setLayer(QString::fromStdString(g->value<string>("layer")));
    vector<string> overlays = g->values<string>("overlay");
    for(vector<string>::const_iterator it = overlays.begin(); it != overlays.end(); ++it)
        mapView->addOverlay(QString::fromStdString(*it));
}

void SessionManager::save(unsigned int id) {
    if(id > sessions.size()) return;

    /* Session configuration group */
    ConfigurationGroup* g;
    if(id == 0) g = defaultSession;
    else g = sessions[id-1];

    /* Map view and raster model plugin name */
    AbstractMapView* mapView = MainWindow::instance()->mapView();
    g->setValue<string>("mapView", mapView ? mapView->plugin() : "");

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    g->setValue<string>("rasterModel", rasterModel() ? rasterModel()->plugin() : "");

    /* Online maps, loaded packages */
    g->setValue<bool>("online", rasterModel() ? rasterModel()->online() : false);
    g->removeAllValues("package");
    int packageCount = rasterModel() ? rasterModel()->packageCount() : 0;
    for(int i = 0; i != packageCount; ++i)
        g->addValue<string>("package", rasterModel()->packageAttribute(i, AbstractRasterModel::Filename));

    rasterModel.unlock();

    /* Current coordinates, zoom */
    g->setValue<LatLonCoords>("coordinates", mapView ? mapView->coords() : LatLonCoords());
    g->setValue<Zoom>("zoom", mapView ? mapView->zoom() : 0);

    /* Map layer, overlays */
    g->setValue<string>("layer", mapView ? mapView->layer().toStdString() : "");
    g->removeAllValues("overlay");
    QStringList overlays(mapView ? mapView->overlays() : QStringList());
    foreach(const QString& overlay, overlays)
        g->addValue<string>("overlay", overlay.toStdString());

    _current = id;
    emit currentChanged(id);
}

unsigned int SessionManager::newSession(const QString& name) {
    ConfigurationGroup* g = conf.addGroup("session");
    g->setValue<string>("name", name.toStdString());

    sessions.push_back(g);
    _names << name;

    emit namesChanged();

    return sessions.size();
}

void SessionManager::renameSession(unsigned int id, const QString& name) {
    if(id == 0 || id > sessions.size()) return;

    sessions[id-1]->setValue<string>("name", name.toStdString());
    _names[id-1] = name;

    emit namesChanged();
}

void SessionManager::deleteSession(unsigned int id) {
    if(id == 0 || id > sessions.size()) return;

    conf.removeGroup(sessions.at(id-1));

    sessions.erase(sessions.begin()+id-1);
    _names.removeAt(id-1);

    emit namesChanged();
}

}}

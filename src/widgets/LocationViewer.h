/****************************************************************************/
//    copyright 2013 - 2016 Chris Rizzitello <sithlord48@gmail.com>         //
//                                                                          //
//    This file is part of FF7tk                                            //
//                                                                          //
//    FF7tk is free software: you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License, or     //
//    (at your option) any later version.                                   //
//                                                                          //
//    FF7tk is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          //
//    GNU General Public License for more details.                          //
/****************************************************************************/
#ifndef LOCATIONVIEWER_H
#define LOCATIONVIEWER_H

#include <QtWidgets>
// Include FF7tk Items.
#include "../data/FF7Location.h"
#include "../data/FF7FieldItemList.h"

/** \class LocationViewer
 *  \brief Set Saves Location or View Field Locations and Toggle the items picked up from them.
 *  \todo Support Progression within this widget
 **/
class LocationViewer : public QWidget
{
    Q_OBJECT
public:
    enum filterMode {NAME, ITEM};
    explicit LocationViewer(qreal Scale = 1, QWidget *parent = nullptr);
    void setFilterString(QString filter = "", LocationViewer::filterMode mode = LocationViewer::NAME);
signals:
    void locationChanged(QString);
    void xChanged(int x);
    void yChanged(int y);
    void tChanged(int t);
    void dChanged(int d);
    void mapIdChanged(int mapId);
    void locIdChanged(int locId);
    void locationStringChanged(QString);
    void fieldItemConnectRequest(quint8 index, QList<quint16> offset, QList<quint8>bit);
    void fieldItemCheck(int index);
    void fieldItemChanged(int index, bool checked);

public slots:
    void setSelected(QString);
    void setLocationChangesSaved(bool saveChanges);
    bool locationChangesSaved(void);
    void setRegion(QString region);
    void setTranslationBaseFile(QString);
    void setX(int x);
    void setY(int y);
    void setT(int t);
    void setD(int d);
    void setMapId(int mapId);
    void setLocationId(int locId);
    void setLocationString(QString);
    void setHorizontalHeaderStyle(QString styleSheet);
    void setFieldItemChecked(int row, bool checked);
    void init_fieldItems(void);
    void setAdvancedMode(bool advancedMode);
    bool advancedMode(void);

private slots:
    void itemChanged(int currentRow, int currentColumn, int prevRow, int prevColumn);
    void sbMapIdChanged(int mapId);
    void sbLocIdChanged(int locId);
    void sbXChanged(int x);
    void sbYChanged(int y);
    void sbTChanged(int t);
    void sbDChanged(int d);
    void lineLocationNameChanged(QString);
    void setLocation(int mapId, int locId);
    void filterLocations(QString filter);
    void actionNameSearchToggled(bool checked);
    void actionItemSearchToggled(bool checked);
    void actionRegExpSearchToggled(bool checked);
    void actionCaseSensitiveToggled(bool checked);
    void fieldItemListItemChanged(QModelIndex index);
    void chkAutoUpdateChanged(bool checked);
protected:
    void resizeEvent(QResizeEvent *ev);
private:
    void init_display(void);
    void init_connections(void);
    void init_disconnect(void);
    QString translate(QString text);
    void searchItem(QRegExp exp);
    void searchName(QRegExp exp);
    QTableWidget *locationTable;
    QLineEdit *lineTableFilter;
    QAction *actionNameSearch;
    QAction *actionItemSearch;
    QAction *actionRegExpSearch;
    QAction *actionCaseSensitive;
    QToolButton *btnSearchOptions;
    FF7Location *Locations;
    FF7FieldItemList *fieldItems;
    QLabel *lblLocationPreview;
    QWidget *CoordsWidget;
    QLineEdit *lineLocationName;
    QSpinBox *sbMapID;
    QSpinBox *sbLocID;
    QSpinBox *sbX;
    QSpinBox *sbY;
    QSpinBox *sbT;
    QSpinBox *sbD;
    QString region;
    QString transBasePath;
    QListWidget *fieldItemList;
    QGroupBox *groupFieldItems;
    QCheckBox *chkAutoUpdate;
    quint8 searchMode;
    bool regExpSearch;
    bool caseSensitive;
    bool autoUpdate;
    bool _advancedMode;
    qreal scale;
};

#endif // LOCATIONVIEWER_H

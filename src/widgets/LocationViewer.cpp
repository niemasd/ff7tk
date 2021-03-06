/****************************************************************************/
//    copyright 2013 -2016  Chris Rizzitello <sithlord48@gmail.com>         //
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
#include "LocationViewer.h"

LocationViewer::LocationViewer(qreal Scale, QWidget *parent) :  QWidget(parent)
{
    scale = Scale;
    region = "";
    transBasePath = "";
    autoUpdate = false;
    regExpSearch = false;
    caseSensitive = false;
    _advancedMode = false;
    Locations = new FF7Location();
    fieldItems = new FF7FieldItemList();
    init_display();
    init_connections();
    actionNameSearch->setChecked(true);
}

void LocationViewer::setLocationChangesSaved(bool saveChanges)
{
    chkAutoUpdate->setChecked(saveChanges);
}

bool LocationViewer::locationChangesSaved(void)
{
    return autoUpdate;
}

void LocationViewer::resizeEvent(QResizeEvent *ev)
{
    if (ev->type() == QResizeEvent::Resize) {
        QPixmap pix(QStringLiteral(":/locations/%1_%2").arg(QString::number(sbMapID->value()), QString::number(sbLocID->value())));
        if (pix.isNull()) {
            return;
        } else {
            lblLocationPreview->setPixmap(pix.scaled(lblLocationPreview->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void LocationViewer::init_display(void)
{

    lblLocationPreview = new QLabel;
    lblLocationPreview->setMinimumSize(int(320 * scale), int(240 * scale));
    lblLocationPreview->setBaseSize(int(640 * scale), int(480 * scale));
    lblLocationPreview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    locationTable = new QTableWidget;
    locationTable->setColumnCount(3);
    locationTable->setRowCount(Locations->size());
    locationTable->verticalHeader()->setHidden(true);
    locationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    locationTable->setSelectionMode(QAbstractItemView::SingleSelection);
    locationTable->setSortingEnabled(true);

    QTableWidgetItem *newItem = new QTableWidgetItem(tr("Filename"), 0);
    locationTable->setHorizontalHeaderItem(0, newItem);
    locationTable->setColumnWidth(0, fontMetrics().width(QChar('W')) * 6);
    newItem = new QTableWidgetItem(tr("Location Name"), 0);
    locationTable->setHorizontalHeaderItem(1, newItem);
    locationTable->setColumnWidth(1, fontMetrics().width(QChar('W')) * 15);
    newItem = new QTableWidgetItem(tr("LocID"), 0);
    locationTable->setColumnWidth(2, fontMetrics().width(QChar('W')) * 4);
    locationTable->setHorizontalHeaderItem(2, newItem);

    for (int i = 0; i < locationTable->rowCount(); i++) {
        //set the tooltip to the needed file
        QString tooltip(QStringLiteral("<html><head/><body><p><img src=\":/locations/%1_%2\" width=\"%3\" height\"%4\" /></p></body></html>")
                        .arg(Locations->mapID(i), Locations->locationID(i), QString::number(320 * scale), QString::number(480 * scale)));

        newItem = new QTableWidgetItem(Locations->fileName(i), 0);
        newItem->setFlags(newItem->flags() &= ~Qt::ItemIsEditable);
        newItem->setToolTip(tooltip);
        newItem->setTextAlignment(Qt::AlignLeft);
        locationTable->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(Locations->locationString(i), 0);
        newItem->setFlags(newItem->flags() &= ~Qt::ItemIsEditable);
        newItem->setTextAlignment(Qt::AlignLeft);
        newItem->setToolTip(tooltip);
        locationTable->setItem(i, 1, newItem);

        //To assure proper numerical sorting of location IDs they should all contain the same number of characters.
        newItem = new QTableWidgetItem(QStringLiteral("%1").arg(Locations->locationID(i).toInt(), 3, 10, QChar('0')).toUpper());
        newItem->setFlags(newItem->flags() &= ~Qt::ItemIsEditable);
        newItem->setTextAlignment(Qt::AlignHCenter);
        locationTable->setItem(i, 2, newItem);
        locationTable->setRowHeight(i, (fontMetrics().height() + 2));
    }
    locationTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    locationTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    locationTable->adjustSize();
    locationTable->setFixedWidth(locationTable->columnWidth(0) + locationTable->columnWidth(1) + locationTable->columnWidth(2) + locationTable->verticalScrollBar()->widthMM() + fontMetrics().width(QChar('W')));
    locationTable->setCurrentCell(-1, -1);
    locationTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    btnSearchOptions = new QToolButton;
    btnSearchOptions->setLayoutDirection(Qt::RightToLeft);
    btnSearchOptions->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear"), QPixmap(":/common/edit-clear")));
    btnSearchOptions->setPopupMode(QToolButton::MenuButtonPopup);

    actionNameSearch = new QAction(tr("Filter Mode: Name / Location String"), btnSearchOptions);
    actionNameSearch->setCheckable(true);

    actionItemSearch = new QAction(tr("Filter Mode: Items Found at Location"), btnSearchOptions);
    actionItemSearch->setCheckable(true);

    actionRegExpSearch = new QAction(tr("Process Regular Expressions"), btnSearchOptions);
    actionRegExpSearch->setCheckable(true);

    actionCaseSensitive = new QAction(tr("Case Sensitive"), btnSearchOptions);
    actionCaseSensitive->setCheckable(true);

    QMenu *newMenu = new QMenu;
    newMenu->addAction(actionNameSearch);
    newMenu->addAction(actionItemSearch);
    newMenu->addSeparator();
    newMenu->addAction(actionRegExpSearch);
    newMenu->addAction(actionCaseSensitive);

    btnSearchOptions->setMenu(newMenu);

    lineTableFilter = new QLineEdit;

    lineLocationName = new QLineEdit;
    lineLocationName->setPlaceholderText(tr("Location Name"));

    sbMapID = new QSpinBox;
    //sbMapID->setMaximum(3);
    //setMax when varified
    sbMapID->setWrapping(true);
    sbMapID->setPrefix(tr("MapID: "));
    sbMapID->setAlignment(Qt::AlignCenter);

    sbLocID = new QSpinBox;
    sbLocID->setMaximum(786);
    sbLocID->setWrapping(true);
    sbLocID->setPrefix(tr("LocID: "));
    sbLocID->setAlignment(Qt::AlignCenter);

    sbX = new QSpinBox;
    sbX->setPrefix(tr("X: "));
    sbX->setMinimum(-32767);
    sbX->setMaximum(32767);
    sbX->setWrapping(true);
    sbX->setAlignment(Qt::AlignCenter);

    sbY = new QSpinBox;
    sbY->setPrefix(tr("Y: "));
    sbY->setMinimum(-32767);
    sbY->setMaximum(32767);
    sbY->setWrapping(true);
    sbY->setAlignment(Qt::AlignCenter);

    sbT = new QSpinBox;
    sbT->setPrefix(tr("T: "));
    sbT->setMaximum(65565);
    sbT->setWrapping(true);
    sbT->setAlignment(Qt::AlignCenter);

    sbD = new QSpinBox;
    sbD->setMaximum(255);
    sbD->setPrefix(tr("D: "));
    sbD->setWrapping(true);
    sbD->setAlignment(Qt::AlignCenter);

    chkAutoUpdate = new QCheckBox;
    chkAutoUpdate->setText(tr("Save &Location Changes"));
    chkAutoUpdate->setFixedWidth(locationTable->width());
    //connect now and forget it
    connect(chkAutoUpdate, SIGNAL(clicked(bool)), this, SLOT(chkAutoUpdateChanged(bool)));

    fieldItemList = new QListWidget();
    fieldItemList->setFixedHeight(0);
    fieldItemList->setUniformItemSizes(true);
    fieldItemList->setSelectionMode(QAbstractItemView::NoSelection);

    QVBoxLayout *fitemLayout = new QVBoxLayout();
    fitemLayout->setContentsMargins(0, 0, 0, 0);
    fitemLayout->setSpacing(0);
    fitemLayout->addWidget(fieldItemList);
    groupFieldItems = new QGroupBox(tr("Field Items"));
    groupFieldItems->setLayout(fitemLayout);
    groupFieldItems->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QHBoxLayout *nameIDs = new QHBoxLayout;
    nameIDs->addWidget(lineLocationName);
    nameIDs->addWidget(sbMapID);
    nameIDs->addWidget(sbLocID);

    QHBoxLayout *XYTD = new QHBoxLayout;
    XYTD->addWidget(sbX);
    XYTD->addWidget(sbY);
    XYTD->addWidget(sbT);
    XYTD->addWidget(sbD);

    QVBoxLayout *CoordsLayout = new QVBoxLayout;
    CoordsLayout->setContentsMargins(3, 3, 3, 3);
    CoordsLayout->addLayout(nameIDs);
    CoordsLayout->addLayout(XYTD);

    QHBoxLayout *PreviewLayout = new QHBoxLayout;
    PreviewLayout->setAlignment(Qt::AlignCenter);
    PreviewLayout->addWidget(lblLocationPreview);

    QHBoxLayout *FilterLayout = new QHBoxLayout;
    FilterLayout->setContentsMargins(0, 0, 0, 0);
    FilterLayout->addWidget(lineTableFilter, 8);
    FilterLayout->addWidget(btnSearchOptions, 2);

    QVBoxLayout *LeftSideLayout = new QVBoxLayout;
    LeftSideLayout->setSpacing(0);
    LeftSideLayout->addWidget(chkAutoUpdate);
    LeftSideLayout->addWidget(locationTable);
    LeftSideLayout->addLayout(FilterLayout);

    QVBoxLayout *RightSideLayout = new QVBoxLayout;
    RightSideLayout->addLayout(CoordsLayout);
    RightSideLayout->addLayout(PreviewLayout);
    RightSideLayout->addWidget(groupFieldItems);
    setAdvancedMode(advancedMode());

    QHBoxLayout *FinalLayout = new QHBoxLayout;
    FinalLayout->setContentsMargins(3, 3, 3, 3);
    FinalLayout->addLayout(LeftSideLayout, 1);
    FinalLayout->addLayout(RightSideLayout, 9);
    setLayout(FinalLayout);
    adjustSize();
}
void LocationViewer::init_connections(void)
{
    connect(locationTable, &QTableWidget::currentCellChanged, this, &LocationViewer::itemChanged);
    connect(lineTableFilter, &QLineEdit::textChanged, this, &LocationViewer::filterLocations);
    connect(btnSearchOptions, &QCheckBox::clicked, lineTableFilter, &QLineEdit::clear);
    connect(actionNameSearch, &QAction::toggled, this, &LocationViewer::actionNameSearchToggled);
    connect(actionItemSearch, &QAction::toggled, this, &LocationViewer::actionItemSearchToggled);
    connect(actionRegExpSearch, &QAction::toggled, this, &LocationViewer::actionRegExpSearchToggled);
    connect(actionCaseSensitive, &QAction::toggled, this, &LocationViewer::actionCaseSensitiveToggled);
    connect(sbMapID, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbMapIdChanged);
    connect(sbLocID, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbLocIdChanged);
    connect(sbX, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbXChanged);
    connect(sbY, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbYChanged);
    connect(sbT, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbTChanged);
    connect(sbD, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbDChanged);
    connect(lineLocationName, &QLineEdit::textChanged, this, &LocationViewer::lineLocationNameChanged);
    connect(fieldItemList, &QListWidget::clicked, this, &LocationViewer::fieldItemListItemChanged);
}
void LocationViewer::init_disconnect(void)
{
    disconnect(locationTable, &QTableWidget::currentCellChanged, this, &LocationViewer::itemChanged);
    disconnect(lineTableFilter, &QLineEdit::textChanged, this, &LocationViewer::filterLocations);
    disconnect(btnSearchOptions, &QCheckBox::clicked, lineTableFilter, &QLineEdit::clear);
    disconnect(actionNameSearch, &QAction::toggled, this, &LocationViewer::actionNameSearchToggled);
    disconnect(actionItemSearch, &QAction::toggled, this, &LocationViewer::actionItemSearchToggled);
    disconnect(actionRegExpSearch, &QAction::toggled, this, &LocationViewer::actionRegExpSearchToggled);
    disconnect(actionCaseSensitive, &QAction::toggled, this, &LocationViewer::actionCaseSensitiveToggled);
    disconnect(sbMapID, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbMapIdChanged);
    disconnect(sbLocID, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbLocIdChanged);
    disconnect(sbX, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbXChanged);
    disconnect(sbY, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbYChanged);
    disconnect(sbT, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbTChanged);
    disconnect(sbD, QOverload<int>::of(&QSpinBox::valueChanged), this, &LocationViewer::sbDChanged);
    disconnect(lineLocationName, &QLineEdit::textChanged, this, &LocationViewer::lineLocationNameChanged);
    disconnect(fieldItemList, &QListWidget::clicked, this, &LocationViewer::fieldItemListItemChanged);
}

void LocationViewer::itemChanged(int currentRow, int currentColumn, int prevRow, int prevColumn)
{
    Q_UNUSED(currentColumn)
    Q_UNUSED(prevColumn)

    if (currentRow != prevRow) {
        int mapID = Locations->mapID(locationTable->item(currentRow, 0)->text()).toInt();
        int locID = Locations->locationID(locationTable->item(currentRow, 0)->text()).toInt();
        setLocation(mapID, locID);
        if (autoUpdate) {
            emit(locationChanged(Locations->fileName(mapID, locID)));
        }
    }
}

void LocationViewer::setSelected(QString locFilename)
{
    locationTable->setCurrentItem(locationTable->item(-1, -1));
    for (int i = 0; i < Locations->size(); i++) {
        if (locationTable->item(i, 0)->text() == locFilename) {
            bool u = autoUpdate;
            autoUpdate = true;
            locationTable->setCurrentItem(locationTable->item(i, 0));
            autoUpdate = u;
            break;
        }
    }
}

void LocationViewer::sbMapIdChanged(int mapId)
{
    setLocation(mapId, sbLocID->value());
    QString fileName = Locations->fileName(mapId, sbLocID->value());
    if (fileName.isEmpty()) {
        emit(mapIdChanged(mapId));
    } else {
        emit(locationChanged(fileName));
    }
}

void LocationViewer::sbLocIdChanged(int locId)
{
    setLocation(sbMapID->value(), locId);
    QString fileName = Locations->fileName(sbMapID->value(), locId);
    if (fileName.isEmpty()) {
        emit(locIdChanged(locId));
    } else {
        emit(locationChanged(fileName));
    }
}

void LocationViewer::sbXChanged(int x)
{
    emit(xChanged(x));
}

void LocationViewer::sbYChanged(int y)
{
    emit(yChanged(y));
}

void LocationViewer::sbTChanged(int t)
{
    emit(tChanged(t));
}

void LocationViewer::sbDChanged(int d)
{
    emit(dChanged(d));
}

void LocationViewer::setLocation(int mapId, int locId)
{
    init_disconnect();
    QString fileName = Locations->fileName(mapId, locId);
    setSelected(fileName);

    if (fileName.isEmpty()) {
        lblLocationPreview->setPixmap(QString());
    } else {
        lblLocationPreview->setPixmap(QPixmap(QStringLiteral("://locations/%1_%2").arg(QString::number(mapId), QString::number(locId))).scaledToWidth(lblLocationPreview->width(), Qt::SmoothTransformation));
        QString oldStr = Locations->locationString(fileName);
        QString newStr = translate(oldStr);
        if (oldStr != newStr && autoUpdate) {
            emit(locationStringChanged(newStr));
            qWarning() << QStringLiteral("LocationString Changed: %1").arg(newStr);
        }
        sbMapID->setValue(Locations->mapID(fileName).toInt());
        sbLocID->setValue(Locations->locationID(fileName).toInt());
        sbX->setValue(Locations->x(fileName).toInt());
        sbY->setValue(Locations->y(fileName).toInt());
        sbT->setValue(Locations->t(fileName).toInt());
        sbD->setValue(Locations->d(fileName).toInt());
        lineLocationName->setText(newStr);
        init_fieldItems();
    }
    init_connections();
}

void LocationViewer::lineLocationNameChanged(QString locName)
{
    emit(locationStringChanged(locName)); qWarning() << QStringLiteral("LocationString Changed: %1").arg(locName);
}

void LocationViewer::setX(int x)
{
    init_disconnect(); sbX->setValue(x); init_connections();
}

void LocationViewer::setY(int y)
{
    init_disconnect(); sbY->setValue(y); init_connections();
}

void LocationViewer::setT(int t)
{
    init_disconnect(); sbT->setValue(t); init_connections();
}

void LocationViewer::setD(int d)
{
    init_disconnect(); sbD->setValue(d); init_connections();
}

void LocationViewer::setMapId(int mapId)
{
    sbMapID->setValue(mapId);
}

void LocationViewer::setLocationId(int locId)
{
    sbLocID->setValue(locId);
}

void LocationViewer::setLocationString(QString locString)
{
    init_disconnect();
    QString newStr = translate(locString);
    lineLocationName->setText(newStr);
    if (locString != newStr && autoUpdate) {
        emit(locationStringChanged(newStr));
        qWarning() << QStringLiteral("LocationString Changed: %1").arg(newStr);
    }
    init_connections();
}

void LocationViewer::chkAutoUpdateChanged(bool checked)
{
    autoUpdate = checked;
    if (checked) {
        emit locationChanged(Locations->fileName(sbMapID->value(), sbLocID->value()));
    }
}

void LocationViewer::setHorizontalHeaderStyle(QString styleSheet)
{
    locationTable->horizontalHeader()->setStyleSheet(styleSheet);
}

void LocationViewer::setRegion(QString newRegion)
{
    region = newRegion; setLocation(sbMapID->value(), sbLocID->value());
}

void LocationViewer::setTranslationBaseFile(QString basePathName)
{
    transBasePath = basePathName;
}

QString LocationViewer::translate(QString text)
{
    if (region == "") {
        qWarning() << "Translate: No Region"; return text;
    }

    if (transBasePath == "") {
        qWarning() << "Translate: No Base Path"; return text;
    } else {
        QString lang = transBasePath;
        QTranslator Translator;// will do the translating.
        QString reg = region;// remove trailing  FF7-SXX
        reg.chop(7);
        if (reg == "BASCUS-94163" || reg == "BESCES-00867") {
            lang.append("en.qm");
        } else if (reg == "BESCES-00868") {
            lang.append("fr.qm");
        } else if (reg == "BESCES-00869") {
            lang.append("de.qm");
        } else if (reg == "BESCES-00900") {
            lang.append("es.qm");
        } else if (reg == "BISLPS-00700" || reg == "BISLPS-01057") {
            lang.append("ja.qm");
        } else {//unknown language.
            qWarning() << QString("Unknown Region:%1").arg(reg);
            return text;
        }
        Translator.load(lang);
        QString newText = Translator.translate("Locations", text.toUtf8());
        if (newText.isEmpty()) {
            return text;
        } else {
            return newText;
        }
    }
}

void LocationViewer::filterLocations(QString filter)
{
    if (filter.isEmpty()) {
        for (int i = 0; i < locationTable->rowCount(); i++) {
            locationTable->setRowHidden(i, false);
        }
        return;
    }

    QRegExp exp(filter);
    exp.setPatternSyntax(regExpSearch ? QRegExp::Wildcard
                         : QRegExp::FixedString);

    exp.setCaseSensitivity(caseSensitive ? Qt::CaseSensitive
                           : Qt::CaseInsensitive);

    switch (searchMode) {
    case NAME: searchName(exp); break;
    case ITEM: searchItem(exp); break;
    }
}

void LocationViewer::actionNameSearchToggled(bool checked)
{
    if (checked) {
        actionItemSearchToggled(false);
        searchMode = NAME;
        lineTableFilter->setPlaceholderText(actionNameSearch->text());
        if (!lineTableFilter->text().isEmpty()) {
            filterLocations(lineTableFilter->text());
        }
    } else {
        actionNameSearch->setChecked(false);
    }
}

void LocationViewer::actionItemSearchToggled(bool checked)
{
    if (checked) {
        actionNameSearchToggled(false);
        searchMode = ITEM;
        lineTableFilter->setPlaceholderText(actionItemSearch->text());
        if (!lineTableFilter->text().isEmpty()) {
            filterLocations(lineTableFilter->text());
        }
    } else {
        actionItemSearch->setChecked(false);
    }
}

void LocationViewer::actionRegExpSearchToggled(bool checked)
{
    regExpSearch = checked;
    if (lineTableFilter->text().isEmpty()) {
        filterLocations(lineTableFilter->text());
    }

}
void LocationViewer::actionCaseSensitiveToggled(bool checked)
{
    caseSensitive = checked;
    if (!lineTableFilter->text().isEmpty()) {
        filterLocations(lineTableFilter->text());
    }
}

void LocationViewer::init_fieldItems(void)
{
    groupFieldItems->setVisible(false);
    fieldItemList->clear();
    QString fieldFileName = Locations->fileName(sbMapID->value(), sbLocID->value()); //store our current field's FileName
    if (fieldFileName.isEmpty()) {
        fieldItemList->setFixedHeight(0);
        return;
    } else {
        for (int i = 0; i < fieldItems->size(); i++) {
            for (int j = 0; j < fieldItems->maps(i).count(); j++) {
                if (fieldItems->maps(i).at(j) == fieldFileName) {
                    QListWidgetItem *newItem = new QListWidgetItem(fieldItems->text(i));
                    newItem->setCheckState(Qt::Unchecked);
                    fieldItemList->addItem(newItem);
                    emit fieldItemConnectRequest(quint8(fieldItemList->count()) - 1, fieldItems->offset(i), fieldItems->bit(i));
                    emit fieldItemCheck(fieldItemList->count() - 1);
                }
            }
        }
        if (fieldItemList->count() > 0) {
            groupFieldItems->setVisible(true);
        }

        if (fieldItemList->count() <= 5) {
            fieldItemList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            fieldItemList->setFixedHeight(fieldItemList->sizeHintForRow(0)*fieldItemList->count() + fieldItemList->contentsMargins().top() + fieldItemList->contentsMargins().bottom());
        } else {
            fieldItemList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            fieldItemList->setFixedHeight(fieldItemList->sizeHintForRow(0) * 5 + fieldItemList->contentsMargins().top() + fieldItemList->contentsMargins().bottom());
        }
        groupFieldItems->setFixedHeight(fieldItemList->contentsRect().height() + groupFieldItems->contentsMargins().bottom() + groupFieldItems->contentsMargins().top());
    }
}

void LocationViewer::fieldItemListItemChanged(QModelIndex index)
{
    bool checked;
    if (fieldItemList->item(index.row())->checkState() == Qt::Checked) {
        checked = true;
    } else {
        checked = false;
    }
    emit fieldItemChanged(index.row(), checked);

}
void LocationViewer::setFieldItemChecked(int row, bool checked)
{
    init_disconnect();
    if (fieldItemList->count() > row) {
        if (checked) {
            fieldItemList->item(row)->setCheckState(Qt::Checked);
        } else {
            fieldItemList->item(row)->setCheckState(Qt::Unchecked);
        }
    }
    init_connections();
}
void LocationViewer::searchName(QRegExp exp)
{
    for (int i = 0; i < locationTable->rowCount(); i++) {
        bool hidden = true;
        for (int j = 0; j < locationTable->columnCount(); j++) {
            if (locationTable->item(i, j)->text().contains(exp)) {
                hidden = false;
                break;
            }
        }
        locationTable->setRowHidden(i, hidden);
    }
}
void LocationViewer::searchItem(QRegExp exp)
{
    QStringList locationNames;
    for (const FieldItem &fieldItem : fieldItems->fieldItemList()) {
        for (const QString &item : qAsConst(fieldItem.Maps)) {
            if (item.contains(exp)) {
                locationNames.append(item);
            }
        }
    }

    for (int i = 0; i < locationTable->rowCount(); i++) {
        bool hidden = true;
        for (int j = 0; j < locationNames.count(); j++) {
            if (locationTable->item(i, 0)->text() == locationNames.at(j)) {
                hidden = false;
                break;
            }
        }
        locationTable->setRowHidden(i, hidden);
    }
}

void LocationViewer::setAdvancedMode(bool advancedMode)
{
    _advancedMode = advancedMode;
    sbMapID->setVisible(_advancedMode);
    sbLocID->setVisible(_advancedMode);
    sbX->setVisible(_advancedMode);
    sbY->setVisible(_advancedMode);
    sbT->setVisible(_advancedMode);
    sbD->setVisible(_advancedMode);
}

bool LocationViewer::advancedMode(void)
{
    return _advancedMode;
}

void LocationViewer::setFilterString(QString filter, filterMode mode)
{
    switch (mode) {
    case NAME:
        actionNameSearch->setChecked(true);
        break;
    case ITEM:
        actionItemSearch->setChecked(true);
        break;
    }
    lineTableFilter->setText(filter);
    setLocationChangesSaved(false);
    for (int i = 0; i < locationTable->rowCount(); i++) {
        if (!locationTable->isRowHidden(i)) {
            locationTable->selectRow(i);
            return;
        }
    }
}

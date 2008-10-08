/*
 *      keylist.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "keylist.h"

KeyList::KeyList(GpgME::Context* ctx, QString iconpath, QWidget *parent)
        : QWidget(parent)
{
    mCtx = ctx;
    this->iconPath = iconpath;

    mKeyList = new QTableWidget(this);
    mKeyList->setColumnCount(5);
    mKeyList->verticalHeader()->hide();
    mKeyList->setShowGrid(false);
    mKeyList->setColumnWidth(0, 24);
    mKeyList->setColumnWidth(1, 20);
    mKeyList->sortByColumn(2,Qt::AscendingOrder);
    mKeyList->setSelectionBehavior(QAbstractItemView::SelectRows);
    mKeyList->setColumnHidden(4, true);
    // tableitems not editable
    mKeyList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // no focus (rectangle around tableitems)
    // may be it should focus on whole row
    mKeyList->setFocusPolicy(Qt::NoFocus);

    mKeyList->setAlternatingRowColors(true);

    QStringList labels;
    labels << "" << "" << "Name" << "EMail" << "id";
    mKeyList->setHorizontalHeaderLabels(labels);
    mKeyList->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mKeyList);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);
    setLayout(layout);

    popupMenu = new QMenu(this);
	connect(mCtx, SIGNAL(keyDBChanged()), this, SLOT(refresh()));
    refresh();
}

void KeyList::refresh()
{
    // while filling the table, sort enabled causes errors
    mKeyList->setSortingEnabled(false);
    mKeyList->clearContents();

    GpgKeyList keys = mCtx->listKeys();
    mKeyList->setRowCount(keys.size());

    int row=0;
    GpgKeyList::iterator it = keys.begin();
    while (it != keys.end()) {

        QTableWidgetItem *tmp0 = new QTableWidgetItem();
        tmp0->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tmp0->setCheckState(Qt::Unchecked);
        mKeyList->setItem(row, 0, tmp0);

        if(it->privkey) {
            QTableWidgetItem *tmp1 = new QTableWidgetItem(QIcon(iconPath + "kgpg_key2.png"),"");
            mKeyList->setItem(row, 1, tmp1);
        }
        QTableWidgetItem *tmp2 = new QTableWidgetItem(it->name);
        tmp2->setToolTip(it->name);
        mKeyList->setItem(row, 2, tmp2);
        QTableWidgetItem *tmp3 = new QTableWidgetItem(it->email);
        tmp3->setToolTip(it->email);
        mKeyList->setItem(row, 3, tmp3);
        QTableWidgetItem *tmp4 = new QTableWidgetItem(it->id);
        mKeyList->setItem(row, 4, tmp4);
        it++;
        ++row;
    }
    mKeyList->setSortingEnabled(true);
}


QList<QString> *KeyList::getChecked()
{
    QList<QString> *ret = new QList<QString>();
    for (int i = 0; i < mKeyList->rowCount(); i++) {
        if (mKeyList->item(i,0)->checkState() == Qt::Checked) {
            *ret << mKeyList->item(i,4)->text();
        }
    }
    return ret;
}

QList<QString> *KeyList::getSelected()
{
    QList<QString> *ret = new QList<QString>();

    for (int i = 0; i < mKeyList->rowCount(); i++) {
        if (mKeyList->item(i,0)->isSelected() == 1) {
            *ret << mKeyList->item(i,4)->text();
        }
    }
    return ret;
}

void KeyList::setColumnWidth(int row, int size)
{
    mKeyList->setColumnWidth(row, size);
}

void KeyList::contextMenuEvent(QContextMenuEvent *event)
{
    popupMenu->exec(event->globalPos());
}

void KeyList::addMenuAction(QAction *act)
{
    popupMenu->addAction(act);
}

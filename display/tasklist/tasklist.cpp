/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "taskList.h"
#include "filterwidget.h"
#include "commander.h"

#include <QtWidgets>
#include <QVector>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>

TaskList::TaskList(QWidget *parent,Commander *control): QWidget(parent),cmd(control)
{
    proxyModel = new QSortFilterProxyModel(this);

    QLabel *focusLabel = new QLabel(tr("&focus"));
    Q_CHECK_PTR(focusLabel);
    focus = new QComboBox(this);
    Q_CHECK_PTR(focus);

    focus->addItem(tr("CPU"));
    focus->addItem(tr("Memory"));

    focusLabel->setBuddy(focus);

    number = new QSpinBox(this);
    Q_CHECK_PTR(number);
    number->setValue(5);
    number->setMinimum(5);
    number->setMaximum(50);
    number->setPrefix(tr("number "));

    filterWidget = new FilterWidget;
    connect(filterWidget, &FilterWidget::filterChanged, this, &TaskList::textFilterChanged);

    QLabel * filterPatternLabel = new QLabel(tr("&process filter:"));
    filterPatternLabel->setBuddy(filterWidget);

    connect(filterWidget, &QLineEdit::textChanged,
            this, &TaskList::textFilterChanged);

    proxyView = new QTreeView;
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    proxyView->setSortingEnabled(true);
    proxyView->sortByColumn(1, Qt::AscendingOrder);

    QGridLayout *proxyLayout = new QGridLayout;

    proxyLayout->addWidget(focusLabel, 0, 0,1,1);
    proxyLayout->addWidget(focus, 0, 1,1,1);

    proxyLayout->addWidget(number, 0, 2,1,1);

    proxyLayout->addWidget(filterPatternLabel, 0, 3,1,1);
    proxyLayout->addWidget(filterWidget, 0, 4, 1, 1);

    proxyLayout->addWidget(proxyView, 1, 0, 1, 5);


    setLayout(proxyLayout);

    setWindowTitle(tr("task list"));
    setWindowIcon(QIcon(":/images/images/task.png"));
    resize(500, 450);

    createTaskModel();

    connect(cmd, &Commander::psResultTaskList,
            this, &TaskList::disTaskList);
}
void TaskList::disTaskList(const QVector<QMap<QString, QString>> &info)
{
    model->removeRows(0, model->rowCount());
    for(int i = 0; i < info.size(); ++i)
    {
        model->insertRow(0);
        model->setData(model->index(0, 0), info[i].value("name"));
        model->setData(model->index(0, 1), info[i].value("pid"));
        model->setData(model->index(0, 2), info[i].value("state"));
        model->setData(model->index(0, 3), info[i].value("priority"));
        model->setData(model->index(0, 4), info[i].value("nice"));
        model->setData(model->index(0, 5), info[i].value("threads"));
        model->setData(model->index(0, 6), info[i].value("cpu"));
        model->setData(model->index(0, 7), info[i].value("rss"));
    }

    setSourceModel(model);
}
void TaskList::createTaskModel()
{
    model = new QStandardItemModel(0, 8, this);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("PID"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("state"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("priority"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("nice"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("threads"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("cpu"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("RSS(MB)"));
}

void TaskList::setSourceModel(QAbstractItemModel *model)
{
    proxyModel->setSourceModel(model);

    for (int i = 0; i < proxyModel->columnCount(); ++i)
        proxyView->resizeColumnToContents(i);
}

void TaskList::textFilterChanged()
{
    QRegExp regExp(filterWidget->text(),
                   filterWidget->caseSensitivity(),
                   filterWidget->patternSyntax());
    proxyModel->setFilterRegExp(regExp);
}
void TaskList::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
}
void TaskList::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
}
void TaskList::execTaskList()
{
    task_list_focus fcs = FOCUS_CPU;
    if(focus->currentText() == "Memory")
    {
        fcs = FOCUS_MEM;
    }

    cmd->requestTaskList(fcs, number->value());
}

/********************************************************************************
** Form generated from reading UI file 'calibrationdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATIONDIALOG_H
#define UI_CALIBRATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_CalibrationDialog
{
public:
    QComboBox *comboBox;
    QPushButton *calibButton;
    QVTKWidget *qvtkWidget;
    QPushButton *getCloudsButton;

    void setupUi(QDialog *CalibrationDialog)
    {
        if (CalibrationDialog->objectName().isEmpty())
            CalibrationDialog->setObjectName(QString::fromUtf8("CalibrationDialog"));
        CalibrationDialog->resize(1150, 703);
        comboBox = new QComboBox(CalibrationDialog);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(300, 10, 241, 41));
        calibButton = new QPushButton(CalibrationDialog);
        calibButton->setObjectName(QString::fromUtf8("calibButton"));
        calibButton->setGeometry(QRect(570, 10, 241, 41));
        qvtkWidget = new QVTKWidget(CalibrationDialog);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
        qvtkWidget->setGeometry(QRect(10, 70, 1131, 621));
        getCloudsButton = new QPushButton(CalibrationDialog);
        getCloudsButton->setObjectName(QString::fromUtf8("getCloudsButton"));
        getCloudsButton->setGeometry(QRect(20, 10, 241, 41));

        retranslateUi(CalibrationDialog);

        QMetaObject::connectSlotsByName(CalibrationDialog);
    } // setupUi

    void retranslateUi(QDialog *CalibrationDialog)
    {
        CalibrationDialog->setWindowTitle(QApplication::translate("CalibrationDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("CalibrationDialog", "coppia di camere", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("CalibrationDialog", "0-1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("CalibrationDialog", "0-2", 0, QApplication::UnicodeUTF8)
        );
        calibButton->setText(QApplication::translate("CalibrationDialog", "calibra", 0, QApplication::UnicodeUTF8));
        getCloudsButton->setText(QApplication::translate("CalibrationDialog", "Acquisisci nuvole", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CalibrationDialog: public Ui_CalibrationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATIONDIALOG_H

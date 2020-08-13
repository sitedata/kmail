/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "formatcomboboxtest.h"
#include <QTest>
#include "../widgets/formatcombobox.h"

FormatComboBoxTest::FormatComboBoxTest(QObject *parent)
    : QObject(parent)
{
}

FormatComboBoxTest::~FormatComboBoxTest()
{
}

void FormatComboBoxTest::shouldHaveDefaultValue()
{
    FormatComboBox combo;
    QCOMPARE(combo.count(), 4);
}

void FormatComboBoxTest::changeCurrentItem_data()
{
    QTest::addColumn<int>("input");
    QTest::addColumn<int>("output");
    QTest::newRow("first") <<  0 << 0;
    QTest::newRow("second") <<  1 << 1;
    QTest::newRow("third") <<  2 << 2;
    QTest::newRow("fourth") <<  3 << 3;
    QTest::newRow("invalid") <<  5 << 0;
}

void FormatComboBoxTest::changeCurrentItem()
{
    QFETCH(int, input);
    QFETCH(int, output);
    FormatComboBox combo;
    combo.setFormat(static_cast<MailCommon::BackupJob::ArchiveType>(input));
    QCOMPARE(combo.format(), static_cast<MailCommon::BackupJob::ArchiveType>(output));
}

QTEST_MAIN(FormatComboBoxTest)

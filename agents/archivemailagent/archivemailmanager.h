/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ARCHIVEMAILMANAGER_H
#define ARCHIVEMAILMANAGER_H

#include <QObject>

#include <KSharedConfig>
#include <Collection>

class ArchiveMailKernel;
class ArchiveMailInfo;

class ArchiveMailManager : public QObject
{
    Q_OBJECT
public:
    explicit ArchiveMailManager(QObject *parent = nullptr);
    ~ArchiveMailManager();
    void removeCollection(const Akonadi::Collection &collection);
    void backupDone(ArchiveMailInfo *info);
    void pause();
    void resume();

    Q_REQUIRED_RESULT QString printArchiveListInfo() const;
    void collectionDoesntExist(ArchiveMailInfo *info);

    Q_REQUIRED_RESULT QString printCurrentListInfo() const;

    void archiveFolder(const QString &path, Akonadi::Collection::Id collectionId);

    ArchiveMailKernel *kernel() const
    {
        return mArchiveMailKernel;
    }

public Q_SLOTS:
    void load();

Q_SIGNALS:
    void needUpdateConfigDialogBox();

private:
    Q_DISABLE_COPY(ArchiveMailManager)
    QString infoToStr(ArchiveMailInfo *info) const;
    void removeCollectionId(Akonadi::Collection::Id id);
    KSharedConfig::Ptr mConfig;
    QVector<ArchiveMailInfo *> mListArchiveInfo;
    ArchiveMailKernel *mArchiveMailKernel = nullptr;
};

#endif /* ARCHIVEMAILMANAGER_H */

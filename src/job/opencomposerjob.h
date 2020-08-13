/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OPENCOMPOSERJOB_H
#define OPENCOMPOSERJOB_H

#include <QObject>
#include <KMime/Message>
#include "editor/kmcomposerwin.h"
struct OpenComposerSettings
{
    OpenComposerSettings()
        : mHidden(false)
    {
    }

    OpenComposerSettings(const QString &to, const QString &cc, const QString &bcc, const QString &subject, const QString &body, bool hidden, const QString &messageFile, const QStringList &attachmentPaths, const QStringList &customHeaders, const QString &replyTo, const QString &inReplyTo,
                         const QString &identity)
        : mAttachmentPaths(attachmentPaths)
        , mCustomHeaders(customHeaders)
        , mTo(to)
        , mCc(cc)
        , mBcc(bcc)
        , mSubject(subject)
        , mBody(body)
        , mMessageFile(messageFile)
        , mReplyTo(replyTo)
        , mInReplyTo(inReplyTo)
        , mIdentity(identity)
        , mHidden(hidden)
    {
    }

    QStringList mAttachmentPaths;
    QStringList mCustomHeaders;
    QString mTo;
    QString mCc;
    QString mBcc;
    QString mSubject;
    QString mBody;
    QString mMessageFile;
    QString mReplyTo;
    QString mInReplyTo;
    QString mIdentity;
    bool mHidden = false;
};

class OpenComposerJob : public QObject
{
    Q_OBJECT
public:
    explicit OpenComposerJob(QObject *parent = nullptr);
    ~OpenComposerJob();
    void start();

    void setOpenComposerSettings(const OpenComposerSettings &openComposerSettings);

private:
    Q_DISABLE_COPY(OpenComposerJob)
    void slotOpenComposer();
    OpenComposerSettings mOpenComposerSettings;
    KMime::Message::Ptr mMsg = nullptr;
    KMail::Composer::TemplateContext mContext = KMail::Composer::New;
    uint mIdentityId = 0;
};

#endif // OPENCOMPOSERJOB_H

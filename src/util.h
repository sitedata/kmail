/*******************************************************************************
**
** Filename   : util
** Created on : 03 April, 2005
** Copyright  : (c) 2005 Till Adam
** Email      : <adam@kde.org>
**
*******************************************************************************/

/*******************************************************************************
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   It is distributed in the hope that it will be useful, but
**   WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**   General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
**   In addition, as a special exception, the copyright holders give
**   permission to link the code of this program with any edition of
**   the Qt library by Trolltech AS, Norway (or with modified versions
**   of Qt that use the same license as Qt), and distribute linked
**   combinations including the two.  You must obey the GNU General
**   Public License in all respects for all of the code used other than
**   Qt.  If you modify this file, you may extend this exception to
**   your version of the file, but you are not obligated to do so.  If
**   you do not wish to do so, delete this exception statement from
**   your version.
**
*******************************************************************************/
#ifndef KMAILUTIL_H
#define KMAILUTIL_H

#include <QWidget>
#include <AkonadiCore/item.h>
#include <AkonadiCore/Collection>
#include <MailCommon/FolderCollection>

#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
namespace QtPrivate
{
template <typename T> struct QAddConst {
    typedef const T Type;
};
}

// this adds const to non-const objects (like std::as_const)
template <typename T>
Q_DECL_CONSTEXPR typename QtPrivate::QAddConst<T>::Type &qAsConst(T &t) Q_DECL_NOTHROW { return t; }
// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) Q_DECL_EQ_DELETE;
#endif

namespace KMail
{
/**
     * The Util namespace contains a collection of helper functions use in
     * various places.
     */
namespace Util
{

/**
     * Returns any mailing list post addresses set on the
     *  parent collection (the mail folder) of the item.
     */
KMime::Types::Mailbox::List mailingListsFromMessage(const Akonadi::Item &item);

/**
     * Whether or not the mail item has the keep-reply-in-folder
     *  attribute set.
     */
Akonadi::Item::Id putRepliesInSameFolder(const Akonadi::Item &item);

/**
     * Handles a clicked URL, but only in case the viewer didn't handle it.
     * Currently only support mailto.
     */
bool handleClickedURL(const QUrl &url, const QSharedPointer<MailCommon::FolderCollection> &folder = QSharedPointer<MailCommon::FolderCollection>());

bool mailingListsHandleURL(const QList<QUrl> &lst, const QSharedPointer<MailCommon::FolderCollection> &folder);

bool mailingListPost(const QSharedPointer<MailCommon::FolderCollection> &fd);
bool mailingListSubscribe(const QSharedPointer<MailCommon::FolderCollection> &fd);
bool mailingListUnsubscribe(const QSharedPointer<MailCommon::FolderCollection> &fd);
bool mailingListArchives(const QSharedPointer<MailCommon::FolderCollection> &fd);
bool mailingListHelp(const QSharedPointer<MailCommon::FolderCollection> &fd);

void lastEncryptAndSignState(bool &lastEncrypt, bool &lastSign, const KMime::Message::Ptr &msg);

void addQActionHelpText(QAction *action, const QString &text);
}
}

#endif

/*  -*- mode: C++; c-file-style: "gnu" -*-
 *
 *  This file is part of KMail, the KDE mail client.
 *  Copyright (c) 2003 Zack Rusin <zack@kde.org>
 *
 *  KMail is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License, version 2, as
 *  published by the Free Software Foundation.
 *
 *  KMail is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of this program with any edition of
 *  the Qt library by Trolltech AS, Norway (or with modified versions
 *  of Qt that use the same license as Qt), and distribute linked
 *  combinations including the two.  You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  Qt.  If you modify this file, you may extend this exception to
 *  your version of the file, but you are not obligated to do so.  If
 *  you do not wish to do so, delete this exception statement from
 *  your version.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ssllabel.h"

#include <kiconloader.h>
#include <klocale.h>

#include <qtooltip.h>

namespace KMail {

SSLLabel::SSLLabel( QWidget* parent )
  : QLabel( parent )
{
  setState( Done );
}

void SSLLabel::setEncrypted( bool enc )
{
  if ( enc ) {
    m_lastEncryptionState = Encrypted;
  } else {
    m_lastEncryptionState = Unencrypted;
  }
}

SSLLabel::State SSLLabel::lastState() const
{
  return m_lastEncryptionState;
}

void SSLLabel::setState( State state )
{
  switch( state ) {
  case Encrypted:
    QToolTip::remove( this );
    QToolTip::add( this, i18n("Connection is encrypted") );
    setPixmap( SmallIcon( "encrypted", KGlobal::instance() ) );
    show();
    break;
  case Unencrypted:
    QToolTip::remove( this );
    QToolTip::add( this, i18n("Connection is unencrypted") );
    setPixmap( SmallIcon( "decrypted" ) );
    show();
    break;
  case Done:
    QToolTip::remove( this );
    hide();
    break;
  case Clean:
  default:
    QToolTip::remove( this );
    hide();
    //we return because we do not save the state as the only
    //action we want to perform is to hide ourself
    return;
  }
  m_lastEncryptionState = state;
}


} //end namespace KMail

//#include "ssllabel.moc"

/*
    A node in a MIME tree.

    Copyright (C) 2002 by Klarälvdalens Datakonsult AB

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include "partNode.h"
#include <klocale.h>
#include <kdebug.h>
#include "kmmimeparttree.h"
#include <mimelib/utility.h>
#include <qregexp.h>

/*
  ===========================================================================


  S T A R T    O F     T E M P O R A R Y     M I M E     C O D E


  ===========================================================================
  N O T E :   The partNode structure will most likely be replaced by KMime.
  It's purpose: Speed optimization for KDE 3.   (khz, 28.11.01)
  ===========================================================================
*/

void partNode::buildObjectTree( bool processSiblings )
{
    partNode* curNode = this;
    while( curNode && curNode->dwPart() ) {
        //dive into multipart messages
        while( DwMime::kTypeMultipart == curNode->type() ) {
            partNode* newNode = curNode->setFirstChild(
                new partNode( curNode->dwPart()->Body().FirstBodyPart() ) );
            curNode = newNode;
        }
        // go up in the tree until reaching a node with next
        // (or the last top-level node)
        while(     curNode
               && !(    curNode->dwPart()
                     && curNode->dwPart()->Next() ) ) {
            curNode = curNode->mRoot;
        }
        // we might have to leave when all children have been processed
        if( this == curNode && !processSiblings )
            return;
        // store next node
        if( curNode && curNode->dwPart() && curNode->dwPart()->Next() ) {
            partNode* nextNode = new partNode( curNode->dwPart()->Next() );
            curNode = curNode->setNext( nextNode );
        } else
            curNode = 0;
    }
}

QCString partNode::typeString() const {
  DwString s;
  DwTypeEnumToStr( type(), s );
  return s.c_str();
}

QCString partNode::subTypeString() const {
  DwString s;
  DwSubtypeEnumToStr( subType(), s );
  return s.c_str();
}

partNode::CryptoType partNode::firstCryptoType() const
{
    CryptoType ret = cryptoType();
    if(    (CryptoTypeUnknown == ret || CryptoTypeNone == ret)
        && mChild )
        ret = mChild->firstCryptoType();
    if(    (CryptoTypeUnknown == ret || CryptoTypeNone == ret)
        && mNext )
        ret = mNext->firstCryptoType();
    return ret;
}


KMMsgEncryptionState partNode::overallEncryptionState() const
{
    KMMsgEncryptionState myState = KMMsgEncryptionStateUnknown;
    if( mEncryptionState == KMMsgNotEncrypted ) {
        // NOTE: children are tested ONLY when parent is not encrypted
        if( mChild )
            myState = mChild->overallEncryptionState();
        else
            myState = KMMsgNotEncrypted;
    }
    else { // part is partially or fully encrypted
        myState = mEncryptionState;
    }
    // siblings are tested always
    if( mNext ) {
        KMMsgEncryptionState otherState = mNext->overallEncryptionState();
        switch( otherState ) {
        case KMMsgEncryptionStateUnknown:
            break;
        case KMMsgNotEncrypted:
            if( myState == KMMsgFullyEncrypted )
                myState = KMMsgPartiallyEncrypted;
            else if( myState != KMMsgPartiallyEncrypted )
                myState = KMMsgNotEncrypted;
            break;
        case KMMsgPartiallyEncrypted:
            myState = KMMsgPartiallyEncrypted;
            break;
        case KMMsgFullyEncrypted:
            if( myState != KMMsgFullyEncrypted )
                myState = KMMsgPartiallyEncrypted;
            break;
        case KMMsgEncryptionProblematic:
            break;
        }
    }

//kdDebug(5006) << "\n\n  KMMsgEncryptionState: " << myState << endl;

    return myState;
}


KMMsgSignatureState  partNode::overallSignatureState() const
{
    KMMsgSignatureState myState = KMMsgSignatureStateUnknown;
    if( mSignatureState == KMMsgNotSigned ) {
        // children are tested ONLY when parent is not signed
        if( mChild )
            myState = mChild->overallSignatureState();
        else
            myState = KMMsgNotSigned;
    }
    else { // part is partially or fully signed
        myState = mSignatureState;
    }
    // siblings are tested always
    if( mNext ) {
        KMMsgSignatureState otherState = mNext->overallSignatureState();
        switch( otherState ) {
        case KMMsgSignatureStateUnknown:
            break;
        case KMMsgNotSigned:
            if( myState == KMMsgFullySigned )
                myState = KMMsgPartiallySigned;
            else if( myState != KMMsgPartiallySigned )
                myState = KMMsgNotSigned;
            break;
        case KMMsgPartiallySigned:
            myState = KMMsgPartiallySigned;
            break;
        case KMMsgFullySigned:
            if( myState != KMMsgFullySigned )
                myState = KMMsgPartiallySigned;
            break;
        case KMMsgEncryptionProblematic:
            break;
        }
    }

//kdDebug(5006) << "\n\n  KMMsgSignatureState: " << myState << endl;

    return myState;
}


int partNode::nodeId()
{
    int curId = 0;
    partNode* rootNode = this;
    while( rootNode->mRoot )
        rootNode = rootNode->mRoot;
    return rootNode->calcNodeIdOrFindNode( curId, this, 0, 0 );
}


partNode* partNode::findId( int id )
{
    int curId = 0;
    partNode* rootNode = this;
    while( rootNode->mRoot )
        rootNode = rootNode->mRoot;
    partNode* foundNode;
    rootNode->calcNodeIdOrFindNode( curId, 0, id, &foundNode );
    return foundNode;
}


int partNode::calcNodeIdOrFindNode( int &curId, const partNode* findNode, int findId, partNode** foundNode )
{
    // We use the same algorithm to determine the id of a node and
    //                           to find the node when id is known.
    curId++;
    // check for node ?
    if( findNode && this == findNode )
        return curId;
    // check for id ?
    if(  foundNode && curId == findId ) {
        *foundNode = this;
        return curId;
    }
    if( mChild )
    {
        int res = mChild->calcNodeIdOrFindNode( curId, findNode, findId, foundNode );
        if (res != -1) return res;
    }
    if( mNext )
        return mNext->calcNodeIdOrFindNode( curId, findNode, findId, foundNode );

    if(  foundNode )
        *foundNode = 0;
    return -1;
}


partNode* partNode::findType( int type, int subType, bool deep, bool wide )
{
#ifndef NDEBUG
  DwString typeStr, subTypeStr;
  DwTypeEnumToStr( mType, typeStr );
  DwSubtypeEnumToStr( mSubType, subTypeStr );
  kdDebug(5006) << "partNode::findType() is looking at " << typeStr.c_str()
                << "/" << subTypeStr.c_str() << endl;
#endif
    if(    (mType != DwMime::kTypeUnknown)
           && (    (type == DwMime::kTypeUnknown)
                   || (type == mType) )
           && (    (subType == DwMime::kSubtypeUnknown)
                   || (subType == mSubType) ) )
        return this;
    else if( mChild && deep )
        return mChild->findType( type, subType, deep, wide );
    else if( mNext && wide )
        return mNext->findType(  type, subType, deep, wide );
    else
        return 0;
}

partNode* partNode::findTypeNot( int type, int subType, bool deep, bool wide )
{
    if(    (mType != DwMime::kTypeUnknown)
           && (    (type == DwMime::kTypeUnknown)
                   || (type != mType) )
           && (    (subType == DwMime::kSubtypeUnknown)
                   || (subType != mSubType) ) )
        return this;
    else if( mChild && deep )
        return mChild->findTypeNot( type, subType, deep, wide );
    else if( mNext && wide )
        return mNext->findTypeNot(  type, subType, deep, wide );
    else
        return 0;
}

void partNode::fillMimePartTree( KMMimePartTreeItem* parentItem,
                                 KMMimePartTree*     mimePartTree,
                                 QString labelDescr,
                                 QString labelCntType,
                                 QString labelEncoding,
                                 KIO::filesize_t size,
                                 bool revertOrder )
{
  if( parentItem || mimePartTree ) {

    if( mNext )
        mNext->fillMimePartTree( parentItem, mimePartTree,
                                 QString::null, QString::null, QString::null, 0,
                                 revertOrder );

    QString cntDesc, cntType, cntEnc;
    KIO::filesize_t cntSize = 0;

    if( labelDescr.isEmpty() ) {
        DwHeaders* headers = 0;
        if( mDwPart && mDwPart->hasHeaders() )
          headers = &mDwPart->Headers();
        if( headers && headers->HasSubject() )
            cntDesc = KMMsgBase::decodeRFC2047String( headers->Subject().AsString().c_str() );
        if( headers && headers->HasContentType()) {
            cntType = headers->ContentType().TypeStr().c_str();
            cntType += '/';
            cntType += headers->ContentType().SubtypeStr().c_str();
        }
        else
            cntType = "text/plain";
        if( cntDesc.isEmpty() )
            cntDesc = msgPart().contentDescription();
        if( cntDesc.isEmpty() )
            cntDesc = msgPart().name().stripWhiteSpace();
        if( cntDesc.isEmpty() )
            cntDesc = msgPart().fileName();
        if( cntDesc.isEmpty() ) {
            if( mRoot && mRoot->mRoot )
                cntDesc = i18n("internal part");
            else
                cntDesc = i18n("body part");
        }
        cntEnc = msgPart().contentTransferEncodingStr();
        if( mDwPart )
            cntSize = mDwPart->BodySize();
    } else {
        cntDesc = labelDescr;
        cntType = labelCntType;
        cntEnc  = labelEncoding;
        cntSize = size;
    }
    // remove linebreak+whitespace from folded Content-Description
    cntDesc.replace( QRegExp("\\n\\s*"), " " );

kdDebug(5006) << "      Inserting one item into MimePartTree" << endl;
kdDebug(5006) << "                Content-Type: " << cntType << endl;
    if( parentItem )
      mMimePartTreeItem = new KMMimePartTreeItem( parentItem,
                                                  this,
                                                  cntDesc,
                                                  cntType,
                                                  cntEnc,
                                                  cntSize,
                                                  revertOrder );
    else if( mimePartTree )
      mMimePartTreeItem = new KMMimePartTreeItem( mimePartTree,
                                                  this,
                                                  cntDesc,
                                                  cntType,
                                                  cntEnc,
                                                  cntSize );
    mMimePartTreeItem->setOpen( true );
    if( mChild )
        mChild->fillMimePartTree( mMimePartTreeItem, 0,
                                  QString::null, QString::null, QString::null, 0,
                                  revertOrder );

  }
}

void partNode::adjustDefaultType( partNode* node )
{
    // Only bodies of  'Multipart/Digest'  objects have
    // default type 'Message/RfC822'.  All other bodies
    // have default type 'Text/Plain'  (khz, 5.12.2001)
    if( node && DwMime::kTypeUnknown == node->type() ) {
        if(    node->mRoot
               && DwMime::kTypeMultipart == node->mRoot->type()
               && DwMime::kSubtypeDigest == node->mRoot->subType() ) {
            node->setType(    DwMime::kTypeMessage   );
            node->setSubType( DwMime::kSubtypeRfc822 );
        }
        else
            {
                node->setType(    DwMime::kTypeText     );
                node->setSubType( DwMime::kSubtypePlain );
            }
    }
}

bool partNode::isAttachment() const
{
  if( !dwPart() )
    return false;
  DwHeaders& headers = dwPart()->Headers();
  if( headers.HasContentDisposition() )
    return ( headers.ContentDisposition().DispositionType()
             == DwMime::kDispTypeAttachment );
  else
    return false;
}

bool partNode::hasContentDispositionInline() const
{
  if( !dwPart() )
    return false;
  DwHeaders& headers = dwPart()->Headers();
  if( headers.HasContentDisposition() )
    return ( headers.ContentDisposition().DispositionType()
             == DwMime::kDispTypeInline );
  else
    return false;
}

const QString& partNode::trueFromAddress() const
{
  const partNode* node = this;
  while( node->mFromAddress.isEmpty() && node->mRoot )
    node = node->mRoot;
  return node->mFromAddress;
}

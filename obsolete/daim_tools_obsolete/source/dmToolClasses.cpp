
/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

#define dmUseExtendedTools
#define dmUseToolPrefs
#include "daim_tools.h"
#include "dmToolFactory.h"
#include "dmToolClasses.h"

//-----------------------------------------------------------------------
static bool __CHECK_POINTS( const dmPoly* p, size_t m )
{
  if(p->Size()<m) {
    dmLOG(_TXT("ERROR : missing data in <CreateTool> !\n"));
    return false;
  }
  return true;
}
//-----------------------------------------------------------------------
#define CHECK_POINTS( p, n, t ) if(!__CHECK_POINTS(p,n)) {\
	                               delete t;              \
	                               return NULL;           \
                                }
//------------------------------------------
// Tools declarations
//------------------------------------------
__dmFN_DECLARE( dmCreateLine  )
{
  if(infos==NULL) infos = dmLineEdit::ClassInfo();
  dmLineEdit* pTool = dmLineEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmLine( (*pts)[0], (*pts)[1]));
  }
  
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateArrow )
{
  if(infos==NULL) infos =  dmLineExtEdit::ClassInfo();
  dmLineEdit* pTool = dmLineExtEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmLine((*pts)[0],(*pts)[1]));
  }
  
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateRect  )
{
  if(infos==NULL) infos =  dmRectEdit::ClassInfo();
  dmRectEdit* pTool = dmRectEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmRect((*pts)[0],(*pts)[1]));
  }
  return pTool; 
}

//------------------------------------------
__dmFN_DECLARE( dmCreateEllipse )
{
  if(infos==NULL) infos =  dmEllipseEdit::ClassInfo();
  dmEllipseEdit* pTool = dmEllipseEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmEllipse((*pts)[0],(*pts)[1].x,(*pts)[1].y));
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreatePolyLines  )
{
  if(infos==NULL) infos =  dmPolyEdit::ClassInfo();
  dmPolyEdit* pTool = dmPolyEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(*pts);
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateFreeHand  )
{
  if(infos==NULL) infos = dmFreeHandEdit::ClassInfo();  
  dmFreeHandEdit* pTool = dmFreeHandEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(*pts);
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateText  )
{
  if(infos==NULL) infos = dmText::ClassInfo();
  dmText* pTool = dmText::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmRect((*pts)[0],(*pts)[1]));
  }
  return pTool; 
}
//------------------------------------------
// End of tools declarations
//------------------------------------------
__dmTools bool  dmRegisterTool( const dmString::E* _class, dmClassInfo* pInfo, 
   dmTool* (*pfnCreate)( const dmString::E*, dmClassInfo* infos, const dmPoly* ),
   dmClassInfo* pPrefs )
{
  if((!_class || _class[0]=='\0') && pInfo!=NULL)
    _class =  pInfo->Name();

  if(_class && _class[0]!='\0') {
    dmTOOLFACTORY tf;
    tf.ClassName  = _class;
    tf.pClassInfo = pInfo;
    tf.pfnCreate  = pfnCreate;
    tf.pPrefsInfo = pPrefs;
    return dmToolFactory::Instance().RegisterClass(&tf);
  }
  return false;
}
//------------------------------------------
// Permet d'enregistrer une classe d'outils a
// en utilisant les attributs d'une classe de base
// Note : la classe enregistré et la classe de base
// doivent avoir un ancêtre commun
//------------------------------------------
__dmTools bool dmRegisterClass( const dmString::E* _class, const dmString::E* _base, dmClassInfo* pInfo, dmTool* (*pfnCreate)( const char_t*, dmClassInfo*, const dmPoly* ) )
{
  if((!_class || _class[0]=='\0') && pInfo!=NULL)
    _class =  pInfo->Name();

  if(_class && _class[0]!='\0') {
    dmLPTOOLFACTORY pFactory = dmToolFactory::Instance().GetFactory(_base);
    if(pFactory) {

      // Use base class factory
      if(pInfo && pfnCreate==NULL) 
         pfnCreate = pFactory->pfnCreate;

      return dmRegisterTool(_class,pInfo,pfnCreate,pFactory->pPrefsInfo);
    }
  }
  return false;
}
//------------------------------------------
__dmTools dmTool* dmCreateTool  ( const dmString::E* _class, const dmPoly* pts, dm_uint flags )
{
  return dmToolFactory::Instance().CreateTool(_class,pts,flags);
}
//------------------------------------------
__dmTools bool dmRegisterTools()
{
  static bool bRegistered = false;
  if(bRegistered) return true;

  bool bret = true;
  bret &= dmRegisterTool(dmTOOL_LINE          ,dmLineEdit::ClassInfo()          ,dmCreateLine      ,NULL);
  bret &= dmRegisterTool(dmTOOL_RECT          ,dmRectEdit::ClassInfo()          ,dmCreateRect      ,NULL);
  bret &= dmRegisterTool(dmTOOL_ELLIPSE       ,dmEllipseEdit::ClassInfo()       ,dmCreateEllipse   ,NULL);
  bret &= dmRegisterTool(dmTOOL_POLYLINES     ,dmPolyEdit::ClassInfo()          ,dmCreatePolyLines ,NULL);
  bret &= dmRegisterTool(dmTOOL_CLOSEDPOLY    ,dmClosedPolyEdit::ClassInfo()    ,dmCreatePolyLines ,NULL);
  bret &= dmRegisterTool(dmTOOL_OPENFREEHAND  ,dmFreeHandEdit::ClassInfo()      ,dmCreateFreeHand  ,NULL);
  bret &= dmRegisterTool(dmTOOL_CLOSEDFREEHAND,dmClosedFreeHandEdit::ClassInfo(),dmCreateFreeHand  ,NULL);
  bret &= dmRegisterTool(dmTOOL_ARROW         ,dmLineExtEdit::ClassInfo()       ,dmCreateArrow     ,dmToolArrowPrefs::ClassInfo());
  bret &= dmRegisterTool(dmTOOL_LABEL         ,dmText::ClassInfo()              ,dmCreateText      ,dmToolTextPrefs::ClassInfo());

  bRegistered = true;
  return bret;
}
//------------------------------------------
// retourne la description de classe
// pour les préférences associées à la classes '_class'
// Attention : '_class' doit être le nom retourné par
// dmClassInfo::Name() de l'objet dont on cherche les
// préférences ( cf dmToolFactory::GetFactory(...) )
//------------------------------------------
__dmTools dmClassInfo* dmGetToolPrefs( const dmString::E* _class  )
{
  dmLPTOOLFACTORY lpf = dmToolFactory::Instance().GetFactory(_class,true);
  return  (lpf ? lpf->pPrefsInfo : NULL );
}
//------------------------------------------

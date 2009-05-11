#define dmUseExtendedTools
#define dmUseToolPrefs
#define dmUseToolFactory
#include "daim_tools.h"

#include "advtools/dmRectEditEx.h"
#include "advtools/dmPolyEditEx.h"
#include "advtools/dmCircleEdit.h"
#include "advtools/dmPtsCircleEdit.h"
#include "advtools/dmSector.h"
#include "advtools/dmCaliper.h"
#include "advtools/dmAdvTools.h"

#include "dmStyleManager.h"

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
#define CHECK_POINTS( p, n, t ) if(!__CHECK_POINTS(p,n)) { delete t; return NULL; }
//------------------------------------------
__dmFN_DECLARE( dmCreateRectEx )
{
  if(infos==NULL) infos = dmRectEditEx::ClassInfo();  
  dmRectEditEx* pTool = dmRectEditEx::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmRect((*pts)[0],(*pts)[1]));
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateCircleEx )
{
  if(infos==NULL) infos = dmPtsCircleEdit::ClassInfo();  
  dmPtsCircleEdit* pTool = dmPtsCircleEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  
  if(pts) {
    CHECK_POINTS(pts,3,pTool)
    pTool->SetCoordinates(*pts);
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateCircle )
{
  if(infos==NULL) infos = dmCircleEdit::ClassInfo();
  dmCircleEdit* pTool = dmCircleEdit::Cast(infos->Create());
  dmASSERT(pTool!=NULL);

  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    int rx = (*pts)[1].x;
    int ry = (*pts)[1].y;
    dm_double radius2 = rx*rx+ry*ry;
    pTool->SetCoordinates(dmCircle((*pts)[0],(int)(sqrt(radius2)+0.5)));
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreatePolyEx )
{
  if(infos==NULL) infos = dmPolyEditEx::ClassInfo(); 
  dmPolyEditEx*   pTool = dmPolyEditEx::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(*pts);
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateSector )
{
  if(infos==NULL) infos = dmSector::ClassInfo(); 
  dmSector*   pTool = dmSector::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,3,pTool)
    pTool->SetCoordinates(*pts);
  }
  return pTool; 
}
//------------------------------------------
__dmFN_DECLARE( dmCreateCaliper )
{
  if(infos==NULL) infos = dmCaliper::ClassInfo(); 
  dmCaliper*   pTool = dmCaliper::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmLine((*pts)[0],(*pts)[1]));
  }
  return pTool; 
}
//------------------------------------------
// Create Caliper in line mode
//------------------------------------------
__dmFN_DECLARE( dmCreateCaliper2 )
{
  if(infos==NULL) infos = dmCaliper::ClassInfo(); 
  dmCaliper*   pTool = dmCaliper::Cast(infos->Create());
  dmASSERT(pTool!=NULL);
  
  pTool->ShowHorizon(false);
   
  if(pts) {
    CHECK_POINTS(pts,2,pTool)
    pTool->SetCoordinates(dmLine((*pts)[0],(*pts)[1]));
  }
  return pTool; 
}
//------------------------------------------
bool dmRegisterToolsEx()
{
  static bool bRegistered = false;
  if(bRegistered) return true;

  dmRegisterTools();
  bool bret = true;
  
  bret &= dmRegisterClass(dmTOOL_RECT_EX   ,dmTOOL_RECT     , dmRectEditEx::ClassInfo()   ,dmCreateRectEx);
  bret &= dmRegisterClass(dmTOOL_CIRCLE_EX ,dmTOOL_ELLIPSE  , dmPtsCircleEdit::ClassInfo(),dmCreateCircleEx);
  bret &= dmRegisterClass(dmTOOL_CIRCLE    ,dmTOOL_ELLIPSE  , dmCircleEdit::ClassInfo()   ,dmCreateCircle  );
  bret &= dmRegisterClass(dmTOOL_POLY_EX   ,dmTOOL_POLYLINES, dmPolyEditEx::ClassInfo()   ,dmCreatePolyEx  );

  bret &= dmRegisterClass(dmTOOL_SECTOR_CID  ,dmTOOL_POLYLINES, dmSector::ClassInfo()   ,dmCreateSector  );
  bret &= dmRegisterClass(dmTOOL_CALIPER_CID ,dmTOOL_LINE     , dmCaliper::ClassInfo()  ,dmCreateCaliper );

  bret &= dmRegisterClass("advtools/caliper;1?type=line" ,dmTOOL_LINE , dmCaliper::ClassInfo(),dmCreateCaliper2 );

  // Create some specifics styles

  dmTextAttributs _DfltText;
  dmPenAttributs  _WhitePenAttrs(eClr_WHITE,eClr_BLACK,0,ePenStyle_solid );

  dmGetServices<dmStyleManager>()->SetStyle(
      dmDrawStyle(_TXT("*caliper") ,_WhitePenAttrs,_DfltText),true);

  bRegistered = true;
  return bret;
}
//------------------------------------------

#define dmUseExtendedTools
#define dmUseToolPrefs
#include "daim_tools.h"
#include "advtools/dmCaliper.h"
#include "advtools/dmDrawUtils.h"
#include "common/dmMetrics.h"

#include "dmStyleManager.h"

#define DEFAULT_TICKLENGTH 16
#define DEFAULT_ARCRADIUS  32

#define GENERATE_EVENT( aType )  \
  dmGENERATE_EVENT(const_cast<dmCaliper*>(this),aType,!this->BubbleCancelled());

#define GENERATE_EVENT_NOBUBBLE( aType, aResult )  \
  dmGENERATE_RESULT_EVENT(const_cast<dmCaliper*>(this),aType,aResult,false);

#define GENERATE_RESULT_EVENT( aType, aResult )  \
  dmGENERATE_RESULT_EVENT(const_cast<dmCaliper*>(this),aType,aResult,!this->BubbleCancelled());

#define DRAW_LAYER dmTk::eDrawBorder|dmTk::eDrawLayer
#define DRAW_ERASE dmTk::eDrawBorder|dmTk::eDrawLayerErase

//---------------------------------------------------------------
#define dmCaliper_SCHEMA_VERSION 1
dmImplementClassInfo( dmCaliper, dmLineEdit, dmCaliper_SCHEMA_VERSION );
//---------------------------------------------------------------
dmCaliper::dmCaliper( dmTool* _parent )
:dmLineEdit(_parent)
,ticklength(DEFAULT_TICKLENGTH)
,showHorz(true)
,csHoldGraphics(false)
{
  Init();
}
//---------------------------------------------------------------
dmCaliper::dmCaliper( const dmCaliper& _toCopy, dmTool* _parent )
: dmLineEdit(_toCopy,_parent) 
, ticklength(_toCopy.ticklength)
, showHorz(_toCopy.showHorz)
, csHoldGraphics(false)
{}
//---------------------------------------------------------------
dmCaliper::~dmCaliper()
{
 
}
//----------------------------------------------------------------
void dmCaliper::SetTickLength( dm_int _length )
{
  O_INVALIDATE(this);
  ticklength = _length;
  mPadding += 3*ticklength;
}
//----------------------------------------------------------------
void dmCaliper::ShowHorizon( bool _show )
{
  O_INVALIDATE(this)
  showHorz  = _show;  
}
//---------------------------------------------------------------
void dmCaliper::Init()
{
  SetDrawStyle(dmGetDrawStyleClass(_TXT("*caliper")));
  mLine.SetLineAttrs(&dmGetDrawStylePen(_TXT("*caliper")));
  mPadding += 3*ticklength;
}
//---------------------------------------------------------------
void dmCaliper::OnDisplay( dmGraphics& gr ) const
{
  gr.DrawLine( mLine, dmTk::eDrawLayer );
  dmTk::DrawTick( gr, mLine[0], mLine[1], ticklength, DRAW_LAYER ); 
  dmTk::DrawTick( gr, mLine[1], mLine[0], ticklength, DRAW_LAYER );
 
  if(showHorz && (mLine[0].y != mLine[1].y))
  {
     dmPoint s(  mLine[1].x ,mLine[0].y );

     gr.DrawLine( mLine[0].x,mLine[0].y, s.x ,s.y,
                  dmTk::eDrawLayer ); 

    dmTk::DrawTick( gr, mLine[0], dmPoint(s.x ,s.y),
                    ticklength, DRAW_LAYER); 

    dm_real startAngle = s.x > mLine[0].x ? 0 : dmMetrics::DegreeToRadian(180.0);
    gr.DrawArc( mLine[0], DEFAULT_ARCRADIUS, DEFAULT_ARCRADIUS ,
                startAngle,startAngle + dmMetrics::Angle(s,mLine[0],mLine[1]),
                DRAW_LAYER);     
  }
}
//-----------------------------------------------------
void dmCaliper::OnMove( int dx, int dy,bool _update ) 
{
  int carretIndex = CarretIndex();

  if(_update) 
  { 
    // Move the Carrets
    mLine.MoveCarret(NULL,carretIndex,dx,dy);  

    // Release the graphics if needed
    if(csHoldGraphics) {
      dmGraphics* gr = GetGraphics();
      csHoldGraphics = false;
      if(gr)
         gr->ReleaseGraphics();

      mLine.Visible(true);   
    }    

    GENERATE_RESULT_EVENT( carretIndex==-1 ? evDrag : evResize , carretIndex )

    O_INVALIDATE(this)
    RecalcObject();
  }
  else 
  {
    dmGraphics* gr = GetGraphics();

    if(gr)
    {
      if(!csHoldGraphics) {     // We do an ObtainGraphics() in order to initialize
        gr->ObtainGraphics();   // the graphics so that following calls to 
        csHoldGraphics = true;  // ObtainGraphics() will not (re)allocate ressources  

        mLine.Visible(false);   // Hide carrets
        Redraw();
      }

      gr->ObtainGraphics(eDrawLayer); // Erase Previous position
      DisplayObject(*gr);
    }

    // Move Carrets
    mLine.MoveCarret(NULL,carretIndex,dx,dy);  
 
    if(gr) {
       DisplayObject(*gr);           //Draw new position
       gr->ReleaseGraphics();
    }

    GENERATE_RESULT_EVENT( carretIndex==-1 ? evDrag : evResize , carretIndex )
  }
}
//---------------------------------------------------------------
dmTool* dmCaliper::Clone( dmTool* _parent ) const
{
  return new dmCaliper(*this,_parent);
}
//---------------------------------------------------------------
bool dmCaliper::OnKeyDown(dm_uint _keyCode,dm_uint _keyQual)
{
  if(_keyCode == 'H') 
  {
    O_INVALIDATE(this)
    showHorz  = !showHorz;
    return true;
  }
  return false;
}
//---------------------------------------------------------------

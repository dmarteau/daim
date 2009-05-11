
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
#include "daim_tools.h"

#include "daim/daim_base.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/processing/CCI_Histograms.h"

#include "dmToolHistogram.h"

#define DRAW_LAYER dmTk::eDrawBorder|dmTk::eDrawLayer
#define DRAW_ERASE dmTk::eDrawBorder|dmTk::eDrawLayerErase

#define NO_THRESHOLD   0
#define MAX_THRESHOLD  1
#define MIN_THRESHOLD  2
#define BAND_THRESHOLD 3

//----------------------------------------------------------------------------
static inline bool _LOCATE( int x, int b ) {
  return (x<=b+2 && x>=b-2); 
}

//----------------------------------------------------------
dmToolHistogram::dmToolHistogram( cci_Object* _Histo, dmTool* _parent )
: dmTool(_parent)
, mThresholding(dm_false)
, mMaxDensityValue(0)
, mMaxThrIndex(-1)
, mMinThrIndex(-1)
, mWhat(NO_THRESHOLD)
, mGraphicLock(0)
, mCursorType(eCursor_undefined)
, mChannel(0)
, mData(NULL)
, mCount(0)
, mReadOnly(dm_false)
, mLogScale(dm_false)
{
  mHisto.QueryInterface(_Histo);
}
//----------------------------------------------------------
dmToolHistogram::~dmToolHistogram()
{
  if(mData!=NULL)
   dmMemory::Free(mData);
}
//----------------------------------------------------------
void dmToolHistogram::SetChannel( dm_uint _Channel )
{
  mChannel = _Channel;
  UpdateObject();
}
//----------------------------------------------------------
dm_bool dmToolHistogram::DoThresholding( dm_bool _Thresholding )
{
  if(mMaxDensityValue<=0)
     _Thresholding = dm_false;

  if(mThresholding != _Thresholding) 
  { 
    mThresholding = _Thresholding;
    if(mThresholding) 
    {
      int count = mCount;
      if(mMinThrIndex <  0 || mMaxThrIndex == -1 || mMaxThrIndex >= count)
      {
        mMinThrIndex = count/3;
        mMaxThrIndex = 2*mMinThrIndex;
      }
    }
    Redraw();
  }
  return mThresholding;
}
//----------------------------------------------------------
void dmToolHistogram::DisplayBandThreshold( dmGraphics& gr, int drawFlags ) const
{
  if(mMaxDensityValue > 0)
  {
    gr.ObtainGraphics(eDrawLayer); 
    gr.SetPenColor(DM_RGB(0,255,0));

    if(mLogScale)  
    {
      dm_real maxval = log(1.0 + mMaxDensityValue);

      for(int x,y,i=mMinThrIndex;i<=mMaxThrIndex;++i) {
        x = i + mRect.Left();
        y = mRect.Top() + static_cast<int>((1.0 - log(1.0+mData[i])/maxval)*mRect.Bottom()+0.5);
        gr.DrawLine( x,y,x,mRect.Bottom(),drawFlags );
      }
    }
    else 
    {
      for(int x,y,i=mMinThrIndex;i<=mMaxThrIndex;++i) {
        x = i + mRect.Left();
        y = mRect.Top() + static_cast<int>((1.0 - mData[i]/mMaxDensityValue)*mRect.Bottom()+0.5);
        gr.DrawLine( x,y,x,mRect.Bottom(),drawFlags );
      }
    }

    gr.SetPenColor(DM_RGB(0,255,255));
    gr.DrawLine( mMinThrIndex+mRect.Left(), mRect.Top(), mMinThrIndex+mRect.Left(), mRect.Bottom(),drawFlags );
    gr.DrawLine( mMaxThrIndex+mRect.Left(), mRect.Top(), mMaxThrIndex+mRect.Left(), mRect.Bottom(),drawFlags );

    gr.ReleaseGraphics();   
  }
}
//----------------------------------------------------------
bool dmToolHistogram::SetRangeIndex( dm_int minindex, dm_int maxindex )
{ 
  if(mMaxDensityValue > 0) {
    // Assig only if both are in range
    int count = mCount;
    if(minindex >= 0 && maxindex < count) 
    {
      mMinThrIndex = minindex;
      mMaxThrIndex = maxindex;

      if(mThresholding) {
        Redraw();
        dmGENERATE_RESULT_EVENT(this,evResize,BAND_THRESHOLD,true);
        return true;
      }
    }
  }
  else 
  {
    mMinThrIndex = minindex;
    mMaxThrIndex = maxindex;
    return true;
  }

  return false;
}
//----------------------------------------------------------
bool dmToolHistogram::SetRangeValues( dm_real minThr , dm_real maxThr,
                                      dm_real aLower , dm_real aUpper  )
{
  if((mMaxDensityValue > 0) && 
     (aLower <  aUpper )    && 
     (minThr <= maxThr)) 
  {
    if(minThr < aLower) minThr = aLower;
    if(maxThr > aUpper) maxThr = aUpper;

    double count = mCount - 1;

    return SetRangeIndex( 
      static_cast<dm_int>( 0.5+ ( (minThr - aLower) * count ) / (aUpper-aLower) ),
      static_cast<dm_int>( 0.5+ ( (maxThr - aLower) * count ) / (aUpper-aLower) )
    );
  }
  return false;
}
//----------------------------------------------------------
void dmToolHistogram::OnDisplay( dmGraphics& gr ) const
{

  if(mMaxDensityValue>0 && mRect.Width()>=mCount) 
  {  
    int count = mCount;
  
    if(mLogScale)  
    {
      dm_real maxval = log(1.0 + mMaxDensityValue);

      for(int x,y,i=0;i<count;++i)
      {
        x = i + mRect.Left();
        y = mRect.Top() + static_cast<int>((1.0f - log(1.0 + mData[i])/maxval)*mRect.Bottom()+0.5);
        gr.DrawLine( x,y,x,mRect.Bottom() );
      }

    }
    else
    {
      for(int x,y,i=0;i<count;++i)
      {
        x = i + mRect.Left();
        y = mRect.Top() + static_cast<int>((1.0f - mData[i]/mMaxDensityValue)*mRect.Bottom()+0.5);
        gr.DrawLine( x,y,x,mRect.Bottom() );
      }
    }

    if(mThresholding) 
      DisplayBandThreshold(gr,DRAW_LAYER);
  }
}
//----------------------------------------------------------
void dmToolHistogram::Recalculate()
{
  dm_uint* data;
  dm_uint  count;

  mMaxDensityValue = 0;
  if(mHisto.Histogram(mChannel,data,count) && count > 0 )
  {
    if(count != mCount)
      mData = static_cast<dm_real*>(dmMemory::Realloc(mData,(mCount=count)*sizeof(dm_real)));

    if(mData!=NULL) 
    {
      dm_real maxval = 0;
      dm_real* it    = mData;
      dm_real* last  = mData + mCount;
      for( dm_uint* src=data;it!=last;++it,++src) {
        if(maxval < (*it = *src) ) 
          maxval = *it;
      }
      mMaxDensityValue = maxval;
    }
  } 

  dmTool* _Parent = Parent();
  if(mCount>0 && _Parent) 
  {
    const dmRect& ParentRect = _Parent->ObjectRect();
    dmPoint p = ParentRect.Center();

    mRect.top_left.x = p.x - mCount/2;
    mRect.top_left.y = ParentRect.Top();

    mRect.bottom_right.x = mRect.top_left.x + mCount - 1;
    mRect.bottom_right.y = ParentRect.Bottom();
  }

  dmTool::Recalculate();
}
//----------------------------------------------------------
void dmToolHistogram::OnUpdateRegion( dmRegion& rgn ) 
{
  rgn.SetRectRoi(mRect);
} 
//----------------------------------------------------------
bool dmToolHistogram::ProcessDefaultEvent( dmEvent& evt )
{

  if(!mReadOnly && evt.IsCapturing())
  {
    evt.mTarget=this;    // Assign the target to this object

    switch(evt.mStructType) 
    {
      case evStruct_MouseEvent : 
      {
        dmMouseEvent& _evt = static_cast<dmMouseEvent&>(evt);

        dmGraphics* _Graphics =  GetGraphics();

        switch(_evt.mEventType)
        {
          case evMouseDown   : 
           if(mThresholding && _Graphics) 
           {
              // We do an ObtainGraphics() in order to initialize
              // the graphics so that following calls to 
              // ObtainGraphics() will not (re)allocate ressources

              _Graphics->ObtainGraphics();
              mGraphicLock = 1;

              int index = _evt.mCursor.x - mRect.Left();

              if(_LOCATE(index,mMinThrIndex)) {
                index = mMinThrIndex; 
                mWhat = MIN_THRESHOLD; 
                mCursorType = eCursor_sizeWE;
              } else
              if(_LOCATE(index,mMaxThrIndex)) { 
                index = mMaxThrIndex;
                mWhat = MAX_THRESHOLD; 
                mCursorType = eCursor_sizeWE;
              } else
              if( index < mMaxThrIndex && index > mMinThrIndex ) { 
                mWhat = BAND_THRESHOLD;
                mCursorType = eCursor_grabbing;
              } else
                mWhat =  NO_THRESHOLD;

              if(mWhat) {
                mCursor = index;
                evt.mCursorType = mCursorType;
              }

              return true; // Event is handled

           } break; 
  
          case evMouseMove   : 
            if((mThresholding && _Graphics)) 
            {
               int index  = _evt.mCursor.x - mRect.Left();

               if(mWhat!=NO_THRESHOLD) 
               {
                 int count  = mCount;

                 if(index <= 0)     index = 0; else
                 if(index >= count) index = count - 1;

                 DisplayBandThreshold(*_Graphics,DRAW_ERASE);

                 switch(mWhat) 
                 {
                    case MIN_THRESHOLD  :
                      mMinThrIndex = index;
                      if(mMinThrIndex > mMaxThrIndex ) {
                        std::swap(mMinThrIndex,mMaxThrIndex);
                        mWhat = MAX_THRESHOLD;
                      }
                      break;
                    case MAX_THRESHOLD  :
                      mMaxThrIndex = index;
                      if(mMinThrIndex > mMaxThrIndex ) {
                        std::swap(mMinThrIndex,mMaxThrIndex);
                        mWhat = MIN_THRESHOLD;
                      }
                      break;
                    case BAND_THRESHOLD : {
                      int d = index - mCursor;          
                      if( (d>0 && (mMaxThrIndex + d)<count) || 
                          (d<0 && (mMinThrIndex + d)>=0   ) )
                      { 
                        mMaxThrIndex+=d;
                        mMinThrIndex+=d; 
                      }
                      mCursor = index;
                    } break;
                } 

                evt.mCursorType = mCursorType;

                DisplayBandThreshold(*_Graphics,DRAW_LAYER);

                // Generate a drag event
                dmGENERATE_RESULT_EVENT(this,evResize,mWhat,true);

                return true; // Event is handled
              } 
            } break;

          case evMouseUp     : 
            if(mThresholding && mGraphicLock) 
            {
              _Graphics->ReleaseGraphics();
              mGraphicLock = 0;

              if(mWhat != NO_THRESHOLD) {
                dmGENERATE_RESULT_EVENT(this,evResize,0,true);
              }
              mWhat = NO_THRESHOLD;
              return true; // Event is handled
            }

            break;
   
          //case evMouseWheel  : break;   
          //case evClick       : break;   
          //case evDblClick    : break;   
          //case evContextMenu : break;   
          default: break;
        }
      } break;

      case evStruct_KeyEvent : 
      {
        dmKeyEvent& _evt = static_cast<dmKeyEvent&>(evt);
        switch(_evt.mEventType)
        {
          case evKeyDown :
             if(mThresholding) 
             {
               dmGraphics* _Graphics = GetGraphics();
               if(_Graphics) {
                 switch(_evt.mKeyCode) 
                 {
                   case DM_VK_RIGHT  : 
                     if(_evt.ShiftKey()) {
                       if(_evt.CtrlKey()) SetRangeIndex(mMinThrIndex+1,mMaxThrIndex);
                       else               SetRangeIndex(mMinThrIndex,mMaxThrIndex+1); 
                     }
                     else                SetRangeIndex(mMinThrIndex+1,mMaxThrIndex+1); 
                     return true;
                   case DM_VK_LEFT   :
                     if(_evt.ShiftKey()) {
                       if(_evt.CtrlKey()) SetRangeIndex(mMinThrIndex,mMaxThrIndex-1); 
                       else               SetRangeIndex(mMinThrIndex-1,mMaxThrIndex); 
                     }
                     else                SetRangeIndex(mMinThrIndex-1,mMaxThrIndex-1); 
                     return true;
                 }
               }
             }
             break; 
          //case evKeyUp       : break; 
          //case evKeyPress    : break; 
          default: break;
        }
      } break;

      default: break;
    } 
  }
  return false; // Event is not handled
  
}
//----------------------------------------------------------
dm_int dmToolHistogram::MinDensityValue(int minIndex,int maxIndex) const 
{ 
  if(mMaxDensityValue > 0)
  {
    int i=minIndex;
    while(i<maxIndex && mData[i]==0 ) { ++i; } 
    return i; 
  }
  return 0;
}
//----------------------------------------------------------  
dm_int dmToolHistogram::MaxDensityValue(int minIndex,int maxIndex) const 
{ 
  if(mMaxDensityValue > 0)
  {
    int i=maxIndex; 
    while(i>minIndex && mData[i]==0 ) { --i; } 
    return i; 
  }
  return 0;
} 
//----------------------------------------------------------

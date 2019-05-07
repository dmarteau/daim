#ifndef dmToolHistogram_h
#define dmToolHistogram_h

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


//--------------------------------------------------------
// File         : dmToolHistogram.h
// Date         : 5/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

class dmToolHistogram : public dmTool
{
  private:
    dmRect         mRect;
    dmIHistograms  mHisto;

    dm_bool        mThresholding;
    dm_real        mMaxDensityValue;
    dm_int         mMaxThrIndex;
    dm_int         mMinThrIndex;
    
    dm_uint        mWhat;   
    dm_int         mGraphicLock;

    ECursorType    mCursorType;

    dm_int         mCursor;

  public:
    dm_uint        mChannel;
    dm_real*       mData;
    dm_uint        mCount;

    dm_bool        mReadOnly;
    dm_bool        mLogScale;


  public:
    dmToolHistogram( cci_Object* _Histo, dmTool* _parent = NULL );
   ~dmToolHistogram();

    virtual void OnDisplay( dmGraphics& ) const;
    virtual void Recalculate();

    virtual bool ProcessDefaultEvent( dmEvent& );

    void SetChannel( dm_uint );

    dm_bool DoThresholding( dm_bool );
    dm_bool Thresholding() const { return mThresholding; }

    dm_int GetIndex( dm_int x ) const {
       int rv = x - mRect.Left();
       return ((rv >= 0 && rv < static_cast<int>(mCount))?rv:-1);
    }
 
    bool IsValidData() const { return mMaxDensityValue > 0; }

    bool SetRangeValues( dm_real thrMin, dm_real thrMax,
                         dm_real lower , dm_real upper ); 

    double GetMinIndex () const { return mMinThrIndex; }
    double GetMaxIndex () const { return mMaxThrIndex; }

    dm_int MinDensityValue(int minIndex,int maxIndex) const;
    dm_int MaxDensityValue(int minIndex,int maxIndex) const;

  protected:   
    void DisplayBandThreshold( dmGraphics&, int ) const;
    bool SetRangeIndex( dm_int minindex, dm_int maxindex  );

    virtual void OnUpdateRegion( dmRegion& );


};


#endif // dmToolHistogram_h

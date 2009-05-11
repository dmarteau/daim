#ifndef daim_info_mngr_h
#define daim_info_mngr_h

/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

#define dmUseToolManager
#include "daim_info.h"
#include "daim_cmdtarget.h"
#include "daim_tools.h"

class daim_tools;
class daim_manager;
class daim_layers;

class daim_info_manager : public daim_CmdTarget ,
                          public daim_document  ,
                          public dmToolManager
{

protected:
  daim_tools*   mTools;
  daim_layers*  mLayers;

  bool       mbShowMask;
  bool       mbShowLayers;
  bool       mbMaskVisibility;
  
  EColorType mMaskColor;
  EColorType mExtMaskColor;

  bool   mbEqualizeLUT;
  bool   mbCustomLUT;
  dmLUT  mLut;

  dmLink<dmImage>       mTmpImage;
  
  dmLink<dmNativeImage> mDisplayableImage;
  dmImageData           mDisplayableImageInfo;
  
protected:
  //---------------------
  // dmToolManager members
  //---------------------
  virtual void Recalculate();
  virtual void OnDisplay( dmGraphics& ) const;

  void Create_Tools();
  void Destroy_Tools();
   
  void UpdateLayers( dmNativeImage* );

public:
   //------------------------------------------------
   // daim_document members
   //------------------------------------------------
   virtual void OnRefresh();
   virtual void OnUpdateObject();
   virtual void UpdateMask( bool bUpdate );

   virtual dmImage* GetImage() const;

public:
  daim_info_manager( daim_info* );
 ~daim_info_manager();

  //------------------------------------------------
  // Init special managers
  //------------------------------------------------
  void InsertManager( daim_manager* );
  
  void InitROI();
  void InitMask();
  void InitLayers();
  
  daim_manager* SelectManager( const dmString::E* _name, dm_uint createflags ); 

  dmLink<dmNativeImage> GetDisplayableImage( dmImage*, EPixelFormat _Format = dmPixelFormatUndefined );

  //--------------------------
  // Layers
  //--------------------------

  daim_layers* GetLayers();
  void         ShowLayers( bool bShow );
  void         KillLayers( bool bUpdate );
  //--------------------------
  // Masks
  //--------------------------

  void SetMaskColor   ( EColorType, bool bRefresh=false);
  void SetExtMaskColor( EColorType, bool bRefresh=false);

  void ShowMask( bool );

  EColorType GetMaskColor()     const { return mMaskColor;    }
  EColorType GetExtMaskColor()  const { return mExtMaskColor; }

  void SetMaskVisibility( bool bVisibility );

  //--------------------------
  // LUT
  //--------------------------
        dmLUT* GetLUT()       { return &mLut; }
  const dmLUT* GetLUT() const { return &mLut; }

  void  SetCustomLUT( dmLUT* );
  void  EqualizeLUT ( bool   );
  bool  RestoreLUT( dmImage* );
  bool  ApplyLUT();

  bool IsEqualizedLUT() const { return mbEqualizeLUT; }
  bool IsCustomLUT()    const { return mbCustomLUT;   }

  bool DrawImage();

  virtual void Refresh();
  virtual void UpdateObject();
  //------------------------------------------------
  // Commands
  //------------------------------------------------
  virtual bool OnCommandUpdate ( daim_CmdUI*  );
  virtual bool OnCommand       ( dm_uint nCode );

  __command void OnEscape();
  __command void OnUpdateImage(daim_CmdUI*);

  __command void OnEditUndo();
  __command void OnUpdateEditUndo(daim_CmdUI*);

  __command void OnRevertToSaved();
  __command void OnUpdateRevertToSaved(daim_CmdUI*);

  __command void OnKillMask();
  __command void OnInvertMask();

  __command void OnUpdateValidMask(daim_CmdUI*);

  __command void OnMergeTools();
  __command void OnUpdateMergeTools(daim_CmdUI*);

  __command void OnConvertToRGB();
  __command void OnUpdateConvertToRGB(daim_CmdUI*);

  __command void OnConvertTo8bits();
  __command void OnUpdateConvertTo8bits(daim_CmdUI*);

  __command void OnRotateLeft();
  __command void OnRotateRight();
  __command void OnFlipVer();
  __command void OnFlipHor();

  DECLARE_COMMAND_MAP()

};

#endif // daim_info_mngr_h


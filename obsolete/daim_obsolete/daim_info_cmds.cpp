
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


#include "daim_info_mngr.h"
#include "common/dmUserLib.h"

#define THIS_INFO() mInfo
//------------------------------------------------------
BEGIN_COMMAND_MAP(daim_info_manager, daim_CmdTarget)

  REG_COMMAND_UPDATE(daim_info_manager, EditUndo )
  REG_COMMAND_UPDATE(daim_info_manager, RevertToSaved )

  REG_COMMAND_UPDATE(daim_info_manager, ConvertToRGB )
  REG_COMMAND_UPDATE(daim_info_manager, ConvertTo8bits )

  REG_COMMAND(daim_info_manager, RotateLeft , OnUpdateImage ) 
  REG_COMMAND(daim_info_manager, RotateRight, OnUpdateImage ) 
  REG_COMMAND(daim_info_manager, FlipVer    , OnUpdateImage ) 
  REG_COMMAND(daim_info_manager, FlipHor    , OnUpdateImage ) 

  // Mask operations 
  REG_COMMAND(daim_info_manager, KillMask  , OnUpdateImage )
  REG_COMMAND(daim_info_manager, InvertMask, OnUpdateValidMask )

END_COMMAND_MAP()
//-----------------------------------------------------------------------
void daim_info_manager::OnUpdateImage( daim_CmdUI* pCmdUI )
{
  pCmdUI->Enable(THIS_INFO()->IsValidView());
}
//----------------------------------------------------------------------------
// Image conversions commands
//----------------------------------------------------------------------------
void daim_info_manager::OnConvertTo8bits()
{
  dmLink<dmImage> _Image = THIS_INFO()->ConvertTo8Bits(); 

  THIS_INFO()->SetImage(_Image);
  THIS_INFO()->SetModified(true);
}
//------------------------------------------------------
void daim_info_manager::OnUpdateConvertTo8bits( daim_CmdUI* pCmdUI)
{
  pCmdUI->Enable(  
    THIS_INFO()->IsValidView() && 
    THIS_INFO()->GetImage()->PixelFormat()!=dmPixelFormat8bppIndexed
  );
}
//------------------------------------------------------
void daim_info_manager::OnConvertToRGB()
{
  dmLink<dmImage> _Image = THIS_INFO()->ConvertToRGB();
	 
  THIS_INFO()->SetImage(_Image);
  THIS_INFO()->SetModified(true);
}
//------------------------------------------------------
void daim_info_manager::OnUpdateConvertToRGB( daim_CmdUI* pCmdUI)
{
  pCmdUI->Enable( 
     THIS_INFO()->IsValidView() && 
    !THIS_INFO()->IsRGBImage() 
  );
}
//----------------------------------------------------------------------------
// Image transformations commands
//----------------------------------------------------------------------------
void daim_info_manager::OnRotateLeft() 
{
  dmLink<dmImage> _Image = THIS_INFO()->RotateImage(dmTk::RotateLeft );
  THIS_INFO()->SetImage(_Image);
  THIS_INFO()->SetModified(true); 
}
//----------------------------------------------------------------------------
void daim_info_manager::OnRotateRight() 
{
  dmLink<dmImage> _Image = THIS_INFO()->RotateImage(dmTk::RotateRight);
  THIS_INFO()->SetImage(_Image);
  THIS_INFO()->SetModified(true); 
}
//----------------------------------------------------------------------------
void daim_info_manager::OnFlipVer() 
{
  dmLink<dmImage> _Image = THIS_INFO()->FlipImage(dmTk::FlipVer);
  THIS_INFO()->SetImage(_Image);
  THIS_INFO()->SetModified(true); 
}
//----------------------------------------------------------------------------
void daim_info_manager::OnFlipHor()
{
  dmLink<dmImage> _Image = THIS_INFO()->FlipImage(dmTk::FlipHor);
  THIS_INFO()->SetImage(_Image);
  THIS_INFO()->SetModified(true); 
}
//----------------------------------------------------------------------------
// Image edit commands
//-------------------------------------------------------
void daim_info_manager::OnEditUndo() 
{
  THIS_INFO()->RestoreFromBuffer();
}
//-------------------------------------------------------
void daim_info_manager::OnUpdateEditUndo(daim_CmdUI* pCmdUI) 
{
  pCmdUI->Enable(!THIS_INFO()->GetBuffer()->IsEmpty());
}
//------------------------------------------------------
void daim_info_manager::OnRevertToSaved()
{ 
  THIS_INFO()->RevertToSaved();
}
//------------------------------------------------------
void daim_info_manager::OnUpdateRevertToSaved(daim_CmdUI* pCmdUI)
{
  pCmdUI->Enable(THIS_INFO()->FromDisk());
}
//------------------------------------------------------
// Masks operations
//------------------------------------------------------
#include "daim_kernel/operators/dmOperators.h"
//-------------------------------------------------------
void daim_info_manager::OnUpdateValidMask(daim_CmdUI* pCmdUI) 
{
  pCmdUI->Enable(!THIS_INFO()->GetMask().IsEmptyRoi());
}
//-------------------------------------------------------
void daim_info_manager::OnKillMask()  
{ 
  //XXX: We disable also the layers;
  KillLayers(THIS_INFO()->GetMask().IsEmptyRoi());
  THIS_INFO()->KillMask();
}
//-------------------------------------------------------
void daim_info_manager::OnInvertMask() 
{
  THIS_INFO()->SetMask(
    THIS_INFO()->GetImageRect(),dmTk::Math::XorPixels);
}
//-------------------------------------------------------

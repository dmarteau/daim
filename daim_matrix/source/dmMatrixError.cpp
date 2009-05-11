
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

//---------------------------------------------------------------------------
#include "dmMatrixExcept.h"
#include "dmMatrixBase.h"

BEGIN_MATRIX()
//---------------------------------------------------------------------------
void MatrixErrorException(const char* _msg,const char* _file,int _line)
{
#ifndef dmNoLibUtilities
  dmCString msg = dmCString::FormatString(dmCString::npos,
                  "%s . throw in %s (line %d)",_msg,_file,_line);

  dmTHROW( EMatrixError(msg) );

#else
  fprintf(stderr,"%s . throw in %s (line %d)" ,_msg,_file,line);
  fflush(stderr);
#endif
}

//---------------------------------------------------------------------------
void MatrixDump( const char* text, dmMatrix& _Matrix )
{
  dmMatrixBase* _MBase = _Matrix.Evaluate();
  dmMatrixRow   _Row(_MBase);
  
#ifndef dmNoLibUtilities
  dmLOG("%s[%d] = {\n",text,_MBase->NStore());
  
  int N = _MBase->NRows();
  for(int i=1; i<=N; ++i) {
    for(int j=1;j<=_Row.Length();++j)
        dmLOG("%.6lf,\t",_Row[j]);
    _Row.Next();
    dmLOG("\n");
  }
  dmLOG("};\n");
#else
  printf(("%s[%d] = {\n",text,_MBase->NStore());
  
  int N = _MBase->NRows();
  for(int i=1; i<=N; ++i) {
    for(int j=1;j<=_Row.Length();++j)
        printf("%.6lf,\t",_Row[j]);
    _Row.Next();
    printf("\n");
  }
  printf("};\n");
#endif
}
//---------------------------------------------------------------------------
END_MATRIX()

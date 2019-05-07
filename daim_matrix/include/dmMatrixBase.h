#ifndef dmMatrixBase_h
#define dmMatrixBase_h

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
// File         : dmMatrixBase.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "dmMatrix.h"
#include "dmRowCol.h"

/**
 * @startuml
 * 
 * abstract class dmMatrix
 * 
 * dmMatrix <|-- dmMatrixBase
 * dmMatrixBase <|-- dmMatrixRowCol
 * dmMatrixBase <|-- dmRectMatrix
 * dmMatrixBase <|-- dmSymMatrix
 * dmMatrixBase <|-- dmColumnVector
 * dmMatrixBase <|-- dmRowVector
 * dmMatrixRowCol <|-- dmMatrixCol 
 * dmMatrixRowCol <|-- dmMatrixRow 
 * @enduml
 */

BEGIN_MATRIX()
//----------------------------------------------------------------------------
// Base Matrix
//----------------------------------------------------------------------------
class dmMatrixBase : public dmMatrix
{
  friend class dmMatrix;
  friend class dmMatrixRow;
  friend class dmMatrixCol;

  public:
    enum MatrixStatus { NoStatus, Temporary, ToReuse } ;

  protected:
    MatrixStatus Status;   // Used un evaluations
    int       NbCol;       // Columns
    int       NbRow;       // Lines
    int       Storage;     // Data size
    dm_matrix_t* Store;

    dmMatrixBase& operator=(const dmMatrix& );

  protected:
    dmMatrixBase();
    bool Construct( int _NbRow=0, int _NbCol=0, int _Store=0);

  public:
    virtual ~dmMatrixBase();

    void GetMatrix(const dmMatrix&);
    void Free();
    dmMatrixBase& Set(dm_matrix_t);

    dmMatrixBase& operator>>(dm_matrix_t *);
    dmMatrixBase& operator<<(dm_matrix_t *);

    //--- Acces to data -------------

    dm_matrix_t* GetData()  const { return Store; }    // pointer to first element

    dm_matrix_t* BlkStart() const { return Store; }         // pointer to first element
    dm_matrix_t* BlkEnd()   const { return Store+Storage; } // pointer to one past last element

    dmMatrixBase* Evaluate(dmMatrixType=dmMatrixType::UnSp);

    virtual dmMatrixType  Type() const = 0;
  	virtual dm_matrix_t& operator()( int row, int col ) const = 0;
  	virtual dmMatrixBase* Duplicate() = 0;
  	virtual void Resize(int,int) = 0;

  	int NCols()  const { return NbCol;   }
  	int NRows()  const { return NbRow;   }
  	int NStore() const { return Storage; }

  	// Used in evaluation
  	int  Search(const dmMatrixBase* ) const;
  	void SetStatus(MatrixStatus _Status) { Status = _Status; }
  	bool IsEval()  const { return Status==Temporary; }
  	bool Reuse()   const { return Status==ToReuse;   }

  	void GetCol ( dmMatrixRowCol& _Col, int _NCol ) const;
  	void GetRow ( dmMatrixRowCol& _Row, int _NRow ) const;

  	void GetCol ( dmMatrixRowCol& _Col ) const { GetCol(_Col,_Col.RCRowCol); }
  	void GetRow ( dmMatrixRowCol& _Row ) const { GetRow(_Row,_Row.RCRowCol); }
  	void NextCol( dmMatrixRowCol& _Col ) const { ++_Col.RCRowCol; ++_Col.Ptr; }
  	void NextRow( dmMatrixRowCol& _Row ) const { ++_Row.RCRowCol; _Row.Ptr += NbCol; }
};
//-----------------------------------------------------------
// Acces au Lignes
//-----------------------------------------------------------
class dmMatrixRow : public dmMatrixRowCol
{
  private:
  	const dmMatrixBase* Matrix;
  public:
  	dmMatrixRow(const dmMatrixBase* _Matrix,int _Row=1)
  	: dmMatrixRowCol(_Row), Matrix(_Matrix) { Matrix->GetRow(*this); }
   ~dmMatrixRow() {}

    void Next() { Matrix->NextRow(*this); }
};
//-----------------------------------------------------------
// Acces au Colonnes
//-----------------------------------------------------------
class dmMatrixCol : public dmMatrixRowCol
{
  private:
  	const dmMatrixBase* Matrix;
  public:
  	dmMatrixCol(const dmMatrixBase* _Matrix,int _Col=1)
  	: dmMatrixRowCol(_Col), Matrix(_Matrix) { Matrix->GetCol(*this); }
  	~dmMatrixCol() {}

    void Next() { Matrix->NextCol(*this); }
};
//---------------------------------------------------------------------------
// Standart Rectangular Matrix
//---------------------------------------------------------------------------
class dmRectMatrix : public dmMatrixBase
{
  friend class dmMatrix;
  public:
    dmRectMatrix();
    dmRectMatrix( int _NbRow, int _NbCol );
    dmRectMatrix( const dmMatrix& _Matrix);
   ~dmRectMatrix() {}
    //-----------------------------------
    dmRectMatrix& operator=(const dmMatrix&);
    void Resize(int _Row,int _Col);
    //-----------------------------------
    dmMatrixType Type()  const  { return dmMatrixType::Rect; }
    dm_matrix_t&  operator()( int row, int col ) const;
    //
    dmMatrixBase* Duplicate();
};
//---------------------------------------------------------------------------
// Matrice Symmetrique
//-------------------------------------------------------------------------
class dmSymMatrix : public dmRectMatrix
{
  friend class dmMatrix;
  public:
    dmSymMatrix();
	  dmSymMatrix( int _Size );
	  dmSymMatrix( const dmMatrix& _Matrix);
	 ~dmSymMatrix() {}
	  //-----------------------------------
	  dmSymMatrix& operator=(const dmMatrix& _Matrix);
	  void Resize(int _Row,int _Col);
	  void Resize(int _Size) { Resize(_Size,_Size); }
	  //-----------------------------------
	  dmMatrixType Type()  const  { return dmMatrixType::Sym; }
	  //------------------------------------
	  dmMatrixBase* Duplicate();
};
//---------------------------------------------------------------------------
// matrice colonne
//---------------------------------------------------------------------------
class dmColumnVector : public dmMatrixBase
{
  friend class dmMatrix;
  public:
    dmColumnVector() {}
	  dmColumnVector( int _Size );
	  dmColumnVector( const dmMatrix& );
	 ~dmColumnVector() {}
	  //----------------------------------------
	  dmColumnVector& operator=(const dmMatrix&);
	  void Resize(int _Row,int _Col);
	  void Resize(int _Row) { Resize(_Row,1); }
	  //-----------------------------------
	  dmMatrixType Type()  const  { return dmMatrixType::ColV; }
	  //----------------------------------------
	  dm_matrix_t& operator()( int row, int col ) const;
	  dm_matrix_t& operator()( int row )          const { return (*this)( row, 1 ); }
	  //--------------------------------------------------------------
	  dmMatrixBase* Duplicate();
};
//------------------------------------------------------------------
// matrice ligne
//------------------------------------------------------------------
class dmRowVector : public dmMatrixBase
{
  friend class dmMatrix;
  public:
    dmRowVector() {}
    dmRowVector( int _Size );
    dmRowVector(const dmMatrix& _Matrix);
   ~dmRowVector() {}
    //----------------------------------------
    dmRowVector& operator=(const dmMatrix&);
    void Resize(int _Row,int _Col);
    void Resize(int _Col) { Resize(1,_Col); }
    //-----------------------------------
    dmMatrixType Type()  const  { return dmMatrixType::RowV; }
    //---------------------------------------------------------------
    dm_matrix_t& operator()( int row, int col ) const;
    dm_matrix_t& operator()( int col )          const { return (*this)( 1, col ); }
    //---------------------------------------------------------------
    dmMatrixBase* Duplicate();
};
//------------------------------------------------------------------
END_MATRIX()

#endif // dmMatrixBase_h


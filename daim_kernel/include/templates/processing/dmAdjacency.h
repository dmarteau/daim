#ifndef dmAdjacency_h
#define dmAdjacency_h

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
// File         : dmAdjacency.h
// Date         : 7/2004
// Author       : David Marteau
//---------------------------------------------------------------------------

namespace daim {

struct adjacency
{
  struct _Link;

  typedef struct _Node {
     _Node* next;  // Noeud suivant   dans la table des noeuds
     _Node* prev;  // Noeud précedent dans la table des noeuds
     _Link* link;  // premier lien vers successeur(s)
     int    index; // index de la partition
  } *XNODE;

  typedef struct _Link {
     _Link* next;  // Lien suivant   dans la table des liens
     _Link* prev;  // Lien précedent dans la table des liens
     _Link* pred;  // addresse du lien issue du predecesseur (constant)
     _Node* succ;  // addresse du noeud successeur
      int   stat;  // status
      int   index; // index du lien   (constant)
      int   key;   // clef de hachage
  } *XLINK;
 
  typedef basic_partition              partition_type;
  typedef basic_partition::index_table index_table;
  typedef hashmap<size_t,XLINK>        hash_type;
  typedef std::pair<int,int>           node_pair;

  hash_type M_Hash;

  partition_type        _partition;
  index_table           _index_table;
  std::vector<_Node>    _N;  // table des noeuds
  std::vector<_Link>    _L;  // table des successeur 

  XNODE node_begin() { return  _N.back().next; }
  XNODE node_end()   { return &_N.back();      }
  
  XLINK link_begin() { return  _L.back().next; }
  XLINK link_end()   { return &_L.back();      }

  int _NodeIndex( int part ) { return _index_table[part]; }
  int _NodeIndex( XNODE  n ) { return n - _N.begin();     }

  int   _PartIndex  ( int part ) { return _index_table[_partition[part]]; }
  XNODE _GetNode    ( int part ) { return &_N[_NodeIndex(part)]; }
  XNODE _GetNodePart( int part ) { return &_N[_PartIndex(part)]; }

  static __LinkKey( int n1,int n2, size_t sz ) {
    return (n1>n2 ? n2+n1*sz : n1+n2*sz);
  }

  size_t _LinkKey( int n1, int n2 )     { return __LinkKey(n1,n2,_N.size()); }
  size_t _LinkKey( XNODE n1, XNODE n2 ) { return  _LinkKey(_NodeIndex(n1),_NodeIndex(n2)); }
  size_t _LinkKey( XLINK x )            { return  _LinkKey(x->succ,x->pred->succ); }

  // fusionne deux partition et retourne une pair d'indices de noeuds
  node_pair  _MergeNodes( XNODE, XNODE );
  node_pair  _MergeNodes( XLINK x )              { return _MergeNodes(x->pred->succ,x->succ); }
  void       _Eraselinks( XNODE );

  static XLINK _Remove ( XLINK x );
  static XLINK _Remove2( XLINK x ) { return (_Remove(x->pred),_Remove(x)); }

  size_t _LinkTableSize() const { return (_L.size()-1)/2; }
  size_t _NodeTableSize() const { return _N.size()-1;     }

  void _Build(const basic_partition&,const map_type&, connectivity _connect, 
              const dmRegion* = NULL );

  ~adjacency();
};
//-----------------------------------------------------------------------------
inline adjacency::XLINK adjacency::_Remove( adjacency::XLINK x ) 
{
    XNODE npred = x->pred->succ;
    if( npred->link == x) npred->link = x->next; // update node    
    (*(x->prev)).next = x->next;                 // remove the link
    (*(x->next)).prev = x->prev;                 //
    x->stat = 0;                                 // Invalidate the link
    //xlnk->prev = xlnk->next = 0; 
    return x->next;
}
//-----------------------------------------------------------------------------
inline void adjacency::_Eraselinks( adjacency::XNODE n )
{
  XLINK end   = n->next->link;
  XLINK start = n->link;
  for(XLINK x=start;x!=end;x=x->next) 
     M_Hash.erase( x->key );    
}
//-----------------------------------------------------------------------------

}; //namespace daim


#endif // dmAdjacency_h


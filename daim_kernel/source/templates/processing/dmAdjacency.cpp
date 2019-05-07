
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

#define dmUseKernelTemplates
#include "daim_kernel.h"

#include "templates/processing/dmPartition.h"
#include "templates/processing/dmAdjacency.h"

#define BAD_NODE -1
#define BAD_INDX -1

namespace daim {
//---------------------------------------------------------
// Creation du graphe d'adjacence 
//---------------------------------------------------------
typedef std::pair<int,int> _pair_type;
//---------------------------------------------------------
struct _pair_less : public std::binary_function<_pair_type,_pair_type,bool>
{
  bool operator()( const _pair_type& x1, const _pair_type& x2 ) {
    return (x1.first==x2.first ? x1.second < x2.second : x1.first < x2.first );
  }
};
//---------------------------------------------------------
struct __ADJ
{
  const basic_partition&    _part;
  std::vector<_pair_type>&  _heap;
  connectivity              _conn;
  
  __ADJ( std::vector<_pair_type>& h,const basic_partition& p, connectivity _connect ) 
  : _part(p),_heap(h),_conn(_connect) {}
 
  void insert( int i, int j ) {
    if(i!=j) {
      _heap.push_back( _pair_type(i,j) );
      _heap.push_back( _pair_type(j,i) );
    }
  }

  void operator()( map_type::const_line_type _in , long x1, long x2 )
  {
    const map_type::value_type* p1 = _in[1]; // lower line
    const map_type::value_type* p0 = _in[0];

    int u;
    for(int x=x1; x<=x2;++x) {
      u = _part[p0[x]];
      insert(u,_part[p0[x+1]]);
      insert(u,_part[p1[x]]  );
      if(_conn==connect8) 
        insert(u,_part[p1[x+1]]); 
    }
  }  
};
//-----------------------------------------------------------------------------
adjacency::~adjacency()
{
}
//-----------------------------------------------------------------------------
void adjacency::_Build(const basic_partition& _p, const map_type& _m, 
                       connectivity _connect, const dmRegion* pRgn ) 
{
  typedef hash_type::value_type hash_value;

  _partition = _p;
  _N.clear();
  _L.clear();
  _index_table.clear();
  M_Hash.clear();

  _partition.update();

  int nPartitions = _partition.get_index_table(_index_table);

  if(nPartitions>0) {

    dmRegion fg;
    if(pRgn) fg = *pRgn;

    dmRect r = _m.rect();
    r.Resize(0,0,-1,-1);

    std::vector<_pair_type> _heap; 

    if(pRgn) fg.ClipToRect(r);

    // compute pairs
    _heap.reserve( 4 * nPartitions );

    if(!pRgn) {
      apply(r,_m,__ADJ(_heap,_partition,_connect));
    } else {
      // Get connections in foreground;
      apply(fg,_m,__ADJ(_heap,_partition,_connect  ));
      // Get connections in background;     
      _connect = (_connect==connect8 ? connect4 : connect8);
      fg.XorCoordinates(r);
      apply(fg,_m,__ADJ(_heap,_partition,_connect));  
    }
    
    std::vector<_pair_type>::iterator Li,Xi;

    std::sort( _heap.begin(), _heap.end(), _pair_less() );   // sort pairs
    Li = std::unique( _heap.begin(), _heap.end() );          // make unique pairs sequence
    Xi = _heap.begin();

    _N.resize( nPartitions + 1 );
    _L.resize( Li - Xi + 1 );

    M_Hash.get_allocator().set_block_size(_L.size());
    M_Hash.resize( 4 * _L.size() );

    // build nodes table
    XNODE lastn = node_end();  
    lastn->link = link_end();
    lastn->next = _N.begin();
    lastn->next->prev = lastn;
    for(XNODE n     = _N.begin();n!=lastn;++n) {
      n->next       = n+1;
      n->next->prev = n;
      n->link       = lastn->link;
      n->index      = BAD_NODE;
    }
    //--------------------------
    // Buill link table
    //--------------------------
    XLINK lastx = link_end();
    lastx->next = _L.begin();
    lastx->next->prev = lastx;

    hash_type::iterator _Last = M_Hash.end();
    
    int index=0,n1,n2,currnode = -1;
    if(_L.begin()==lastx) 
    {      
      // Only one link => 1 object only with with no background
      dmASSERT( nPartitions == 2 );
      _N[1].index = _index_table[1];             
    } else {
      for(XLINK y,x = _L.begin();x!=lastx;++Xi,++x) 
      { 
        n1 = _index_table[Xi->first];
        n2 = _index_table[Xi->second];

        x->next       = x+1;
        x->next->prev = x;
        x->succ       = &_N[n2];
        x->key        = _LinkKey(n1,n2);
        x->index      = BAD_INDX;
        x->stat       = 0;
        x->pred       = NULL;
   
        if(currnode!=n1) {
          while( currnode != n1 ) {
            dmASSERT( currnode < nPartitions );
            _N[++currnode].link = x;
          }
          _N[n1].index = Xi->first;
        }
  
        hash_type::iterator _It = M_Hash.find( x->key );
        if(_It!=_Last) {
          y = _It->second;
          y->pred  = x;
          x->pred  = y;
          x->index = y->index;        
        } else {
          x->index = ++index;
          M_Hash.insert( hash_value(x->key,x) );
        }
      }
    }
    //--------------------------
    // Remove unlinked node if set to invalid
    //--------------------------
    lastn = node_end();
    for(n = _N.begin();n!=lastn;++n) {
      if(n->link==n->next->link) {
        if( n->index == BAD_NODE ) {
          n->prev->next = n->next;
          n->next->prev = n->prev;
        }
      } else dmASSERT( n->index != BAD_NODE );
    }
    dmASSERT(2*M_Hash.size()==_L.size()-1);
  }
}
//-----------------------------------------------------------------------------
adjacency::node_pair 
adjacency::_MergeNodes( XNODE p1, XNODE p2 )
{
  typedef hash_type::value_type hash_value;

  _partition.resolve(p1->index,p2->index);
  _Eraselinks(p1);
  _Eraselinks(p2);

  XLINK end   = p2->next->link;
  XLINK start = p2->link;
  for(XLINK pos=start;pos!=end;pos=pos->next)     // Table des predecesseurs
     pos->pred->succ = p1;
  //-------------------------
  // Reorganise les liens
  //-------------------------
  pos = p1->next->link;
  if(pos!=start) {
    XLINK end_prev = end->prev;
    (end->prev     = start->prev)->next = end;
    (start->prev   = pos->prev)->next   = start;
    (pos->prev     = end_prev)->next    = pos;
  } 

  //-------------------------
  // Reorganise les noeuds : on enlève n2
  //-------------------------
  p2->prev->next = p2->next;
  p2->next->prev = p2->prev;
  //p2->next       = p2->prev = 0; 
  
  end = p1->next->link; 
  hash_type::iterator _Last = M_Hash.end();
  for(pos=p1->link;pos!=end;) {
 
    assert( pos != link_end() );

    if(pos->succ!=p1) {
      pos->key = _LinkKey(pos->succ,p1);       // recalcule la clef
      if(M_Hash.find( pos->key ) == _Last) {
        M_Hash.insert(hash_value(pos->key,pos));
        pos->pred->stat = ++pos->stat;
        pos->pred->key  = pos->key;
        pos = pos->next;
        continue;
      }
    }
    pos = _Remove2( pos );
  }

  dmDEBUG_ASSERT( node_end()->link == link_end() );
  return node_pair( _NodeIndex(p1), _NodeIndex(p2) );
};
//-----------------------------------------------------------------------------
} //namespace daim

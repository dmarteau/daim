#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmDrawUtils.h"
#include "common/dmMetrics.h"

namespace dmTk
{

void DrawTick( dmGraphics& gr ,
               const dm_point& _uStart, 
               const dm_point& _uEnd, 
               dm_int _Length, int flags )
{
  dm_double d = dmMetrics::Euclidian_Distance(_uStart,_uEnd);
  if(d>0) 
  {
    dm_int dx = static_cast<dm_int>(daim::round((_Length * (_uStart.x - _uEnd.x))/d));
    dm_int dy = static_cast<dm_int>(daim::round((_Length * (_uStart.y - _uEnd.y))/d));
    gr.DrawLine(_uEnd.x - dy,_uEnd.y + dx,_uEnd.x,_uEnd.y,flags);
    gr.DrawLine(_uEnd.x + dy,_uEnd.y - dx,_uEnd.x,_uEnd.y,flags);
  }
}

} // namespace dmTk

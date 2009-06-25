//To be included in cciMeasures.h

bool ComputeDensity( const dmIImage<dmPixelFormat32bppFloat>& , dm_uint _Order, dm_real* _Results, const dmPoint& );
bool ComputeMeanDensity( const dmIImage<dmPixelFormat32bppFloat>& , dm_real* _Results, const dmPoint& );
bool ComputeMinDensity( const dmIImage<dmPixelFormat32bppFloat>&  , dm_real* _Results, const dmPoint& );
bool ComputeMaxDensity( const dmIImage<dmPixelFormat32bppFloat>&  , dm_real* _Results, const dmPoint& );

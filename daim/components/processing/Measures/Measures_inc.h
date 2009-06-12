
//To be included in cciMeasures.h

// Measures
void M_UpdateCount();
void M_UpdateCentroids(); 
void M_UpdateEuler1( XNODE root  ); 
void M_UpdateEuler(); 
void M_UpdateBoundary(); 
bool M_ComputeArea(  dm_real* _Results );
bool M_ComputeBoundary( dm_real* _Results );
bool M_ComputeBorder( dm_real* _Results );
bool M_ComputeMomentum( dm_real* _Results, int p, int q);
bool M_ComputeLabels( dm_real* _Results );
void M_UpdateDir(); 
void M_ComputeRectProps1( dm_real _xcal,dm_real _ycal,const dmPoly& p, dm_real theta, dm_real& l1, dm_real& l2 );
void M_UpdateRectProps( cciMeasurements* _This ); 
bool M_ComputeDir( dm_real* _Results );
bool M_ComputeRectProps( dm_real* _Results, int l );
bool M_ComputeEuler( dm_real* _Results  ); 
bool M_ComputeDepth( dm_real* _Results  );
bool M_ComputeHoles(  dm_real* _Results  ); 
bool M_ComputeParentLabels(  dm_real* _Results  ); 
bool M_ComputePosition( dm_real* _Results, int p );


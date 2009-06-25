
//To be included in cciMeasures.h

// Measures
void M_UpdateCount();
void M_UpdateCentroids(); 
void M_UpdateEuler(); 
void M_UpdateBoundary(); 
bool M_ComputeArea(  dm_real* _Results );
bool M_ComputeBoundary( dm_real* _Results );
bool M_ComputeBorder( dm_real* _Results );
bool M_ComputeMomentum( dm_real* _Results, int p, int q);
bool M_ComputeLabels( dm_real* _Results );
void M_UpdateDir(); 
void M_UpdateRectProps(); 
bool M_ComputeDir( dm_real* _Results );
bool M_ComputeRectProps( dm_real* _Results, int l );
bool M_ComputeEuler( dm_real* _Results  ); 
bool M_ComputeDepth( dm_real* _Results  );
bool M_ComputeHoles(  dm_real* _Results  ); 
bool M_ComputeParentLabels(  dm_real* _Results  ); 
bool M_ComputePosition( dm_real* _Results, int p );


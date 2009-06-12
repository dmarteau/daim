
//To be included in cciMeasures.h

void N_InsertNodes();
bool N_CreateNodes();
int  N_UpdatePartition(); 
void N_UpdateRoi();
void N_UpdatePartRoi();
int  N_BuildIndexTable(); 
void N_CollectChildLabels( XNODE node, bool include_holes );
void N_MergeChilds( XNODE node );
bool N_MergeNode( XNODE node );
bool N_ClearNode( XNODE node );
bool N_RemoveLabels();

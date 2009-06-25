
//To be included in cciMeasures.h

void M_InsertNodes();
bool M_CreateNodes();
int  M_UpdatePartition(); 
void M_UpdateRoi();
void M_UpdatePartRoi();
int  M_BuildIndexTable(); 
void M_CollectChildLabels( XNODE node, bool include_holes );
void M_MergeChilds( XNODE node );
bool M_MergeNode( XNODE node );
bool M_ClearNode( XNODE node );
bool M_RemoveLabels();

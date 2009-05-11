
// Define plugins for graycStoration
#ifndef dmCImgPlugins
#define dmCImgPlugins

 dmIUnknown*    _Progress;
 _Instance_Ptr  _Instance;

 void setprogress( const unsigned int pc, const unsigned int iter ) {
   if(_Progress) {
      _Progress->CCI_ISET( GREYCStoration, Progress, pc ); 
      stopflag = (_Instance->stopped != dm_false);
   }
 }
 
 void notify_iter( const unsigned int iter ) {
   if(_Progress) {
      _Progress->CCI_ISET( GREYCStoration, Iter, iter ); 
   }
 }

#endif // dmCImgPlugins

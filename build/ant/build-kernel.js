/*
 * build-kernel.js
 * 
 * Create C++ kernel description file
 * from XML definiton 
 * 
 * inputFile  : name of the xml input file
 * outputFile : name of the c++ output file
 *
 * @author : David Marteau
 */

try {
  // strip xml processing instruction declaration (E4X don't like it!)
  var echo    = kernel_builder.createTask("echo");  
  var input   = new XML(inputXML.replace(/^<\?xml[^>]+>\s*/,""));
  
  // Get thecvsc root node name
  var localName = input.localName();

  var output = "#ifndef "+localName+"_h\n"
             + "#define "+localName+"_h\n"
             + "\n";
  
  output += "namespace "+localName+" {\n\n";
  
  var familyEntries = "";
  
  // Get all families
  var families = input.family;
  for each(var family in families)
  {
    var familyString = family.@name.toString();
    trace(familyString);
    var familyName   = familyString
                        .replace(/[\(\)\-\s]/g,"_")
                        .replace(/_$/,"");

   // Get default value for kernel 
    var kx      = family.@x.toString();
    var ky      = family.@y.toString();
    var kwidth  = family.@width.toString();
    var kheight = family.@height.toString();
    var mode    = family.@mode.toString();

    
    output +="//============================\n";
    output +="// Kernel Family "+familyString+"\n";
    output +="// "+family.@title.toString()+"\n";
    output +="//============================\n\n";
    
    mode = mode ?  "\""+mode+"\"" : "dm_null";
    
    familyEntries += "ADD_FAMILY_ENTRY("+localName+","+familyName+","
                     +"\""+localName+"/"+familyString+"\","+mode+");\n";
    
    var kernels  = family.kernel;
    var koutput  = "";
    var kentries = "";

    var count = 0;
    
    for each(var kernel in kernels)
    {
      ++count;
      
      var kernelName = kernel.@name.toString()
                       .replace(/[\(\)\s\-]/,"_")
                       .replace(/_$/,"");
                        
      var x      = kernel.@x.toString()      || kx;
      var y      = kernel.@y.toString()      || ky;
      var width  = kernel.@width.toString()  || kwidth;
      var height = kernel.@height.toString() || kheight;
      var norm   = kernel.@norm.toString();

      var kernelID = (kernelName||("K"+count));
      
      koutput += "BEGIN_KERNEL("+familyName+","+kernelID+")\n"
                 +kernel.text()
                 +"\nEND_KERNEL\n\n";
                 
      kentries += "  KERNEL_ENTRY("+familyName+","+kernelID+","+x+","+y+","+width+","+height+","
                 +(norm ? norm : "0")
                 +")\n";
    }
 
    output += koutput;
    
    output += "BEGIN_FAMILY("+familyName+")\n";
    output += kentries;
    output += "END_FAMILY\n\n";

  }

  output += "} // namespace "+localName;
  
  output += "\n\n/*\n";         
  output += familyEntries;
  output += "*/\n"; 
      
  output += "\n#endif // "+localName+"_h";         
           
  trace("Appending to "+outputFile);
  
  // create a ant task
  var antcall = kernel_builder.createTask("antcall");  
  antcall.setTarget("generateHeaderContent");
  
  setParam(antcall,"outputFile",outputFile);
  setParam(antcall,"outputContent",output);
  
 // Execute Ant task
  antcall.execute(); 
} 
catch (e)
{
  trace(e);	
}

// Utility trace

function trace( message )
{
  echo.setMessage(message);
  echo.perform(); 
}

function setParam( task, name, value )
{
  var param = task.createParam();
  param.setName(name);
  param.setValue(value)
}


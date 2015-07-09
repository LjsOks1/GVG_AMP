/** \example test_gvgampclient.cpp
* \brief A short example of how to use the GvgAmpClient class in C++.
*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <gvgampclient/gvgampclient.h>

int main(int argc, char *argv[])
{
   using namespace gvgampclient;
   std::vector<std::string> folders;
   std::vector<std::string> files;
   std::string cwd;

   if(argc<2) {
       printf("Please specify host as the first argument.\n");
       exit(-1);
   }
   
   try {
       GvgAmpClient gvg_client(argv[1]);
       printf("Looks like it's connected\n");
  
       gvg_client.ListFirstFolder(folders);
       gvg_client.ListNextFolder(folders);       
       std::cout << "Listing folders:\n";
       for( std::vector<std::string>::const_iterator i = folders.begin(); 
                                                     i != folders.end(); ++i)
           std::cout << "   " << *i << '\n';   
       std::cout << "Done.\n"; 

       std::cout << "Listing files:\n";
       gvg_client.ListFirstID(files,1);
       gvg_client.ListNextID(files,10,1);
       for( std::vector<std::string>::const_iterator i = files.begin(); 
                                                     i != files.end(); ++i)
           std::cout << "   " << *i << '\n';   
       std::cout << "Done.\n"; 
  
       gvg_client.GetWorkingFolder(cwd);
       std::cout << "Current working folder:" << cwd << "\n";

  }
   catch(SocketException &e) {
       printf("Received an exception:%s\n",e.description().c_str());
   }
}


   

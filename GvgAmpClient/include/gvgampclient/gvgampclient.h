/** \defgroup public_api Public API
\brief This is the list of the functions that has been implemented so far from the GVG Advanced
media protocol. 
*/

#pragma once

#include <string>
#include <vector>
#include <sstream>

//#define AMP_PORT "3811"   //The port the client will connect to
#define MAXDATASIZE 1024  //max number of bytes we receive at once


//! Namespace contains all the classes of the GVG AMP client implementation.
namespace gvgampclient {

    #ifndef SocketException_class
    #define SocketException_class
//! Exception class. Thrown by GvgAmpClient class functions if something goes wrong.
/*! Application should use GvgAmpClient class in try blocks and catch
*   SocketException for error handling.
*/
    class SocketException {
        public:
            SocketException ( std::string s ) : m_s ( s ) {};
            ~SocketException (){};
            std::string description() { return m_s; }
        private:
            std::string m_s;
    };
    #endif

//! Main class that implements the client for GVG Advanced Media Protocol. 
/*! For the implemented functionality check the public member functions. 
*   You can find usage templates in the test folder.*/
    class GvgAmpClient {
        private:
            int sockfd; ///< Socket descriptor for the server connection.
            int sendCommand(char* command,
                            std::vector<unsigned char> &response, 
                            int isExtendedReply=0);
            int hexStringToByteArray(char*,
                                     std::vector<unsigned char>&);
            int stringToHexString(std::string,std::string&);
        public:
            GvgAmpClient(char* server,char* channel=NULL);
            ~GvgAmpClient();
/** \addtogroup public_api
*   @{  */
            int ListFirstFolder(std::vector<std::string> &);
            int ListNextFolder(std::vector<std::string>&);
            int ListFolders(std::vector<std::string>&);
            int ListFirstID(std::vector<std::string>&,
                            int isExtendedReply);
            int ListNextID(std::vector<std::string>&,
                            int file_count,
                            int isExtendedReply);
            int ListIDs(std::vector<std::string>&,
                            int isExtendedReply);
            int GetWorkingFolder(std::string&);
            int SetWorkingFolder(std::string);
            int SetAFDSetting(std::string,
                              int,
                              int);
            int GetAFDSetting(std::string,
                              int&,
                              int&);

/** @} End of public_api group */  
    };
}

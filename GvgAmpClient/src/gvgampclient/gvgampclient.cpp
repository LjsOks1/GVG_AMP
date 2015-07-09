/** \file 
* \brief GVG Advanced Media Protocol client implementation
*
* This file contains all the GvgAmpClient class implementation */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <gvgampclient/gvgampclient.h>


using namespace gvgampclient;

//! Constructor, that takes a hostname and an optional channel parameter.
/*! It tries to connect to the GVG server on the specified channel, or creates 
*   a channel-less connection if channel is not specified.
*   In case something goes wrong, throw a SocketException exception.
*   Valid values for channel are: Vtr1, Vtr2, Vtr3 and Vtr4 */
GvgAmpClient::GvgAmpClient(char* server,char* channel)
{
   int rv;
   struct addrinfo hints,*servinfo, *p;
   char strCommand[100],buf[MAXDATASIZE];

   memset(&hints,0,sizeof(hints));
   hints.ai_family=AF_UNSPEC;
   hints.ai_socktype=SOCK_STREAM;

   if((rv=getaddrinfo(server,"3811",&hints,&servinfo))==0) {
       //This is good so far,loop through the result and connect if we can..
       for (p=servinfo; p!=NULL; p=p->ai_next) {
          if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
             continue;
          if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1)
             continue;
          break;
       }
       if(p!=NULL) {
           //We have a connected socket here,initialize AMP...
           freeaddrinfo(servinfo); //we don't need this structure any more...
           if(channel==NULL or strlen(channel)==0) {
               //This is a channeless connection...
               sprintf(strCommand,"CRAT00014\n");
           } else {
               //This is a connection to a GVG channel...
               //TODO: We could have an enum for the valid channel names...
               sprintf(strCommand,"CRAT0007204%s\n",channel);
           }
               if(send(sockfd,strCommand,strlen(strCommand),0)>0){
               //The connection string was sent, waiting for reply...
               if(recv(sockfd,buf,4,0)==4) {
                   buf[4]='\0';
                   if(strcmp(buf,"1001")==0) {
                       //ACK received, this is good!
                       return;
                   } else {
                       //We have a connected socket here, but GVG refused the connection string
                       //Close the socket to be on the safe side...
                       close(sockfd);
                       throw SocketException("Server refused the connection string.");
                   }
               } else {
                   //Something was wrong with receiving response...
                   //Should not happen, throw extension and close the socket.
                   close(sockfd);
                   throw SocketException("Bad response from server.");
               }
           } else {
               //Something wrong with sending the connection string.
               //Should not happen, close socket and throw exception.
               close(sockfd);
               throw SocketException("Couldn't send command to server.");
           }
       } else {
           //We couldn't establish a connection to the server. Throw exception!
           throw SocketException("Couldn't establish connection to server.");
       }      
   } else {
       //We couldn't find address info for the requested server. Throw exception!
       throw SocketException("Couldn't find address to server.");
   }      
}
//! Disconnects from server and close the socket.
GvgAmpClient::~GvgAmpClient()
{
   char strCommand[100]; 
   char buf[MAXDATASIZE]; 
   sprintf(strCommand,"STOP0000\n");
   if(send(sockfd,strCommand,strlen(strCommand),0)>0){
       //The connection string was sent, waiting for reply...
       if(recv(sockfd,buf,4,0)==4) {
           buf[4]='\0';
           if(strcmp(buf,"1001")==0) {
               //ACK received, this is good! Let's close the socket...
               close(sockfd);
           } else {
               //We have a connected socket here, but GVG refused the disconnection string
               //Close the socket to be on the safe side...
               close(sockfd);
               throw SocketException("Server refused to disconnect.");
           }
       } else {
           //Something was wrong with receiving response...
           //Should not happen, throw extension and close the socket.
           close(sockfd);
           throw SocketException("Bad response from server at disconnect.");
       }
   } else { //Couldn't send disconnect request
       close(sockfd);
       throw SocketException("Couldn't send disconnect command.");
   }
}

//! Converts a variable length of hexstring to an array of bytes.
/*  A simple private utility function. Byte array is passed as a reference,
*   should be allocated by the calling function. */
//  TODO: Missing error checking, doesn't report error at all! 
int GvgAmpClient::hexStringToByteArray(char* input_string,
                                       std::vector<unsigned char> &byte_array) 
{
   char buf[3];
   std::stringstream ss(input_string);
   memset(buf,0,3);
   while(ss.read(buf,2)) {
      byte_array.push_back(strtoul(buf,NULL,16));
   }   
   return 0;
}

//! Converts a string to a hexstring.
/*  A simple private utility function. hex string is passed as a reference,
*   should be allocated by the calling function. */
int GvgAmpClient::stringToHexString(std::string input_string,
                                    std::string &hex_string) 
{
   char buf,hex_buf[3];
   std::stringstream ss(input_string);
   memset(hex_buf,0,3);
   while(ss.read(&buf,1)) {
      sprintf(hex_buf,"%02x",buf);
      hex_string+=hex_buf;
   }   
   return 0;
}


//! Private function to send a command and receive the response from a GVG device.
/*  Response is converted from hexstring to byte array before returned to the calling
*   function. Response it not imterpretted here.*/
int GvgAmpClient::sendCommand(char* command, 
                              std::vector<unsigned char> &response, 
                              int isExtendedReply)
{
   char strCommand[200],buf[MAXDATASIZE];
   char cmd1[3],cmd2[3],datalen[3];
   int icmd1,icmd2,idatalen;
   std::vector<unsigned char> data_array;
   
   sprintf(strCommand,"CMDS%04d%s\n",(int)strlen(command),command);
   if (send(sockfd,strCommand,strlen(strCommand),0)>0) {
       //Sent the command without error, receive response...
       if(recv(sockfd,cmd1,2,0)==2) {
           //Received the first two bytes....
           icmd1=strtoul(cmd1,NULL,16);
           if((icmd1&0x0f)==0) 
               isExtendedReply=0;
           if(isExtendedReply) { //Extended response, can be much longer then standard.
               //We expect an extended reply, process the rest of the response...
               recv(sockfd,cmd2,2,0);
               icmd2=strtoul(cmd2,NULL,16);
               recv(sockfd,datalen,4,0);
               idatalen=strtoul(datalen,NULL,16);
               //buf has the hexstring, that should be converted to bytearray...
               recv(sockfd,buf,idatalen*2+2,0);
               hexStringToByteArray(buf,data_array);
               response.push_back(icmd1%256);
               response.push_back(icmd2%256);
               response.push_back(idatalen/256);
               response.push_back(idatalen%256);
               response.insert(response.end(),data_array.begin(),data_array.end());
            } else { // Standard response, simpler than the extended ones.
               idatalen=icmd1 & 0x0f;
               recv(sockfd,buf,idatalen*2+2,0);
               hexStringToByteArray(buf,data_array);
               response.push_back(icmd1%256);
               response.insert(response.end(),data_array.begin(),data_array.end());     
            }       
       }
   }                      
   return 0;
}
//! Implements the (A0.2A) List First Folder command.
/*! This command requests the name of the first folder that currently exists in the 
* video disk recorder's storage. 
* \param folders A vector of strings with one element with the name of the first folder. 
* \return The number of folder names added to the in string vector. 0 means no folder found. */
int GvgAmpClient::ListFirstFolder(std::vector<std::string> &folders)
{
   std::vector<unsigned char> response; //The full response structure from the server
   int length; // The length of the directory name
   char folder_name[255];
   memset(folder_name,0,255);
   if(sendCommand((char*)"a02a",response,1)==0) {
       //Received a valid response
       if(response[0]==0x82) {
           //At least one folder present
           length=response[4]*256+response[5];
           strncpy(folder_name,(const char*)&response[6],length); 
           folders.push_back(folder_name);
           return 1;
       } else { //We don't have any folders.
           return 0;
       }          
   } else { //Something was wrong, throw exception.
       throw SocketException("ListFirstFolder failed.");
       return -1;
   }
}

//! Implements the (A0.2B) List Next Folder command.
/*! This command requests the next set of folders that currently exists in the video
* disk recorder's storage. Folder names are returned in extended format. In one shot 
* as many folder names are returned as fit into a 255 byte data stream. 
* Should be called subsequently untill 80.2A returned. */
int GvgAmpClient::ListNextFolder(std::vector<std::string> &folders)
{
   std::vector<unsigned char> response; //The full response structure from the server
   int length;        // The length of the current directory name
   int total_length;  // The total length of the response data       
   int pos;           // Actual position of response parsing
   char folder_name[255];  // Actual retrieved directory name
   int counter=0;  //count the number of folders in response

   if(sendCommand((char*)"a02b",response,1)==0) {
       //Received a valid response
       if(response[0]==0x82) {
       //One or more folders returned in response...
           total_length=response[2]*256+response[3];
           pos=0;
           while(total_length>pos) {
               memset(folder_name,0,255);
               length=response[4+pos]*256+response[5+pos];
               strncpy(folder_name,(const char*)&response[6+pos],length); 
               folders.push_back(folder_name);       
               counter++;
               pos=pos+length+2;
           }
           return counter;
       } else { //No folder names in response...
           return 0;
       }
   } else { //Something was wrong, throw exception.
       throw SocketException("ListNextFolder failed.");
       return -1;
   }
   return 0;
}

//! Helper function to list all the folders with one command.
/*! This is not in the AMP command set but it's more convinient to use this function
*   instead of the ListFirstFolder and subsequent ListNextFolder commands. */
int GvgAmpClient::ListFolders(std::vector<std::string> &folders)
{
   int counter=0;
   int i;
   if(ListFirstFolder(folders)>0) {
       //At least one folder exists, let's check if we have more...
       counter=1;
       while((i=ListNextFolder(folders))>0) {
           counter+=i;
       }
       return counter;
   } else {
       //There was no folder. Strange, at least we should have a default one.
       return 0;
   }
}
   
//! Implements the (AX.14) List First ID command
/*! This command requests the first ID in sorted order that currently exists in the
*   video disk recorder's storage. Moves the current listing position to the second
*   ID. Returns clip name in a string vector. */
int GvgAmpClient::ListFirstID(std::vector<std::string> &files,
                              int isExtendedReply)
{
   std::vector<unsigned char> response; //The full response structure from the server
   int length;                          //Length of the received filename
   char filename[255];                  //The name of the received file

   memset(filename,0,255);
   if(isExtendedReply) { //We need filename in extended format.
       if(sendCommand((char*)"a2140000",response,1)==0) {
           //Received valid extended response from server
           if(response[0]==0x8a) {
               //One clipname returned;
               length=response[4]*256+response[5];
               strncpy(filename,(const char*)&response[6],length);
               files.push_back(filename);
               return 1;
           } else {  //No clips in the folder.
               return 0;
           }
       } else {  //Something was wrong, throw exception
           throw SocketException("ListFirstID - Extended version - failed.");
           return -1;
       }
           
   } else {  //We need filename in standard format. (Exactly 8 character.)
       if(sendCommand((char*)"a014",response,0)==0) {
           //Received a valid standard response, exactly 8 characters
           if(response[0]==0x88) { //There is a clipname in response
               strncpy(filename,(const char*)&response[2],8);
               files.push_back(filename);
               return 1;
           } else {  //No clipname in response.
               return 0;
           }
       } else { //Something was wrong, throw exception
           throw SocketException("ListFirstID - Standard format - failed.");
           return -1;
       }
   }
}
//! Implements the (AX.15) List Next ID command.
/*! This command requests the next set of clips that currently exists in the video
* disk recorder's storage. Clip names are returned in either extended or standard
* format.  In extended format as many clip names are returned as fit into a 255 byte 
* data stream. Should be called subsequently untill 80.14 returned.*/
int GvgAmpClient::ListNextID(std::vector<std::string> &files,
                     int file_count, //Number of clipnames to return.(Default:5)
                      int isExtendedReply)
{
   std::vector<unsigned char> response; //The full response structure from the server
   int length;        // The length of the current clip name
   int total_length;  // The total length of the response data       
   int pos;           // Actual position of response parsing
   char clip_name[255];  // Actual retrieved clip name
   char strCommand[20];
   int counter=0;

   if(isExtendedReply) {
       sprintf(strCommand,"a115%02x",file_count);
       if(sendCommand(strCommand,response,1)==0) {
           //Received a valid response
           if(response[0]==0x8a) { //At least one clipname in response
               total_length=response[2]*256+response[3];
               pos=0;
               while(total_length>pos) {
                   memset(clip_name,0,255);
                   length=response[4+pos]*256+response[5+pos];
                   strncpy(clip_name,(const char*)&response[6+pos],length); 
                   files.push_back(clip_name);       
                   counter++;
                   pos=pos+length+2;
               }
               return counter;
           } else {  //No files returned in response.
               return 0; //No exception, but returns 0.
           }
            
       } else { //Something was wrong, throw exception.
           throw SocketException("ListNextID - Extended version - failed.");
           return -1;
       }
   } else { //Standard reply, one clipname with 8 characters.
       if(sendCommand((char*)"a015",response,0)==0) {
           //Valid standard response
           if(response[0]==0x88) { //There is one clipname in standard format in response
               memset(clip_name,0,255);
               strncpy(clip_name,(const char*)&response[2],8);
               files.push_back(clip_name);
               return 1;
           } else { //No clipname in response
               return 0;
           }
       } else { //Something was wrong, throw exception
         throw SocketException("ListNextID - Standard version - failed.");
          return -1;
       } 
   }   
}

//! Helper function to list all the clipnames in a folder with one command.
/*! This is not in the AMP command set but it's more convinient to use this function
*   instead of the ListFirstID and subsequent ListNextID commands. */
int GvgAmpClient::ListIDs(std::vector<std::string> &clips,int isExtendedReply)
{
   int counter=0;
   int i;
   if(ListFirstID(clips,isExtendedReply)>0) {
       //At least one clipname exists, let's check if we have more...
       counter=1;
       while((i=ListNextID(clips,10,isExtendedReply))>0) {
           counter+=i;
       }
       return counter;
   } else {
       //There was no clipname. 
       return 0;
   }
}
 
//! Implements the (A0.0F) Get Working Folder command.
/*! This command requests the name of the working folder. Returns a string.*/
int GvgAmpClient::GetWorkingFolder(std::string &folder)
{
   std::vector<unsigned char> response; //The full response structure from the server
   int length; // The length of the directory name
   char folder_name[255];
   memset(folder_name,0,255);
   if(sendCommand((char*)"a00f",response,1)==0) {
       //Received a valid response
       length=response[4]*256+response[5];
       strncpy(folder_name,(const char*)&response[6],length); 
       folder=folder_name;       
   } else { //Something was wrong, throw exception.
       throw SocketException("GetWorkingFolder failed.");
       return -1;
   }
   return 0;
}

//! Implements the (A2.0E) Set Working Folder command.
/*! This command sets the working folder. */
// TODO:Clip cache needs reinitialized. How to handle that?
int GvgAmpClient::SetWorkingFolder(std::string folder)
{
   std::vector<unsigned char> response; //The full response structure from the server
   char strCommand[100];
   std::string hex_folder;

   stringToHexString(folder,hex_folder);
   sprintf(strCommand,"a20e%04x%04x%s",(unsigned int)folder.length()+2,
                             (unsigned int)folder.length(),hex_folder.data());
   if(sendCommand(strCommand,response,0)==0) {
       //Received a valid response
       if(response[0]==0x10 && response[1]==0x01) {
           //ACK received
           return 0;
       } else {
           return -1;
       }
    } else { //Something was wrong, throw exception.
       throw SocketException("SetWorkingFolder failed.");
       return -1;
    }
}

//! Implements the (A2.3B) Set AFD Setting command.
/*! This command sets the AFD and the widescreen data of the specified clip. */
int GvgAmpClient::SetAFDSetting(std::string filename,
                                int afd,
                                int widescreen)
{
   std::vector<unsigned char> response; //The full response structure from the server
   char strCommand[100];
   std::string hex_filename;

   stringToHexString(filename,hex_filename);
   sprintf(strCommand,"a23b%04x%02x%02x%04x%s",(unsigned int)filename.length()+4,
                                       (unsigned int)afd,
                                       (unsigned int)widescreen,
                                       (unsigned int)filename.length(),
                                       hex_filename.data());
   if(sendCommand(strCommand,response,0)==0) {
       //Received a valid response
       if(response[0]==0x10 && response[1]==0x01) {
           //ACK received
           return 0;
       } else {
           return -1;
       }
    } else { //Something was wrong, throw exception.
       throw SocketException("SetAFDSetting failed.");
       return -1;
    }
}

//! Implements the (A2.3A) Get AFD Setting command.
/*! This command gets the AFD and the widescreen data of the specified clip. 
*   Incase response arrives, but clip doesn't exists returns -1
* \param filename The clipname to get the AFD and widescreen flag for.
* \param afd The value returned for the AFD code. (Valid if function returns 0!)
* \param widescreen The value returned for the widescreen code. (Valid if function returns 0!)
* \return If everything was ok, returns 0, otherwise -1
* \exception SocketException if something was wrong with the communication. */
int GvgAmpClient::GetAFDSetting(std::string filename,
                                int &afd,
                                int &widescreen)
{
   std::vector<unsigned char> response; //The full response structure from the server
   char strCommand[100];   //The command to send to the server
   std::string hex_filename;  //input filename in hexstring format

   stringToHexString(filename,hex_filename);
   sprintf(strCommand,"a23a%04x%04x%s",(unsigned int)filename.length()+2,
                                       (unsigned int)filename.length(),
                                       hex_filename.data());
   if(sendCommand(strCommand,response,0)==0) {
       //Received a valid response
       if(response[0]==0x82 && response[1]==0x3a) {
           //AFD and WS settings received
           afd=response[2];
           widescreen=response[3];
           return 0;
       } else {
           return -1;
       }
    } else { //Something was wrong, throw exception.
       throw SocketException("SetAFDSetting failed.");
       return -1;
    }
}



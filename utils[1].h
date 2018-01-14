// ********************** Utilities Code ************************************
// Author        :      Chiranjeev Ghosh (chiranjeev.ghosh@tamu.edu)
// Organization  :      Texas A&M University, CS for ECEN 602 Assignment 1
// Description   :      Contains writen() and readline() function definitions
// Last_Modified :      10/08/2017

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int MAX_STR_LEN = 512;


// ********************** SBCP format structures definitions *************************


// All fields allocated according to protocol spec


struct SBCP_Attribute_Format {
  unsigned int                   Type   : 16;                     
  unsigned int                   Length : 16;
  char                           Payload [512];
};


struct SBCP_Message_Format {
  unsigned int                   Version : 9;                     
  unsigned int                   Type    : 7;                     
  unsigned int                   Length  : 16;                    
  struct SBCP_Attribute_Format   SBCP_Attributes;
};






int err_sys(const char* x)    // Error display source code
{ 
  error(x); 
  exit(1); 
}

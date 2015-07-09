%module pGvgAmpClient
%include <std_vector.i>
%include <std_string.i>
%include "typemaps.i"

%typemap(out) std::vector<std::string>                     
  { 
       array_init(return_value); 
       for(unsigned int i=0;i<$1.size();++i) 
           add_next_index_string(return_value,$1[i].c_str(),1); 
  } 

%typemap(argout,noblock=1) std::vector<std::string>&
  { 
       array_init((*$input)); 
       for(unsigned int i=0;i<$1->size();++i) 
           add_next_index_string((*$input),(*$1)[i].c_str(),1); 
  } 

%typemap(in) std::vector<std::string>&  (std::vector<std::string>  vec) 
{ 
      zval **data; 
      HashTable *hash; 
      HashPosition ptr; 

      hash = Z_ARRVAL_PP($input); 
      for( 
          zend_hash_internal_pointer_reset_ex(hash,&ptr); 
          zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS; 
          zend_hash_move_forward_ex(hash,&ptr) 
         ) 
      { 
          zval temp, *str; 
          int is_str = 1; 
          if(Z_TYPE_PP(data) != IS_STRING) 
          { 
              temp = **data; 
              zval_copy_ctor(&temp); 
              convert_to_string(&temp); 
              str =&temp; 
              is_str = 0; 
          } 
          else 
              str = *data; 
          vec.push_back(std::string(Z_STRVAL_P(str))); 
          if(!is_str) 
              zval_dtor(&temp); 
      } 
      $1 =&vec; 
} 


namespace std {
#   %template(IntVector) vector<int>;
#   %template(DoubleVector) vector<double>;
   %template(StringVector) vector<string>;
#   %template(ConstCharVector) vector<const char*>;
}
#%apply std::vector<std::string> OUTPUT { std::vector<std::string> &result };

%apply int &OUTPUT {int &}

%{
#include <gvgampclient/gvgampclient.h>
%}
%include <gvgampclient/gvgampclient.h>

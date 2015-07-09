%module jGvgAmpClient

%rename("cpp_toString") toString;

%include std_string.i

%{
#include <gvgampclient/gvgampclient.h>
%}

%include <gvgampclient/gvgampclient.h>

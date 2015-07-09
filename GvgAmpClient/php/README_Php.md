There is a good chance that php compilation will fail if not the latest version of
cmake and swig are installed on the target system. This is the case with ibm4.

It is still possible to compile the PHP module manually, please follow the next procedure:
(use root account to keep it simple.)

#Checkout source tree from SVN:
* `cd /usr/local/src`
* `mkdir GvgAmpClient_release`
* `svn checkout http://10.28.98.14/svn/MTVA/GVG_AMP/GvgAmpClient GvgAmpClient`
* `cd GvgAmpClient_release`

#Build and install at least the C++ library with cmake...
* `cmake ../GvgAmpClient`
* `make`
* `cp src/libGvgAmpClientShared.so /usr/lib64`
* `ldconfig`

#Now build the php extension manually...
* `cd ../GvgAmpClient/php`
* `swig -php -c++ -I../include gvgampclient.i`
* `gcc `php-config --includes` -I../include -fpic -c -v gvgampclient_wrap.cpp`
* `gcc -shared *.o -o libGvgAmpClientPhp.so -lstdc++ -lGvgAmpClient -lgcov`
* `cp libGvgAmpClientPhp.so `php-config --extension-dir`
* `cp pGvgAmpClient.php /usr/share/php

Add the extension to php.ini by inserting `extension = libGvgAmpClientPhp.so` to /etc/php.ini.
Make sure that extension_dir contains whatever `php-config --extension-dir` is.

#Check if library can be loaded by running `php -m`



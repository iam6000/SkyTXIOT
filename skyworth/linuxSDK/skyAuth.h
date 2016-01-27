/*!
* funcs for auth  
*  @ add by 6K 2015/12/15
*/




unsigned char * makePostJSON(unsigned char* sn);


bool parseJSONString(char * jsonStr , char* license) ;



bool getSnNum(unsigned char* snNum);



bool getLicenseFromService(char * license , unsigned char* snNum) ;



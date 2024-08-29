#include <Arduino.h>
#include <Configuration.h>

#ifndef MYCONFIG_H
#define MYCONFIG_H

class MyConfig : public Configuration
{
private:
public:


    MyConfig() : Configuration()
    {
        HOSTNAME = "TEST_CC1101";
    }
   
};

#endif

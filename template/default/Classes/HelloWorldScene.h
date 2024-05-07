#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "includes.h"

class HelloWorld : public Layer
{
public:
    HelloWorld(); // constructor
    virtual ~HelloWorld(); // virtual destructor

    CREATE_FUNC(HelloWorld); // create() function
    static Scene* scene(); // scene function

    virtual bool init(); // init function
    
    virtual void keyBackClicked(); // key back(escape on pc) callback
};

#endif // __HELLOWORLD_SCENE_H__

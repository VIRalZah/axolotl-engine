#include "HelloWorldScene.h"
#include "base/Value.h"

HelloWorld::HelloWorld()
{
}

HelloWorld::~HelloWorld()
{
}

Scene* HelloWorld::scene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);

    return scene;
}

bool HelloWorld::init()
{
    if (!Layer::init())
    {
        return false;
    }

    setKeypadEnabled(true); // for keyBackClicked()
    
    auto director = Director::getInstance();

    const auto origin = director->getVisibleOrigin();
    const auto visibleSize = director->getVisibleSize();

    auto axolotl = Sprite::create("Axolotl.png"); // create sprite

    axolotl->setPosition(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2
    ); // set position to center

    addChild(axolotl); // add sprite to layer

    return true;
}

void HelloWorld::keyBackClicked()
{
    Director::getInstance()->getGLView()->setFrameSize(640, 320); // close game in next frame
}
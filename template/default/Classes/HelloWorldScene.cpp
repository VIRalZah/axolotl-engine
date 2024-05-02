#include "HelloWorldScene.h"

USING_NS_AX;

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
    
    auto director = Director::sharedDirector();

    Point origin = director->getVisibleOrigin();
    Size visibleSize = director->getVisibleSize();

    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        this,
        menu_selector(HelloWorld::menuCloseCallback)
    );
    
	closeItem->setPosition(
        Point
        (
            origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
            origin.y + closeItem->getContentSize().height / 2
        )
    );

    Menu* menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    addChild(menu, 1);

    auto helloWorld = Sprite::create("HelloWorld.png");

    helloWorld->setPosition(
        Point(
            visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y
        )
    );

    addChild(helloWorld);

    return true;
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
    Director::sharedDirector()->end();
}
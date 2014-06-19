#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer, 0);
    
    auto hud = new HudLayer();
    hud->init();
    scene->addChild(hud, 1);
    layer->_hud = hud;
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    _tileMap = TMXTiledMap::create("TileMap.tmx");
    _background = _tileMap->getLayer("Background");
    
    this->addChild(_tileMap);
    
    auto objectGroup = _tileMap->getObjectGroup("Objects");
    if (objectGroup == nullptr) {
        log("tile map has no objects object layer");
        return false;
    }
    ValueMap object = objectGroup->getObject("SpawnPoint");
    int x = object["x"].asInt();
    int y = object["y"].asInt();
    _player = Sprite::create("Player.png");
    _player->setPosition(Vec2(x, y));
    this->addChild(_player);
    this->setViewPointCenter(_player->getPosition());
    
    _meta = _tileMap->getLayer("Meta");
    _meta->setVisible(false);
    _foreground = _tileMap->getLayer("Foreground");
    
    this->enableTouchEvent(true);
    return true;
}

void HelloWorld::setViewPointCenter(cocos2d::Vec2 position)
{
    auto winSize = Director::getInstance()->getWinSize();
    float x = MAX(position.x, winSize.width / 2);
    float y = MAX(position.y, winSize.height / 2);
    x = MIN(x, (_tileMap->getMapSize().width * _tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
    auto actualPosition = Vec2(x, y);
    auto centerOfView = Vec2(winSize.width/2, winSize.height/2);
    auto viewPoint = centerOfView - actualPosition;
    this->setPosition(viewPoint);
}

Vec2 HelloWorld::tileCoordForPosition(cocos2d::Vec2 position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    
    return Vec2(x, y);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::setPlayerPosition(cocos2d::Vec2 position)
{
    Vec2 tileCoord = this->tileCoordForPosition(position);
    int tileGid = _meta->getTileGIDAt(tileCoord, nullptr);
    if (tileGid) {
        Value properties = _tileMap->getPropertiesForGID(tileGid);
        ValueMap map = properties.asValueMap();
        if (map.size() != 0) {
            if (map.find("Collidable") != map.end()) {
                if (map.at("Collidable").asString().compare("True") == 0) {
                    return;
                }
            } else if (map.find("Collectable") != map.end()) {
                if (map.at("Collectable").asString().compare("True") == 0) {
                    log("setPlayerPosition Collectable!");
                    _numCollected++;
                    _hud->numCollectedChanged(_numCollected);
                    _meta->removeTileAt(tileCoord);
                    _foreground->removeTileAt(tileCoord);
                }
            }
        }
    }
    _player->setPosition(position);
}

#pragma mark -
#pragma mark touch events
void HelloWorld::enableTouchEvent(bool enabled)
{
    if (this->_touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(this->_touchListener);
        this->_touchListener = nullptr;
    }
    if (enabled) {
        this->_touchListener = EventListenerTouchOneByOne::create();
        _touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
        _touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
        _touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
        _touchListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, this);
    }
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
    
    return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *event)
{
}

void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{
    auto location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);

    location = this->convertToNodeSpace(location);
    
    Vec2 playerPos = _player->getPosition();
    Vec2 diff = location - playerPos;
    if (abs(diff.x) > abs(diff.y)) {
        if (diff.x > 0) {
            playerPos.x += _tileMap->getTileSize().width;
        } else {
            playerPos.x -= _tileMap->getTileSize().width;
        }
    } else {
        if (diff.y > 0) {
            playerPos.y += _tileMap->getTileSize().height;
        } else {
            playerPos.y -= _tileMap->getTileSize().height;
        }
    }
    
    if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getTileSize().width)
    &&  playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getTileSize().height)
    &&  playerPos.y >= 0
    &&  playerPos.x >= 0
    ) {
        this->setPlayerPosition(playerPos);
    }
    this->setViewPointCenter(_player->getPosition());
}

void HelloWorld::onTouchCancelled(Touch *touch, Event *event)
{
}

#pragma mark -
//
//  HudLayer.cpp
//  t03
//
//  Created by 野口一也 on 2014/06/12.
//
//

#include "HudLayer.h"

USING_NS_CC;

bool HudLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    Size winSize = Director::getInstance()->getWinSize();
    _label = LabelTTF::create("0", "Verdana-Bold", 18.0);
    _label->setColor(Color3B(0, 0, 0));
    
    int _margin = 10;
    _label->setPosition(Vec2(winSize.width - (_label->getContentSize().width/2) - _margin, _label->getContentSize().height/2 + _margin));
    this->addChild(_label);
    return true;
}

void HudLayer::numCollectedChanged(int numCollected)
{
    _label->setString(std::to_string(numCollected));
//    _label->setColor(Color3B(0, 0, 0));
}
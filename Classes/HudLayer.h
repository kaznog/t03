//
//  HudLayer.h
//  t03
//
//  Created by 野口一也 on 2014/06/12.
//
//

#ifndef __t03__HudLayer__
#define __t03__HudLayer__

#include "cocos2d.h"

USING_NS_CC;

class HudLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CREATE_FUNC(HudLayer);
    
    void numCollectedChanged(int numCollected);
private:
    LabelTTF* _label;
};

#endif /* defined(__t03__HudLayer__) */

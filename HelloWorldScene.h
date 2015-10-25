#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <iostream>
USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    virtual bool init();
 
    void menuCloseCallback(cocos2d::Ref* pSender);
     //瓦片地图
    TMXTiledMap *map;
    
    Sprite *_player;
    Point _beginPoint;// 起始点
    TMXLayer *_meta; // 障碍物
    TMXLayer *_fruit;//西瓜

    __Array *_enemyArray; //敌人数组
    __Array *_bulletArray; //子弹数组
    
    void goon(Node *sender);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    void movePlayer(Point p);//移动英雄函数
    void setViewPointCenter(Point position);//设置层视角
    void updataGame(float index);
    void targetFinish(Node *node);
    
    CREATE_FUNC(HelloWorld);
};

class ButtonMenu:public Layer{
public:
    virtual bool init();
    void change();
    CREATE_FUNC(ButtonMenu);


};



#endif // __HELLOWORLD_SCENE_H__

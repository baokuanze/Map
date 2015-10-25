#include "HelloWorldScene.h"

USING_NS_CC;
bool isbullet=false;
bool ButtonMenu::init(){
    if (!Layer::init()) {
        return false;
    }
    auto item1=MenuItemImage::create("projectile-button-off.png","projectile-button-off.png");
    auto item2=MenuItemImage::create("projectile-button-on.png","projectile-button-on.png");
    auto toggle=MenuItemToggle::createWithCallback(CC_CALLBACK_0(ButtonMenu::change, this), item1,item2, NULL);
    auto menu=Menu::create(toggle,NULL);
    menu->setPosition(Point(40,40));
    this->addChild(menu,100);
    return true;
}


void ButtonMenu:: change(){
    isbullet=!isbullet;
    
}

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    auto menu=ButtonMenu::create();
    scene->addChild(menu,100);
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    //初始化存放敌人的数组
    _enemyArray=__Array::create();
    _enemyArray->retain();
    //初始化存放子弹的数组
    _bulletArray=__Array::create();
    _bulletArray->retain();
    
    map=TMXTiledMap::create("Mymap.tmx");
    this->addChild(map);
    //获取地图中的不能行走的层 （墙等障碍物）
    _meta=map->getLayer("meta");
    _meta->setVisible(false);
    _fruit=map->getLayer("Fruit");
    
    //获取对象图层hero中的对象数组
    TMXObjectGroup *objectsValue=map->getObjectGroup("hero");
    //获取地图上的设置好的英雄对象
    ValueMap spawnPoint=objectsValue->getObject("pa");
    //根据地图上设置好的英雄位置来设置当前精灵的位置
    //添加英雄精灵
    _player=Sprite::create("www.png");
    _player->setAnchorPoint(Point(0,0));
    _player->setPosition(Point(spawnPoint.at("x").asFloat(),spawnPoint.at("y").asFloat()));
    this->addChild(_player);
    
     //快速遍历Vector，里面存放的是对象层中的所有对象
    for(auto element:objectsValue->getObjects()){
        log("%s",element.getDescription().c_str());
        if (!element.isNull()) {
            auto nameStr=element.asValueMap().at("name").asString().c_str();
            auto keyStr="enemy";
            if (strcmp(nameStr, keyStr)==0) {
                //初始化敌人精灵，把地图中已经初始化好的敌人对象初始化为要用的敌人精灵
                auto enemySprite=Sprite::create("enemy1.png");
                float x=element.asValueMap().at("x").asFloat();
                float y=element.asValueMap().at("y").asFloat();
                enemySprite->setPosition(Point(x-200,y));
                enemySprite->cocos2d::Node::setAnchorPoint(Point(0,0));
                map->addChild(enemySprite,4);
                _enemyArray->addObject(enemySprite);
                auto func=CallFuncN::create(CC_CALLBACK_1(HelloWorld::goon, this));
                enemySprite->runAction(func);
            }
        }
    }

    auto dispather=Director::getInstance()->getEventDispatcher();
    auto listener=EventListenerTouchOneByOne::create();
    listener->onTouchBegan=CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved=CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded=CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    dispather->addEventListenerWithSceneGraphPriority(listener, this);
    //每隔0.1秒监听一次是否发生碰撞
    this->schedule(schedule_selector(HelloWorld::updataGame), 0.1);
    return true;
}


void HelloWorld:: updataGame(float index){
    Ref *obj=NULL; //数组的快熟遍历
    CCARRAY_FOREACH(_enemyArray, obj){
        //强制类型转化
        auto s=static_cast<Sprite*>(obj);
        if (_player->getBoundingBox().intersectsRect(s->getBoundingBox())) {
            //
            // // Director::getInstance()->replaceScene(MainMenu::scene());
        }
    
    }
    CCARRAY_FOREACH(_bulletArray, obj){
        auto bullet=(Sprite*)obj;
        CCARRAY_FOREACH(_enemyArray, obj){
            auto enemy=(Sprite *)obj;
            //当两个有接触的的时候就是true；
            if (bullet->getBoundingBox().intersectsRect(enemy->getBoundingBox())) {
                enemy->removeFromParent();
                bullet->removeFromParent();
                _bulletArray->removeObject(bullet);
                _enemyArray->removeObject(enemy);
                break;
            }
        
        }
    }
}

void HelloWorld:: goon(Node *sender){
    auto s=(Sprite*)sender;
    //x轴方向的移动距离
    float x=_player->getPosition().x-s->getPosition().x>0?10:-10;
    //y轴方向上的移动距离
    float y=_player->getPosition().y-s->getPosition().y>0?10:-10;
    auto move=MoveBy::create(0.5, Point(x,y));
    //递归抵用，让敌人精灵移动
    auto func=CallFuncN::create(CC_CALLBACK_1(HelloWorld::goon, this));
    s->runAction(Sequence::create(move,func, NULL));
}

 bool HelloWorld:: onTouchBegan(Touch *touch, Event *unused_event){
     //获取当前触摸到点的位置，属于世界坐标系
     auto touchLocation=touch->getLocation();
     log("%f,%f",touchLocation.x,touchLocation.y);
     _beginPoint=this->convertToNodeSpace(touchLocation);
     log("%f,%f",_beginPoint.x,_beginPoint.y);
     return true;
 }
 void HelloWorld:: onTouchMoved(Touch *touch, Event *unused_event){
     
 }
 void HelloWorld:: onTouchEnded(Touch *touch, Event *unused_event){
     //获取当前
     Point touchLocation=touch->getLocation();
     Point endPoint=this->convertToNodeSpace(touchLocation);
     if (!isbullet) {
         //如果开始和结束是同一个点，当用户抬起手指的时候触发
         if (_beginPoint.equals(endPoint)) {
             //获取精灵当前的位置
             auto playerPos=_player->getPosition();
             //得到用户触摸点与当前精灵点的位置的差距
             auto disPos=endPoint-playerPos;
             //判断偏移的距离是偏上下还是偏左右
             if(abs(disPos.x)>=abs(disPos.y)){
                 if (disPos.x>0) {
                     //每次偏移一个图块的宽度。
                     playerPos.x+=map->getTileSize().width;
                 }else{
                     playerPos.x-=map->getTileSize().width;
                 }
             }else{ //偏上下
                 if(disPos.y>0 ){
                 //每次偏移一个图块的高度
                     playerPos.y+=map->getTileSize().height;
                 }else{
                 
                     playerPos.y-=map->getTileSize().height;
                 }
             }
             //移动精灵到特定的位置
              movePlayer(endPoint);
             //让地图层随着精灵一起运动
             setViewPointCenter(_player->getPosition());
         }
     }else{
     //发射飞镖
         auto bullet=Sprite::create("Projectile.png");
         bullet->setPosition(_player->getPosition());
         map->addChild(bullet,4);
         //将子弹添加到数组中
         _bulletArray->addObject(bullet);
         
         //当前手触摸点与英雄精灵x轴上的差值
         float dx=endPoint.x-_player->getPosition().x;
         float dy=endPoint.y-_player->getPosition().y;
         //向前发子弹
         if(dx>0){
             auto winSize=Director::getInstance()->getWinSize();   //_player英雄精灵的位置
             float lx=map->getTileSize().width*map->getMapSize().width-_player->getPosition().x;
             float ly=dy/dx *lx;//??
             auto move=MoveBy::create(3, Point(lx+bullet->getContentSize().width,ly));
             //子弹发射完毕之后执行回收子弹的函数。
             auto ff=CallFuncN::create(CC_CALLBACK_1(HelloWorld::targetFinish, this));
             bullet->runAction(Sequence::create(move,ff, NULL));
         }else{
             float lx=0-_player->getPosition().x;
             float ly=dy/dx *lx;
             auto move=MoveBy::create(3, Point(lx-bullet->getContentSize().width,ly));
             auto ff=CallFuncN::create(CC_CALLBACK_1(HelloWorld::targetFinish, this));
             bullet->runAction(Sequence::create(move,ff, NULL));
         
         }
     }
 }
void HelloWorld::targetFinish(Node *node){
    node->removeFromParent();
    _bulletArray->removeObject(node);


}



//移动英雄函数
void HelloWorld:: movePlayer(Point p){
 //   map->getMapSize(); 地图的图块数
 //   map->getTileSize(); //每一个图块的宽高
    int x=p.x / map->getTileSize().width;//定位在X轴的第几块上
    int y=map->getMapSize().height-(p.y/ map->getTileSize().height);//定位在y轴的第几块上
    //得到当前的定位点
    Point currentPoint=Point(x,y);
    //判断是否在地图范围内
    if(x!=map->getMapSize().width&&y!=map->getMapSize().height){
    //是否获取到目标，根据指定的点获取到瓦片的id;
        int tilegId=_meta->getTileGIDAt(currentPoint);
        //3.0中返回的类型是Value根据图块id获取当前图块上的各个属性
        Value value=map->getPropertiesForGID(tilegId);
        //一字符串的形式获取value中的内容
        auto valuStr=value.getDescription();
        //如果瓦片存在
        if(tilegId){
        //判断value是否为空
            if(!value.isNull()){
            //如果遇到障碍物，且不是西瓜就不让精灵移动
                if(valuStr.find("Collidable")!=-1){
                    return;
                }else if (valuStr.find("Collection")!=-1){
                    //如果遇到了水果
                    _meta->removeTileAt(currentPoint);
                    //获取当前的水果，并将其从视图中移除
                    auto fruitSprite=_fruit->getTileAt(currentPoint);
                    fruitSprite->removeFromParent();
                }
            }
        }
        //让英雄移动到点击的瓦片上
        _player->setPosition(p);
    }
}
//设置层视角
void HelloWorld:: setViewPointCenter(Point position){
    auto winSize=Director::getInstance()->getWinSize();
    //如果精灵在屏幕的1/2~4/2 x的值就是精灵当前的x的值
    int x=MAX(position.x,winSize.width/2);
    int y=MAX(position.y, winSize.height/2);
    //获取整个地图的快数
    auto mapSize=map->getMapSize();
    //获取每一个图块的宽高
    auto tileSize=map->getTileSize();
    //重新规划xy 的值
    x=MIN(x, mapSize.width*tileSize.width-winSize.width/2);
    y=MIN(y, mapSize.height*tileSize.height-winSize.height/2);
    //获取屏幕中心的位置
    auto centerPoint=Point(winSize.width/2,winSize.height/2);
    auto actualPoint=Point(x,y);
    auto viewPoint=centerPoint-actualPoint;
    //重新规划当前视图的位置
    this->setPosition(viewPoint);

}



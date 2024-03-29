#include "object_set.h"
#include "objects/explosion.h"
#include "objects/bomb.h"
#include "objects/breakable_block.h"
#include "objects/particle.h"
#include "objects/ring.h"
#include "objects/walker.h"
#include "objects/hover_pod.h"
#include "objects/dead_kukku.h"
#include "objects/pushable_object.h"
#include "error.h"
#include "tools.h"
#include "tinyxml2.h"
#include "hud.h"
#include "objects/transition.h"
#include "objects/bridge.h"
#include "objects/bird_walker.h"
#include "character.h"
#include "objects/bat_robot.h"
#include "objects/nezu.h"
#include "objects/flame.h"
#include "objects/fire.h"
#include "objects/item_box.h"
#include "objects/drill_mole.h"
#include "objects/moving_platform.h"
#include "objects/bomb_thrower.h"
#include "objects/rock_thrower.h"
#include "objects/jumper.h"
#include "objects/wind.h"
#include "objects/speedy.h"

TA_Object::TA_Object(TA_ObjectSet *newObjectSet)
{
    objectSet = newObjectSet;
    setCamera(objectSet->getLinks().camera);
}

void TA_Object::updatePosition()
{
    setPosition(position);
    hitbox.setPosition(position);
    for(auto & element : hitboxVector) {
        element.hitbox.setPosition(position);
    }
}

TA_Point TA_Object::getDistanceToCharacter()
{
    TA_Point characterPosition = objectSet->getCharacterPosition();
    TA_Point centeredPosition = position + TA_Point(getWidth() / 2, getHeight() / 2);
    return characterPosition - centeredPosition;
}

void TA_ObjectSet::load(std::string filename)
{
    tinyxml2::XMLDocument file;
    file.Parse(TA::readStringFromFile(filename).c_str());

    for(tinyxml2::XMLElement *element = file.FirstChildElement("objects")->FirstChildElement("object");
        element != nullptr; element = element->NextSiblingElement("object"))
    {
        std::string name = element->Attribute("name");
        if(element->IntAttribute("tile_x")) {
            element->SetAttribute("x", element->IntAttribute("tile_x") * 16);
        }
        if(element->IntAttribute("tile_y")) {
            element->SetAttribute("y", element->IntAttribute("tile_y") * 16);
        }

        if(name == "breakable_block") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            bool dropsRing = false;
            if(element->Attribute("drops_ring", "true")) {
                dropsRing = true;
            }
            std::string path = "maps/pf/pf_block.png", particlePath = "maps/pf/pf_rock.png";
            if(element->Attribute("path")) {
                path = element->Attribute("path");
            }
            if(element->Attribute("particle_path")) {
                particlePath = element->Attribute("particle_path");
            }
            spawnObject<TA_BreakableBlock>(path, particlePath, position, dropsRing);
        }

        else if(name == "walker" || name == "hover_pod") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            int range = 0;
            bool direction = true;
            if (element->Attribute("range")) {
                range = element->IntAttribute("range");
            }
            if (element->Attribute("direction", "right")) {
                direction = false;
            }
            if(name == "walker") {
                spawnObject<TA_Walker>(position, range, direction);
            }
            else {
                spawnObject<TA_HoverPod>(position, range, direction);
            }
        }

        else if(name == "pushable_rock" || name == "pushable_spring") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            if(name == "pushable_rock") {
                spawnObject<TA_PushableRock>(position);
            }
            else {
                spawnObject<TA_PushableSpring>(position);
            }
        }

        else if(name == "spawn_point") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            std::string currentLevelPath = "";
            if(element->Attribute("previous")) {
                currentLevelPath = element->Attribute("previous");
            }
            if(!firstSpawnPointSet || currentLevelPath == TA::previousLevelPath) {
                spawnPoint = position;
                if(element->Attribute("direction")) {
                    spawnFlip = element->Attribute("direction", "left");
                }
                else {
                    spawnFlip = false;
                }
                firstSpawnPointSet = true;
            }
        }

        else if(name == "level_transition") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            std::string levelPath = element->Attribute("path");
            spawnObject<TA_Transition>(topLeft, bottomRight, levelPath);
        }

        else if(name == "map_transition") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            int selection = element->IntAttribute("selection");
            spawnObject<TA_Transition>(topLeft, bottomRight, selection);
        }

        else if(name == "bridge") {
            std::string path = element->Attribute("path");
            std::string particlePath = element->Attribute("particle_path");
            int left = element->IntAttribute("leftx");
            int right = element->IntAttribute("rightx");
            int y = element->IntAttribute("y");
            for(int x = left; x <= right; x += 16) {
                spawnObject<TA_Bridge>(TA_Point(x, y), path, particlePath);
            }
        }

        else if(name == "camera_lock_point") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            links.camera->setLockPosition(position);
        }

        else if(name == "bird_walker") {
            double floorY = element->IntAttribute("floor_y");
            spawnObject<TA_BirdWalker>(floorY);
        }

        else if(name == "bat_robot") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_BatRobot>(position);
        }

        else if(name == "sound") {
            if(element->Attribute("begin")) {
                setMusic(element->Attribute("begin"), element->Attribute("loop"));
            }
            else {
                setMusic("", element->Attribute("loop"));
            }
        }

        else if(name == "nezu") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_Nezu>(position);
        }

        else if(name == "flame") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            if(element->Attribute("speed")) {
                double speed = element->DoubleAttribute("speed");
                spawnObject<TA_FlameLauncher>(position, speed);
            }
            else {
                spawnObject<TA_FlameLauncher>(position);
            }
        }

        else if(name == "fire") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            bool direction = false;
            if(element->Attribute("direction", "right")) {
                direction = true;
            }
            spawnObject<TA_Fire>(position, direction);
        }

        else if(name == "item_box") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            int number = element->IntAttribute("number");
            std::string itemName = element->Attribute("item_name");
            spawnObject<TA_ItemBox>(position, number, itemName);
        }

        else if(name == "drill_mole") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_DrillMole>(position);
        }

        else if(name == "moving_platform") {
            TA_Point startPosition(element->IntAttribute("start_x"), element->IntAttribute("start_y"));
            TA_Point endPosition(element->IntAttribute("end_x"), element->IntAttribute("end_y"));
            if(element->Attribute("idle", "false")) {
                spawnObject<TA_MovingPlatform>(startPosition, endPosition, false);
            }
            else {
                spawnObject<TA_MovingPlatform>(startPosition, endPosition, true);
            }
        }

        else if(name == "bomb_thrower") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            double leftX = element->IntAttribute("left_x"), rightX = element->IntAttribute("right_x");
            spawnObject<TA_BombThrower>(position, leftX, rightX);
        }

        else if(name == "rock_thrower") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            bool direction = element->Attribute("direction", "right");
            spawnObject<TA_RockThrower>(position, direction);
        }

        else if(name == "jumper") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_Jumper>(position);
        }

        else if(name == "wind") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            TA_Point velocity(element->DoubleAttribute("xsp"), element->DoubleAttribute("ysp"));
            spawnObject<TA_Wind>(topLeft, bottomRight, velocity);
        }
        
        else if(name == "strong_wind") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            spawnObject<TA_StrongWind>(topLeft, bottomRight);
        }

        else if(name == "speedy") {
            spawnObject<TA_Speedy>();
        }

        else {
            TA::printWarning("Unknown object %s", name.c_str());
        }
    }

    bossBeginSound.load("sound/boss_begin.ogg", TA_SOUND_CHANNEL_MUSIC);
    bossLoopSound.load("sound/boss_loop.ogg", TA_SOUND_CHANNEL_MUSIC, true);
}

void TA_ObjectSet::update()
{
    for(TA_Object *currentObject : deleteList) {
        delete currentObject;
    }
    for(TA_Object *currentObject : spawnedObjects) {
        objects.push_back(currentObject);
    }
    deleteList.clear();
    spawnedObjects.clear();

    hitboxContainer.clear();
    for(TA_Object *currentObject : objects) {
        hitboxContainer.add(currentObject->hitbox, currentObject->getCollisionType());
        for(TA_Object::HitboxVectorElement &element : currentObject->hitboxVector) {
            hitboxContainer.add(element.hitbox, element.collisionType);
        }
    }

    std::vector<TA_Object*> newObjects;
    for(TA_Object *currentObject : objects) {
        if(currentObject->update()) {
            newObjects.push_back(currentObject);
        }
        else {
            deleteList.push_back(currentObject);
        }
    }
    objects = newObjects;
}

void TA_ObjectSet::draw(int priority)
{
    for(TA_Object *currentObject : objects) {
        if(currentObject->getDrawPriority() == priority) {
            currentObject->setUpdateAnimation(!isPaused());
            currentObject->draw();
        }
    }
}

void TA_ObjectSet::updateMusic()
{
    if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_MUSIC)) {
        if(bossMusic) {
            bossLoopSound.play();
        }
        else {
            areaLoopSound.play();
        }
    }
}

void TA_ObjectSet::checkCollision(TA_Polygon &hitbox, int &flags, int halfSolidTop)
{
    if(hitbox.empty()) {
        return;
    }
    flags = links.tilemap->checkCollision(hitbox, halfSolidTop);
    flags |= hitboxContainer.getCollisionFlags(hitbox);
    if(links.character->getHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
    }
    if(links.character->isUsingHammer() && links.character->getHammerHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_HAMMER;
    }
}

int TA_ObjectSet::checkCollision(TA_Polygon &hitbox)
{
    int flags = 0;
    checkCollision(hitbox, flags);
    return flags;
}

TA_Point TA_ObjectSet::getCharacterPosition()
{
    return links.character->getPosition() + TA_Point(24, 24);
}

void TA_ObjectSet::playAreaMusic()
{
    bossMusic = false;
    if(areaBeginSound.empty()) {
        areaLoopSound.play();
    }
    else {
        areaBeginSound.play();
    }
}

void TA_ObjectSet::playBossMusic()
{
    bossMusic = true;
    bossBeginSound.play();
}

void TA_ObjectSet::setMusic(std::string begin, std::string loop)
{
    areaBeginSound.clear();
    if(begin != "") {
        areaBeginSound.load(begin, TA_SOUND_CHANNEL_MUSIC);
    }
    areaLoopSound.load(loop, TA_SOUND_CHANNEL_MUSIC, true);
    loopMusicPath = loop;
    if(!bossMusic) {
        playAreaMusic();
    }
}

TA_ObjectSet::~TA_ObjectSet()
{
    for(TA_Object *currentObject : objects) {
        delete currentObject;
    }
}

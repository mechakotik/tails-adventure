#include "object_set.h"
#include <toml.hpp>
#include "character.h"
#include "error.h"
#include "objects/bat_robot.h"
#include "objects/beehive.h"
#include "objects/bird_walker.h"
#include "objects/bomb_thrower.h"
#include "objects/bomber.h"
#include "objects/breakable_block.h"
#include "objects/bridge.h"
#include "objects/conveyor_belt.h"
#include "objects/cruiser.h"
#include "objects/drill_mole.h"
#include "objects/enemy_mine.h"
#include "objects/fire.h"
#include "objects/flame.h"
#include "objects/grass_block.h"
#include "objects/hover_pod.h"
#include "objects/item_box.h"
#include "objects/jumper.h"
#include "objects/little_kukku.h"
#include "objects/mecha_golem.h"
#include "objects/mine.h"
#include "objects/mine_launcher.h"
#include "objects/mini_sub.h"
#include "objects/moving_platform.h"
#include "objects/nezu.h"
#include "objects/pushable_object.h"
#include "objects/ring.h"
#include "objects/rock_thrower.h"
#include "objects/speedy.h"
#include "objects/transition.h"
#include "objects/walker.h"
#include "objects/wind.h"
#include "objects/wood.h"
#include "resource_manager.h"
#include "save.h"
#include "sea_fox.h"

inline float asIntOrFloat(const toml::value& value) {
    if(value.is_floating()) {
        return value.as_floating();
    }
    return value.as_integer();
}

TA_Object::TA_Object(TA_ObjectSet* newObjectSet) {
    objectSet = newObjectSet;
    setCamera(objectSet->getLinks().camera);
}

void TA_Object::updatePosition() {
    setPosition(position);
    hitbox.setPosition(position);
    for(auto& element : hitboxVector) {
        element.hitbox.setPosition(position);
    }
}

TA_Point TA_Object::getDistanceToCharacter() {
    TA_Point characterPosition = objectSet->getCharacterPosition();
    TA_Point centeredPosition = position + TA_Point(getWidth() / 2, getHeight() / 2);
    return characterPosition - centeredPosition;
}

void TA_ObjectSet::load(std::string filename) {
    try {
        tryLoad(filename);
    } catch(std::exception& e) {
        TA::handleError("%s load failed\n%s", filename.c_str(), e.what());
    }
}

void TA_ObjectSet::tryLoad(std::string filename) {
    const toml::value& table = TA::resmgr::loadToml(filename);
    if(table.contains("level") && table.at("level").contains("music")) {
        TA::sound::playMusic(table.at("level").at("music").as_string());
    }

    if(table.contains("level") && table.at("level").contains("spawn")) {
        for(const auto spawn : table.at("level").at("spawn").as_array()) {
            TA_Point position(spawn.at("x").as_integer(), spawn.at("y").as_integer());
            std::string currentLevelPath = "";
            if(spawn.contains("previous")) {
                currentLevelPath = spawn.at("previous").as_string();
            }
            if(!firstSpawnPointSet || currentLevelPath == TA::previousLevelPath) {
                spawnPoint = position;
                spawnFlip = spawn.contains("flip") && spawn.at("flip").as_boolean();
                firstSpawnPointSet = true;
            }
        }
    }

    if(table.contains("level") && table.at("level").contains("water_level") && links.seaFox != nullptr) {
        waterLevel = table.at("level").at("water_level").as_integer();
    }

    if(table.contains("level") && table.at("level").contains("borders")) {
        std::array<std::string, 4> borders = {"top", "bottom", "left", "right"};
        int mask = 0;
        for(int i = 0; i < 4; i++) {
            if(table.at("level").at("borders").at(borders[i]).as_boolean()) {
                mask |= (1 << i);
            }
        }
        links.tilemap->setBorderMask(mask);
    }

    links.tilemap->updateBorders();

    if(table.contains("level") && table.at("level").contains("camera_borders")) {
        std::array<std::string, 4> borders = {"top", "bottom", "left", "right"};
        int mask = 0;
        for(int i = 0; i < 4; i++) {
            if(table.at("level").at("camera_borders").at(borders[i]).as_boolean()) {
                mask |= (1 << i);
            }
        }
        links.camera->setBorderMask(mask);
    }

    if(table.contains("level") && table.at("level").contains("night")) {
        night = table.at("level").at("night").as_boolean();
    }

    if(table.contains("objects") && table.at("objects").contains("static")) {
        for(const auto& [name, array] : table.at("objects").at("static").as_table()) {
            for(const auto& object : array.as_array()) {
                loadObject(name, object);
            }
        }
    }
    if(table.contains("objects") && table.at("objects").contains("default")) {
        for(const auto& [name, array] : table.at("objects").at("default").as_table()) {
            for(const auto& object : array.as_array()) {
                loadObject(name, object);
            }
        }
    }
}

void TA_ObjectSet::loadObject(std::string name, toml::value object) {
    TA_Point position{0, 0};
    if(object.contains("tile_x")) {
        position.x = static_cast<int>(object.at("tile_x").as_integer()) * 16;
    } else if(object.contains("x")) {
        position.x = static_cast<int>(object.at("x").as_integer());
    }
    if(object.contains("tile_y")) {
        position.y = static_cast<int>(object.at("tile_y").as_integer()) * 16;
    } else if(object.contains("y")) {
        position.y = static_cast<int>(object.at("y").as_integer());
    }
    if(object.contains("offset_x")) {
        position.x += static_cast<int>(object.at("offset_x").as_integer());
    }
    if(object.contains("offset_y")) {
        position.y += static_cast<int>(object.at("offset_y").as_integer());
    }

    if(name == "breakable_block") {
        bool dropsRing = object.contains("drops_ring") && object.at("drops_ring").as_boolean();
        bool strong = object.contains("strong") && object.at("strong").as_boolean();
        std::string path = "maps/pf/pf_block.png";
        std::string particlePath = "maps/pf/pf_rock.png";
        if(object.contains("path")) {
            path = object.at("path").as_string();
        }
        if(object.contains("particle_path")) {
            particlePath = object.at("particle_path").as_string();
        }
        spawnObject<TA_BreakableBlock>(path, particlePath, position, dropsRing, strong);
    }

    else if(name == "walker" || name == "hover_pod") {
        int range = 0;
        bool direction = true;
        if(object.contains("range")) {
            range = object.at("range").as_integer();
        }
        if(object.contains("flip") && object.at("flip").as_boolean()) {
            direction = false;
        }
        if(name == "walker") {
            spawnObject<TA_Walker>(position, range, direction);
        } else {
            spawnObject<TA_HoverPod>(position, range, direction);
        }
    }

    else if(name == "pushable_rock" || name == "pushable_spring") {
        if(name == "pushable_rock") {
            spawnObject<TA_PushableRock>(position);
        } else {
            spawnObject<TA_PushableSpring>(position);
        }
    }

    else if(name == "level_transition") {
        TA_Point topLeft(object.at("left").as_integer(), object.at("top").as_integer());
        TA_Point bottomRight(object.at("right").as_integer(), object.at("bottom").as_integer());
        std::string levelPath = object.at("path").as_string();
        spawnObject<TA_Transition>(topLeft, bottomRight, levelPath);
    }

    else if(name == "map_transition") {
        TA_Point topLeft(object.at("left").as_integer(), object.at("top").as_integer());
        TA_Point bottomRight(object.at("right").as_integer(), object.at("bottom").as_integer());
        int selection = object.at("selection").as_integer();
        bool seaFox = object.contains("seafox") && object.at("seafox").as_boolean();
        spawnObject<TA_Transition>(topLeft, bottomRight, selection, seaFox);
    }

    else if(name == "wind") {
        TA_Point topLeft(object.at("left").as_integer(), object.at("top").as_integer());
        TA_Point bottomRight(object.at("right").as_integer(), object.at("bottom").as_integer());
        TA_Point velocity(asIntOrFloat(object.at("xsp")), asIntOrFloat(object.at("ysp")));
        std::string animation = "leaf";
        if(object.contains("animation")) {
            animation = object.at("animation").as_string();
        }
        spawnObject<TA_Wind>(topLeft, bottomRight, velocity, animation);
    }

    else if(name == "item_box") {
        int number = object.at("number").as_integer();
        std::string itemName = object.at("item_name").as_string();
        spawnObject<TA_ItemBox>(position, TA_Point(0, 0), number, itemName);
    }

    else if(name == "grass_block") {
        std::string path = object.at("path").as_string();
        spawnObject<TA_GrassBlock>(position, path);
    }

    else if(name == "ring") {
        auto* ring = new TA_Ring(this);
        ring->loadStationary(position);
        spawnedObjects.push_back(ring);
    }

    else if(name == "bridge") {
        std::string path = object.at("path").as_string();
        std::string particlePath = object.at("particle_path").as_string();
        int left = object.at("leftx").as_integer();
        int right = object.at("rightx").as_integer();
        int y = object.at("y").as_integer();
        for(int x = left; x <= right; x += 16) {
            spawnObject<TA_Bridge>(TA_Point(x, y), path, particlePath);
        }
    }

    else if(name == "camera_lock_point") {
        links.camera->setLockPosition(position);
    }

    else if(name == "bird_walker") {
        int floorY = object.at("floor_y").as_integer();
        spawnObject<TA_BirdWalker>(floorY);
    }

    else if(name == "bat_robot") {
        spawnObject<TA_BatRobot>(position);
    }

    else if(name == "nezu") {
        spawnObject<TA_Nezu>(position);
    }

    else if(name == "flame") {
        if(object.contains("speed")) {
            float speed = asIntOrFloat(object.at("speed"));
            spawnObject<TA_FlameLauncher>(position, speed);
        } else {
            spawnObject<TA_FlameLauncher>(position);
        }
    }

    else if(name == "fire") {
        bool flip = object.contains("flip") && object.at("flip").as_boolean();
        spawnObject<TA_Fire>(position, flip);
    }

    else if(name == "drill_mole") {
        spawnObject<TA_DrillMole>(position);
    }

    else if(name == "moving_platform") {
        TA_Point startPosition(object.at("start_x").as_integer(), object.at("start_y").as_integer());
        TA_Point endPosition(object.at("end_x").as_integer(), object.at("end_y").as_integer());
        if(object.contains("idle") && !object.at("idle").as_boolean()) {
            spawnObject<TA_MovingPlatform>(startPosition, endPosition, false);
        } else {
            spawnObject<TA_MovingPlatform>(startPosition, endPosition, true);
        }
    }

    else if(name == "bomb_thrower") {
        float leftX = object.at("left_x").as_integer();
        float rightX = object.at("right_x").as_integer();
        spawnObject<TA_BombThrower>(position, leftX, rightX);
    }

    else if(name == "rock_thrower") {
        bool flip = object.contains("flip") && object.at("flip").as_boolean();
        spawnObject<TA_RockThrower>(position, flip);
    }

    else if(name == "jumper") {
        spawnObject<TA_Jumper>(position);
    }

    else if(name == "strong_wind") {
        TA_Point topLeft(object.at("left").as_integer(), object.at("top").as_integer());
        TA_Point bottomRight(object.at("right").as_integer(), object.at("bottom").as_integer());
        spawnObject<TA_StrongWind>(topLeft, bottomRight);
    }

    else if(name == "speedy") {
        spawnObject<TA_Speedy>();
    }

    else if(name == "mecha_golem") {
        spawnObject<TA_MechaGolem>();
    }

    else if(name == "mini_sub") {
        spawnObject<TA_MiniSub>(position);
    }

    else if(name == "mine") {
        spawnObject<TA_EnemyMine>(position, false);
    }

    else if(name == "conveyor_belt") {
        TA_Point topLeft(object.at("left").as_integer(), object.at("top").as_integer());
        TA_Point bottomRight(object.at("right").as_integer(), object.at("bottom").as_integer());
        bool flip = object.contains("flip") && object.at("flip").as_boolean();
        spawnObject<TA_ConveyorBelt>(topLeft, bottomRight, flip);
    }

    else if(name == "beehive") {
        spawnObject<TA_BeeHive>(position);
    }

    else if(name == "little_kukku") {
        spawnObject<TA_LittleKukku>(position);
    }

    else if(name == "cruiser") {
        spawnObject<TA_Cruiser>();
    }

    else if(name == "wood") {
        spawnObject<TA_Wood>(position);
    }

    else if(name == "bomber") {
        float maxY = (object.contains("max_y") ? static_cast<float>(object.at("max_y").as_integer()) : 1e5F);
        spawnObject<TA_Bomber>(position.x, maxY);
    }

    else if(name == "mine_launcher") {
        spawnObject<TA_MineLauncher>(position);
    }

    else {
        TA::handleError("unknown object %s", name.c_str());
    }
}

void TA_ObjectSet::update() {
    for(TA_Object* currentObject : deleteList) {
        delete currentObject;
    }
    for(TA_Object* currentObject : spawnedObjects) {
        objects.push_back(currentObject);
    }
    deleteList.clear();
    spawnedObjects.clear();

    hitboxContainer.clear();
    for(TA_Object* currentObject : objects) {
        hitboxContainer.add(currentObject->hitbox, currentObject->collisionType);
        for(TA_Object::HitboxVectorElement& element : currentObject->hitboxVector) {
            hitboxContainer.add(element.hitbox, element.collisionType);
        }
    }

    std::vector<TA_Object*> newObjects;
    for(TA_Object* currentObject : objects) {
        if(currentObject->update()) {
            newObjects.push_back(currentObject);
        } else {
            deleteList.push_back(currentObject);
        }
    }
    objects = newObjects;
}

void TA_ObjectSet::draw(int priority) {
    if(night && !links.character->isNightVisionApplied()) {
        return;
    }
    for(TA_Object* currentObject : objects) {
        if(currentObject->getDrawPriority() == priority) {
            currentObject->setUpdateAnimation(!isPaused());
            currentObject->draw();
        }
    }
}

void TA_ObjectSet::checkCollision(TA_Rect& hitbox, int& flags) {
    flags = links.tilemap->checkCollision(hitbox);
    flags |= hitboxContainer.getCollisionFlags(hitbox);

    if(links.character && links.character->getHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
    } else if(links.seaFox && links.seaFox->getHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
    }

    if(links.character && links.character->isUsingHammer() && links.character->getHammerHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_ATTACK;
    }
    if(links.seaFox && links.seaFox->getDrillHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_DRILL;
    }
}

int TA_ObjectSet::checkCollision(TA_Rect& hitbox) {
    int flags = 0;
    checkCollision(hitbox, flags);
    return flags;
}

TA_Point TA_ObjectSet::getCharacterPosition() {
    if(links.character) {
        return links.character->getPosition() + TA_Point(24, 24);
    }
    return links.seaFox->getPosition() + TA_Point(16, 16);
}

void TA_ObjectSet::addRings(int count) {
    int rings = TA::save::getSaveParameter("rings");
    rings += count;
    rings = std::min(rings, getMaxRings());
    TA::save::setSaveParameter("rings", rings);
}

void TA_ObjectSet::addRingsToMaximum() {
    TA::save::setSaveParameter("rings", getMaxRings());
}

int TA_ObjectSet::getEmeraldsCount() {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    int count = 0;
    for(int item = 29; item <= 34; item++) {
        if(itemMask & (1ll << item)) {
            count++;
        }
    }
    return count;
}

bool TA_ObjectSet::isVisible(TA_Rect& hitbox) {
    TA_Point cameraPosition = links.camera->getPosition();
    TA_Rect cameraRect;
    cameraRect.setRectangle(
        cameraPosition - TA_Point(5, 5), cameraPosition + TA_Point(TA::screenWidth + 5, TA::screenHeight + 5));
    return cameraRect.intersects(hitbox);
}

bool TA_ObjectSet::enemyShouldDropRing() {
    if(links.character != nullptr) {
        for(int i = 0; i < 4; i++) {
            if(TA::save::getSaveParameter("item_slot" + std::to_string(i)) == 9) {
                return TA::random::next() % 2 == 0;
            }
        }
    }
    return TA::random::next() % 4 == 0;
}

TA_ObjectSet::~TA_ObjectSet() {
    for(TA_Object* currentObject : objects) {
        delete currentObject;
    }
}

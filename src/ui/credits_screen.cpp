#include "credits_screen.h"
#include "filesystem.h"
#include "resource_manager.h"
#include "screen.h"
#include "sound.h"
#include "tools.h"

namespace {
    std::vector<uint8_t> parseSequence(const std::filesystem::path& path) {
        std::string data = TA::filesystem::readAsset(path);
        std::vector<uint8_t> res;
        for(int i = 0; i + 1 < data.size(); i += 2) {
            for(int j = 0; j < static_cast<uint8_t>(data[i + 1]); j++) {
                res.push_back(static_cast<uint8_t>(data[i]));
            }
        }
        return res;
    }

    uint8_t getValue(const std::vector<uint8_t>& seq, float time) {
        int pos = std::min(static_cast<int>(time), static_cast<int>(seq.size()) - 1);
        return seq[pos];
    }

    float getFloatValue(const std::vector<uint8_t>& seq, float time) {
        return static_cast<float>(getValue(seq, time));
    }

    float getInterpolatedValue(const std::vector<uint8_t>& seq, float time) {
        int n = static_cast<int>(seq.size());
        int i1 = std::clamp(static_cast<int>(time), 0, n - 1);
        int i0 = std::max(0, i1 - 1);
        int i2 = std::min(n - 1, i1 + 1);
        int i3 = std::min(n - 1, i1 + 2);

        auto p0 = static_cast<float>(seq[i0]);
        auto p1 = static_cast<float>(seq[i1]);
        auto p2 = static_cast<float>(seq[i2]);
        auto p3 = static_cast<float>(seq[i3]);
        float t = time - std::floor(time);

        return 0.5F * (2.0F * p1 + (-p0 + p2) * t + (2.0F * p0 - 5.0F * p1 + 4.0F * p2 - p3) * t * t +
                          (-p0 + 3.0F * p1 - 3.0F * p2 + p3) * t * t * t);
    }
} // namespace

void TA_CreditsScreen::init() {
    background.load("credits/background.png");
    tails.loadFromToml("tails/tails.toml");
    bird.loadFromToml("credits/bird.toml");
    robot.loadFromToml("credits/robot.toml");
    font.loadFont("fonts/item.toml");

    tailsFrame = parseSequence("credits/tails_frame.seq");
    tailsX = parseSequence("credits/tails_xpos.seq");
    tailsY = parseSequence("credits/tails_ypos.seq");
    tailsFlip = parseSequence("credits/tails_flip.seq");
    birdFrame = parseSequence("credits/bird_frame.seq");
    birdX = parseSequence("credits/bird_xpos.seq");
    birdY = parseSequence("credits/bird_ypos.seq");
    birdFlip = parseSequence("credits/bird_flip.seq");
    robotFrame = parseSequence("credits/robot_frame.seq");

    for(auto& explosion : explosions) {
        explosion.loadFromToml("objects/explosion.toml");
    }

    smallExplosionSound.load("sound/explosion_small.ogg", TA_SOUND_CHANNEL_SFX3);
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);

    loadCredits();
    TA::sound::playMusic("sound/credits.vgm");
}

void TA_CreditsScreen::loadCredits() {
    auto toml = TA::resmgr::loadToml("credits/credits.toml");
    for(auto& rawElement : toml.at("credits").as_array()) {
        CreditsElement element;
        if(rawElement.contains("line")) {
            element.lines.push_back(rawElement.at("line").as_string());
        }
        if(rawElement.contains("lines")) {
            for(auto& line : rawElement.at("lines").as_array()) {
                element.lines.push_back(line.as_string());
            }
        }
        if(rawElement.contains("head") && rawElement.at("head").as_boolean()) {
            element.head = true;
        }
        if(rawElement.contains("sides") && rawElement.at("sides").as_boolean()) {
            element.sides = true;
        }
        credits.push_back(element);
    }
}

TA_ScreenState TA_CreditsScreen::update() {
    timer += TA::elapsedTime;
    TA_Point offset{(TA::screenWidth / 2) - 80, (TA::screenHeight / 2) - 72};

    background.setPosition(offset);
    background.draw();

    robot.setPosition(offset + TA_Point(76, 45));
    robot.setFrame(static_cast<int>(getValue(robotFrame, timer)));
    int robotFrame = robot.getCurrentFrame();

    if(robotFrame != prevRobotFrame) {
        if(robotFrame == 2) {
            explosionMinX = 80;
            explosionMaxX = 80;
            explosionMinY = 54;
            explosionMaxY = 54;
            explosionCount = 1;
            smallExplosionSound.play();
        } else if(robotFrame == 5) {
            explosionMinX = 86;
            explosionMaxX = 96;
            explosionMinY = 46;
            explosionMaxY = 64;
            explosionDelay = 8;
            explosionCount = 8;
            explosionSound.play();
        } else if(robotFrame == 7) {
            explosionMinX = 76;
            explosionMaxX = 96;
            explosionMinY = 46;
            explosionMaxY = 64;
            explosionDelay = 4;
            explosionCount = 16;
            explosionSound.play();
        }
        explosionTimer = 0;
        prevRobotFrame = robotFrame;
    }

    if(robotFrame != 7 || explosionCount != 0) {
        robot.draw();
    }

    if(explosionCount >= 1) {
        explosionTimer -= TA::elapsedTime;
        if(explosionTimer <= 0) {
            for(auto& explosion : explosions) {
                if(!explosion.isAnimated()) {
                    TA_Point pos{
                        explosionMinX + (TA::random::next() % (explosionMaxX - explosionMinX + 1)),
                        explosionMinY + (TA::random::next() % (explosionMaxY - explosionMinY + 1)),
                    };
                    explosion.setPosition(offset + pos);
                    explosion.setAnimation("explosion");
                    break;
                }
            }
            explosionTimer += explosionDelay;
            explosionCount--;
        }
    }

    TA_Point tailsPos = offset;
    tailsPos.x += getInterpolatedValue(tailsX, timer);
    tailsPos.y += getInterpolatedValue(tailsY, timer);
    tailsPos.x -= 68;
    tailsPos.y -= 63;

    tails.setPosition(tailsPos);
    tails.setFrame(getValue(tailsFrame, timer));
    tails.setFlip(getValue(tailsFlip, timer) != 0);
    tails.draw();

    TA_Point birdPos = offset;
    birdPos.x += getInterpolatedValue(birdX, timer);
    birdPos.y += getInterpolatedValue(birdY, timer);
    birdPos.x -= 54;
    birdPos.y -= 39;

    bird.setPosition(birdPos);
    bird.setFrame(getValue(birdFrame, timer));
    bird.setFlip(getValue(birdFlip, timer) != 2);
    bird.draw();

    for(auto& explosion : explosions) {
        if(explosion.isAnimated()) {
            explosion.draw();
        }
    }

    TA::drawRect({0, 0}, {offset.x, TA::screenHeight}, 0, 0, 0, 255);
    TA::drawRect({TA::screenWidth - offset.x, 0}, {TA::screenWidth, TA::screenHeight}, 0, 0, 0, 255);

    float fontPeriod = 9800.0F / static_cast<float>(credits.size());
    float fontTimer = std::fmod(std::min(timer, 9760.0F), fontPeriod);
    if(fontTimer < fontPeriod * 0.1F) {
        font.setAlpha(0);
    } else if(fontTimer < fontPeriod * 0.2F) {
        font.setAlpha(static_cast<int>(255.0F * (fontTimer - fontPeriod * 0.1F) / (fontPeriod * 0.1F)));
    } else if(fontTimer < fontPeriod * 0.9F) {
        font.setAlpha(255);
    } else {
        font.setAlpha(255 - static_cast<int>(255.0F * (fontTimer - fontPeriod * 0.9F) / (fontPeriod * 0.1F)));
    }
    int creditsPos = static_cast<int>(timer / fontPeriod);
    creditsPos = std::max(0, std::min(static_cast<int>(credits.size()) - 1, creditsPos));

    if(credits[creditsPos].lines.size() == 1) {
        float y = (credits[creditsPos].head ? 104.0F : 112.0F);
        font.drawTextCentered(offset.y + y, credits[creditsPos].lines[0]);
    } else {
        if(credits[creditsPos].sides) {
            font.drawText(offset + TA_Point(25, 104), credits[creditsPos].lines[0]);
            font.drawText(offset + TA_Point(120 - font.getTextWidth(credits[creditsPos].lines[1]), 120),
                credits[creditsPos].lines[1]);
        } else {
            font.drawTextCentered(offset.y + 104, credits[creditsPos].lines[0]);
            font.drawTextCentered(offset.y + 120, credits[creditsPos].lines[1]);
        }
    }

    if(timer >= static_cast<float>(tailsFrame.size())) {
        return TA_SCREENSTATE_TITLE;
    }
    return TA_SCREENSTATE_CURRENT;
}

/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

// ideas taken from:
//     . The ocean spray in your face [Jeff Lander]
//        http://www.double.co.nz/dust/col0798.pdf
//     . Building an Advanced Particle System [John van der Burg]
//        http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//        http://love2d.org/
//
//
// Radius mode support, from 71 squared
//        http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical. 
//

#include "ParticleSystem.h"
#include "ParticleBatchNode.h"
#include "base/Types.h"
#include "textures/TextureCache.h"
#include "textures/TextureAtlas.h"
#include "support/base64.h"
#include "support/PointExtension.h"
#include "platform/FileUtils.h"
#include "platform/Image.h"
#include "platform/platform.h"
#include "support/zip_support/ZipUtils.h"
#include "base/Director.h"
#include "support/Profiling.h"
// opengl
#include "GL.h"

#include <string>

using namespace std;


NS_AX_BEGIN

// ideas taken from:
//     . The ocean spray in your face [Jeff Lander]
//        http://www.double.co.nz/dust/col0798.pdf
//     . Building an Advanced Particle System [John van der Burg]
//        http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//        http://love2d.org/
//
//
// Radius mode support, from 71 squared
//        http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical. 
//

ParticleSystem::ParticleSystem()
: m_sPlistFile("")
, m_fElapsed(0)
, m_pParticles(NULL)
, m_fEmitCounter(0)
, m_uParticleIdx(0)
, m_pBatchNode(NULL)
, m_uAtlasIndex(0)
, m_bTransformSystemDirty(false)
, m_uAllocatedParticles(0)
, m_bIsActive(true)
, m_uParticleCount(0)
, m_fDuration(0)
, m_tSourcePosition(Vec2::ZERO)
, m_tPosVar(Vec2::ZERO)
, m_fLife(0)
, m_fLifeVar(0)
, m_fAngle(0)
, m_fAngleVar(0)
, m_fStartSize(0)
, m_fStartSizeVar(0)
, m_fEndSize(0)
, m_fEndSizeVar(0)
, m_fStartSpin(0)
, m_fStartSpinVar(0)
, m_fEndSpin(0)
, m_fEndSpinVar(0)
, m_fEmissionRate(0)
, m_uTotalParticles(0)
, m_pTexture(NULL)
, m_bOpacityModifyRGB(false)
, m_bIsBlendAdditive(false)
, m_ePositionType(kCCPositionTypeFree)
, m_bIsAutoRemoveOnFinish(false)
, m_nEmitterMode(kCCParticleModeGravity)
{
    modeA.gravity = Vec2::ZERO;
    modeA.speed = 0;
    modeA.speedVar = 0;
    modeA.tangentialAccel = 0;
    modeA.tangentialAccelVar = 0;
    modeA.radialAccel = 0;
    modeA.radialAccelVar = 0;
    modeA.rotationIsDir = false;
    modeB.startRadius = 0;
    modeB.startRadiusVar = 0;
    modeB.endRadius = 0;
    modeB.endRadiusVar = 0;            
    modeB.rotatePerSecond = 0;
    modeB.rotatePerSecondVar = 0;
    m_tBlendFunc.src = AX_BLEND_SRC;
    m_tBlendFunc.dst = AX_BLEND_DST;
}
// implementation ParticleSystem

ParticleSystem * ParticleSystem::create(const char *plistFile)
{
    ParticleSystem *pRet = new ParticleSystem();
    if (pRet && pRet->initWithFile(plistFile))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return pRet;
}

ParticleSystem* ParticleSystem::createWithTotalParticles(unsigned int numberOfParticles)
{
    ParticleSystem *pRet = new ParticleSystem();
    if (pRet && pRet->initWithTotalParticles(numberOfParticles))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return pRet;
}

bool ParticleSystem::init()
{
    return initWithTotalParticles(150);
}

bool ParticleSystem::initWithFile(const char *plistFile)
{
    bool bRet = false;
    m_sPlistFile = FileUtils::sharedFileUtils()->fullPathForFilename(plistFile);
    Dictionary *dict = Dictionary::createWithContentsOfFileThreadSafe(m_sPlistFile.c_str());

    AXAssert( dict != NULL, "Particles: file not found");
    
    // XXX compute path from a path, should define a function somewhere to do it
    string listFilePath = plistFile;
    if (listFilePath.find('/') != string::npos)
    {
        listFilePath = listFilePath.substr(0, listFilePath.rfind('/') + 1);
        bRet = this->initWithDictionary(dict, listFilePath.c_str());
    }
    else
    {
        bRet = this->initWithDictionary(dict, "");
    }
    
    dict->release();

    return bRet;
}

bool ParticleSystem::initWithDictionary(Dictionary *dictionary)
{
    return initWithDictionary(dictionary, "");
}

bool ParticleSystem::initWithDictionary(Dictionary *dictionary, const char *dirname)
{
    bool bRet = false;
    unsigned char *buffer = NULL;
    unsigned char *deflated = NULL;
    Image *image = NULL;
    do 
    {
        int maxParticles = dictionary->valueForKey("maxParticles")->intValue();
        // self, not super
        if(this->initWithTotalParticles(maxParticles))
        {
            // angle
            m_fAngle = dictionary->valueForKey("angle")->floatValue();
            m_fAngleVar = dictionary->valueForKey("angleVariance")->floatValue();

            // duration
            m_fDuration = dictionary->valueForKey("duration")->floatValue();

            // blend function 
            m_tBlendFunc.src = dictionary->valueForKey("blendFuncSource")->intValue();
            m_tBlendFunc.dst = dictionary->valueForKey("blendFuncDestination")->intValue();

            // color
            m_tStartColor.r = dictionary->valueForKey("startColorRed")->floatValue();
            m_tStartColor.g = dictionary->valueForKey("startColorGreen")->floatValue();
            m_tStartColor.b = dictionary->valueForKey("startColorBlue")->floatValue();
            m_tStartColor.a = dictionary->valueForKey("startColorAlpha")->floatValue();

            m_tStartColorVar.r = dictionary->valueForKey("startColorVarianceRed")->floatValue();
            m_tStartColorVar.g = dictionary->valueForKey("startColorVarianceGreen")->floatValue();
            m_tStartColorVar.b = dictionary->valueForKey("startColorVarianceBlue")->floatValue();
            m_tStartColorVar.a = dictionary->valueForKey("startColorVarianceAlpha")->floatValue();

            m_tEndColor.r = dictionary->valueForKey("finishColorRed")->floatValue();
            m_tEndColor.g = dictionary->valueForKey("finishColorGreen")->floatValue();
            m_tEndColor.b = dictionary->valueForKey("finishColorBlue")->floatValue();
            m_tEndColor.a = dictionary->valueForKey("finishColorAlpha")->floatValue();

            m_tEndColorVar.r = dictionary->valueForKey("finishColorVarianceRed")->floatValue();
            m_tEndColorVar.g = dictionary->valueForKey("finishColorVarianceGreen")->floatValue();
            m_tEndColorVar.b = dictionary->valueForKey("finishColorVarianceBlue")->floatValue();
            m_tEndColorVar.a = dictionary->valueForKey("finishColorVarianceAlpha")->floatValue();

            // particle size
            m_fStartSize = dictionary->valueForKey("startParticleSize")->floatValue();
            m_fStartSizeVar = dictionary->valueForKey("startParticleSizeVariance")->floatValue();
            m_fEndSize = dictionary->valueForKey("finishParticleSize")->floatValue();
            m_fEndSizeVar = dictionary->valueForKey("finishParticleSizeVariance")->floatValue();

            // position
            float x = dictionary->valueForKey("sourcePositionx")->floatValue();
            float y = dictionary->valueForKey("sourcePositiony")->floatValue();
            this->setPosition( Vec2(x,y) );            
            m_tPosVar.x = dictionary->valueForKey("sourcePositionVariancex")->floatValue();
            m_tPosVar.y = dictionary->valueForKey("sourcePositionVariancey")->floatValue();

            // Spinning
            m_fStartSpin = dictionary->valueForKey("rotationStart")->floatValue();
            m_fStartSpinVar = dictionary->valueForKey("rotationStartVariance")->floatValue();
            m_fEndSpin= dictionary->valueForKey("rotationEnd")->floatValue();
            m_fEndSpinVar= dictionary->valueForKey("rotationEndVariance")->floatValue();

            m_nEmitterMode = dictionary->valueForKey("emitterType")->intValue();

            // Mode A: Gravity + tangential accel + radial accel
            if( m_nEmitterMode == kCCParticleModeGravity ) 
            {
                // gravity
                modeA.gravity.x = dictionary->valueForKey("gravityx")->floatValue();
                modeA.gravity.y = dictionary->valueForKey("gravityy")->floatValue();

                // speed
                modeA.speed = dictionary->valueForKey("speed")->floatValue();
                modeA.speedVar = dictionary->valueForKey("speedVariance")->floatValue();

                // radial acceleration
                modeA.radialAccel = dictionary->valueForKey("radialAcceleration")->floatValue();
                modeA.radialAccelVar = dictionary->valueForKey("radialAccelVariance")->floatValue();

                // tangential acceleration
                modeA.tangentialAccel = dictionary->valueForKey("tangentialAcceleration")->floatValue();
                modeA.tangentialAccelVar = dictionary->valueForKey("tangentialAccelVariance")->floatValue();
                
                // rotation is dir
                modeA.rotationIsDir = dictionary->valueForKey("rotationIsDir")->boolValue();
            }

            // or Mode B: radius movement
            else if( m_nEmitterMode == kCCParticleModeRadius ) 
            {
                modeB.startRadius = dictionary->valueForKey("maxRadius")->floatValue();
                modeB.startRadiusVar = dictionary->valueForKey("maxRadiusVariance")->floatValue();
                modeB.endRadius = dictionary->valueForKey("minRadius")->floatValue();
                modeB.endRadiusVar = 0.0f;
                modeB.rotatePerSecond = dictionary->valueForKey("rotatePerSecond")->floatValue();
                modeB.rotatePerSecondVar = dictionary->valueForKey("rotatePerSecondVariance")->floatValue();

            } else {
                AXAssert( false, "Invalid emitterType in config file");
                AX_BREAK_IF(true);
            }

            // life span
            m_fLife = dictionary->valueForKey("particleLifespan")->floatValue();
            m_fLifeVar = dictionary->valueForKey("particleLifespanVariance")->floatValue();

            // emission Rate
            m_fEmissionRate = m_uTotalParticles / m_fLife;

            //don't get the internal texture if a batchNode is used
            if (!m_pBatchNode)
            {
                // Set a compatible default for the alpha transfer
                m_bOpacityModifyRGB = false;

                // texture        
                // Try to get the texture from the cache
                std::string textureName = dictionary->valueForKey("textureFileName")->getCString();
                
                size_t rPos = textureName.rfind('/');
               
                if (rPos != string::npos)
                {
                    string textureDir = textureName.substr(0, rPos + 1);
                    
                    if (dirname != NULL && textureDir != dirname)
                    {
                        textureName = textureName.substr(rPos+1);
                        textureName = string(dirname) + textureName;
                    }
                }
                else
                {
                    if (dirname != NULL)
                    {
                        textureName = string(dirname) + textureName;
                    }
                }
                
                Texture2D *tex = NULL;
                
                if (textureName.length() > 0)
                {
                    // set not pop-up message box when load image failed
                    bool bNotify = FileUtils::sharedFileUtils()->isPopupNotify();
                    FileUtils::sharedFileUtils()->setPopupNotify(false);
                    tex = TextureCache::sharedTextureCache()->addImage(textureName.c_str());
                    // reset the value of UIImage notify
                    FileUtils::sharedFileUtils()->setPopupNotify(bNotify);
                }
                
                if (tex)
                {
                    setTexture(tex);
                }
                else
                {                        
                    const char *textureData = dictionary->valueForKey("textureImageData")->getCString();
                    AXAssert(textureData, "");
                    AXLOG("%s", textureData);
                    
                    int dataLen = strlen(textureData);
                    if(dataLen != 0)
                    {
                        // if it fails, try to get it from the base64-gzipped data    
                        int decodeLen = base64Decode((unsigned char*)textureData, (unsigned int)dataLen, &buffer);
                        AXAssert( buffer != NULL, "ParticleSystem: error decoding textureImageData");
                        AX_BREAK_IF(!buffer);
                        
                        int deflatedLen = ZipUtils::ccInflateMemory(buffer, decodeLen, &deflated);
                        AXAssert( deflated != NULL, "ParticleSystem: error ungzipping textureImageData");
                        AX_BREAK_IF(!deflated);
                        
                        // For android, we should retain it in VolatileTexture::addCCImage which invoked in TextureCache::sharedTextureCache()->addUIImage()
                        image = new Image();
                        bool isOK = image->initWithImageData(deflated, deflatedLen);
                        AXAssert(isOK, "ParticleSystem: error init image with Data");
                        AX_BREAK_IF(!isOK);
                        
                        setTexture(TextureCache::sharedTextureCache()->addUIImage(image, textureName.c_str()));

                        image->release();
                    }
                }
                AXAssert( this->m_pTexture != NULL, "ParticleSystem: error loading the texture");
            }
            bRet = true;
        }
    } while (0);
    AX_SAFE_DELETE_ARRAY(buffer);
    AX_SAFE_DELETE_ARRAY(deflated);
    return bRet;
}

bool ParticleSystem::initWithTotalParticles(unsigned int numberOfParticles)
{
    m_uTotalParticles = numberOfParticles;

    AX_SAFE_FREE(m_pParticles);
    
    m_pParticles = (tCCParticle*)calloc(m_uTotalParticles, sizeof(tCCParticle));

    if( ! m_pParticles )
    {
        AXLOG("Particle system: not enough memory");
        this->release();
        return false;
    }
    m_uAllocatedParticles = numberOfParticles;

    if (m_pBatchNode)
    {
        for (unsigned int i = 0; i < m_uTotalParticles; i++)
        {
            m_pParticles[i].atlasIndex=i;
        }
    }
    // default, active
    m_bIsActive = true;

    // default blend function
    m_tBlendFunc.src = AX_BLEND_SRC;
    m_tBlendFunc.dst = AX_BLEND_DST;

    // default movement type;
    m_ePositionType = kCCPositionTypeFree;

    // by default be in mode A:
    m_nEmitterMode = kCCParticleModeGravity;

    // default: modulate
    // XXX: not used
    //    colorModulate = YES;

    m_bIsAutoRemoveOnFinish = false;

    // Optimization: compile updateParticle method
    //updateParticleSel = @selector(updateQuadWithParticle:newPosition:);
    //updateParticleImp = (AX_UPDATE_PARTICLE_IMP) [self methodForSelector:updateParticleSel];
    //for batchNode
    m_bTransformSystemDirty = false;
    // update after action in run!
    this->scheduleUpdateWithPriority(1);

    return true;
}

ParticleSystem::~ParticleSystem()
{
    // Since the scheduler retains the "target (in this case the ParticleSystem)
	// it is not needed to call "unscheduleUpdate" here. In fact, it will be called in "cleanup"
    //unscheduleUpdate();
    AX_SAFE_FREE(m_pParticles);
    AX_SAFE_RELEASE(m_pTexture);
}

bool ParticleSystem::addParticle()
{
    if (this->isFull())
    {
        return false;
    }

    tCCParticle * particle = &m_pParticles[ m_uParticleCount ];
    this->initParticle(particle);
    ++m_uParticleCount;

    return true;
}

void ParticleSystem::initParticle(tCCParticle* particle)
{
    // timeToLive
    // no negative life. prevent division by 0
    particle->timeToLive = m_fLife + m_fLifeVar * CCRANDOM_MINUS1_1();
    particle->timeToLive = MAX(0, particle->timeToLive);

    // position
    particle->pos.x = m_tSourcePosition.x + m_tPosVar.x * CCRANDOM_MINUS1_1();

    particle->pos.y = m_tSourcePosition.y + m_tPosVar.y * CCRANDOM_MINUS1_1();


    // Color
    ccColor4F start;
    start.r = clampf(m_tStartColor.r + m_tStartColorVar.r * CCRANDOM_MINUS1_1(), 0, 1);
    start.g = clampf(m_tStartColor.g + m_tStartColorVar.g * CCRANDOM_MINUS1_1(), 0, 1);
    start.b = clampf(m_tStartColor.b + m_tStartColorVar.b * CCRANDOM_MINUS1_1(), 0, 1);
    start.a = clampf(m_tStartColor.a + m_tStartColorVar.a * CCRANDOM_MINUS1_1(), 0, 1);

    ccColor4F end;
    end.r = clampf(m_tEndColor.r + m_tEndColorVar.r * CCRANDOM_MINUS1_1(), 0, 1);
    end.g = clampf(m_tEndColor.g + m_tEndColorVar.g * CCRANDOM_MINUS1_1(), 0, 1);
    end.b = clampf(m_tEndColor.b + m_tEndColorVar.b * CCRANDOM_MINUS1_1(), 0, 1);
    end.a = clampf(m_tEndColor.a + m_tEndColorVar.a * CCRANDOM_MINUS1_1(), 0, 1);

    particle->color = start;
    particle->deltaColor.r = (end.r - start.r) / particle->timeToLive;
    particle->deltaColor.g = (end.g - start.g) / particle->timeToLive;
    particle->deltaColor.b = (end.b - start.b) / particle->timeToLive;
    particle->deltaColor.a = (end.a - start.a) / particle->timeToLive;

    // size
    float startS = m_fStartSize + m_fStartSizeVar * CCRANDOM_MINUS1_1();
    startS = MAX(0, startS); // No negative value

    particle->size = startS;

    if( m_fEndSize == kCCParticleStartSizeEqualToEndSize )
    {
        particle->deltaSize = 0;
    }
    else
    {
        float endS = m_fEndSize + m_fEndSizeVar * CCRANDOM_MINUS1_1();
        endS = MAX(0, endS); // No negative values
        particle->deltaSize = (endS - startS) / particle->timeToLive;
    }

    // rotation
    float startA = m_fStartSpin + m_fStartSpinVar * CCRANDOM_MINUS1_1();
    float endA = m_fEndSpin + m_fEndSpinVar * CCRANDOM_MINUS1_1();
    particle->rotation = startA;
    particle->deltaRotation = (endA - startA) / particle->timeToLive;

    // position
    if( m_ePositionType == kCCPositionTypeFree )
    {
        particle->startPos = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if ( m_ePositionType == kCCPositionTypeRelative )
    {
        particle->startPos = m_obPosition;
    }

    // direction
    float a = AX_DEGREES_TO_RADIANS( m_fAngle + m_fAngleVar * CCRANDOM_MINUS1_1() );    

    // Mode Gravity: A
    if (m_nEmitterMode == kCCParticleModeGravity) 
    {
        Vec2 v(cosf( a ), sinf( a ));
        float s = modeA.speed + modeA.speedVar * CCRANDOM_MINUS1_1();

        // direction
        particle->modeA.dir = PointMult( v, s );

        // radial accel
        particle->modeA.radialAccel = modeA.radialAccel + modeA.radialAccelVar * CCRANDOM_MINUS1_1();
 

        // tangential accel
        particle->modeA.tangentialAccel = modeA.tangentialAccel + modeA.tangentialAccelVar * CCRANDOM_MINUS1_1();

        // rotation is dir
        if(modeA.rotationIsDir)
            particle->rotation = -AX_RADIANS_TO_DEGREES(PointToAngle(particle->modeA.dir));
    }

    // Mode Radius: B
    else 
    {
        // Set the default diameter of the particle from the source position
        float startRadius = modeB.startRadius + modeB.startRadiusVar * CCRANDOM_MINUS1_1();
        float endRadius = modeB.endRadius + modeB.endRadiusVar * CCRANDOM_MINUS1_1();

        particle->modeB.radius = startRadius;

        if(modeB.endRadius == kCCParticleStartRadiusEqualToEndRadius)
        {
            particle->modeB.deltaRadius = 0;
        }
        else
        {
            particle->modeB.deltaRadius = (endRadius - startRadius) / particle->timeToLive;
        }

        particle->modeB.angle = a;
        particle->modeB.degreesPerSecond = AX_DEGREES_TO_RADIANS(modeB.rotatePerSecond + modeB.rotatePerSecondVar * CCRANDOM_MINUS1_1());
    }    
}

void ParticleSystem::stopSystem()
{
    m_bIsActive = false;
    m_fElapsed = m_fDuration;
    m_fEmitCounter = 0;
}

void ParticleSystem::resetSystem()
{
    m_bIsActive = true;
    m_fElapsed = 0;
    for (m_uParticleIdx = 0; m_uParticleIdx < m_uParticleCount; ++m_uParticleIdx)
    {
        tCCParticle *p = &m_pParticles[m_uParticleIdx];
        p->timeToLive = 0;
    }
}
bool ParticleSystem::isFull()
{
    return (m_uParticleCount == m_uTotalParticles);
}

// ParticleSystem - MainLoop
void ParticleSystem::update(float dt)
{
    AX_PROFILER_START_CATEGORY(kCCProfilerCategoryParticles , "ParticleSystem - update");

    if (m_bIsActive && m_fEmissionRate)
    {
        float rate = 1.0f / m_fEmissionRate;
        //issue #1201, prevent bursts of particles, due to too high emitCounter
        if (m_uParticleCount < m_uTotalParticles)
        {
            m_fEmitCounter += dt;
        }
        
        while (m_uParticleCount < m_uTotalParticles && m_fEmitCounter > rate) 
        {
            this->addParticle();
            m_fEmitCounter -= rate;
        }

        m_fElapsed += dt;
        if (m_fDuration != -1 && m_fDuration < m_fElapsed)
        {
            this->stopSystem();
        }
    }

    m_uParticleIdx = 0;

    Vec2 currentPosition = Vec2::ZERO;
    if (m_ePositionType == kCCPositionTypeFree)
    {
        currentPosition = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if (m_ePositionType == kCCPositionTypeRelative)
    {
        currentPosition = m_obPosition;
    }

    if (m_bVisible)
    {
        while (m_uParticleIdx < m_uParticleCount)
        {
            tCCParticle *p = &m_pParticles[m_uParticleIdx];

            // life
            p->timeToLive -= dt;

            if (p->timeToLive > 0) 
            {
                // Mode A: gravity, direction, tangential accel & radial accel
                if (m_nEmitterMode == kCCParticleModeGravity) 
                {
                    Vec2 tmp, radial, tangential;

                    radial = Vec2::ZERO;
                    // radial acceleration
                    if (p->pos.x || p->pos.y)
                    {
                        radial = PointNormalize(p->pos);
                    }
                    tangential = radial;
                    radial = PointMult(radial, p->modeA.radialAccel);

                    // tangential acceleration
                    float newy = tangential.x;
                    tangential.x = -tangential.y;
                    tangential.y = newy;
                    tangential = PointMult(tangential, p->modeA.tangentialAccel);

                    // (gravity + radial + tangential) * dt
                    tmp = PointAdd( PointAdd( radial, tangential), modeA.gravity);
                    tmp = PointMult( tmp, dt);
                    p->modeA.dir = PointAdd( p->modeA.dir, tmp);
                    tmp = PointMult(p->modeA.dir, dt);
                    p->pos = PointAdd( p->pos, tmp );
                }

                // Mode B: radius movement
                else 
                {                
                    // Update the angle and radius of the particle.
                    p->modeB.angle += p->modeB.degreesPerSecond * dt;
                    p->modeB.radius += p->modeB.deltaRadius * dt;

                    p->pos.x = - cosf(p->modeB.angle) * p->modeB.radius;
                    p->pos.y = - sinf(p->modeB.angle) * p->modeB.radius;
                }

                // color
                p->color.r += (p->deltaColor.r * dt);
                p->color.g += (p->deltaColor.g * dt);
                p->color.b += (p->deltaColor.b * dt);
                p->color.a += (p->deltaColor.a * dt);

                // size
                p->size += (p->deltaSize * dt);
                p->size = MAX( 0, p->size );

                // angle
                p->rotation += (p->deltaRotation * dt);

                //
                // update values in quad
                //

                Vec2    newPos;

                if (m_ePositionType == kCCPositionTypeFree || m_ePositionType == kCCPositionTypeRelative) 
                {
                    Vec2 diff = PointSub( currentPosition, p->startPos );
                    newPos = PointSub(p->pos, diff);
                } 
                else
                {
                    newPos = p->pos;
                }

                // translate newPos to correct position, since matrix transform isn't performed in batchnode
                // don't update the particle with the new position information, it will interfere with the radius and tangential calculations
                if (m_pBatchNode)
                {
                    newPos.x+=m_obPosition.x;
                    newPos.y+=m_obPosition.y;
                }

                updateQuadWithParticle(p, newPos);
                //updateParticleImp(self, updateParticleSel, p, newPos);

                // update particle counter
                ++m_uParticleIdx;
            } 
            else 
            {
                // life < 0
                int currentIndex = p->atlasIndex;
                if( m_uParticleIdx != m_uParticleCount-1 )
                {
                    m_pParticles[m_uParticleIdx] = m_pParticles[m_uParticleCount-1];
                }
                if (m_pBatchNode)
                {
                    //disable the switched particle
                    m_pBatchNode->disableParticle(m_uAtlasIndex+currentIndex);

                    //switch indexes
                    m_pParticles[m_uParticleCount-1].atlasIndex = currentIndex;
                }


                --m_uParticleCount;

                if( m_uParticleCount == 0 && m_bIsAutoRemoveOnFinish )
                {
                    this->unscheduleUpdate();
                    m_pParent->removeChild(this, true);
                    return;
                }
            }
        } //while
        m_bTransformSystemDirty = false;
    }
    if (! m_pBatchNode)
    {
        postStep();
    }

    AX_PROFILER_STOP_CATEGORY(kCCProfilerCategoryParticles , "ParticleSystem - update");
}

void ParticleSystem::updateWithNoTime(void)
{
    this->update(0.0f);
}

void ParticleSystem::updateQuadWithParticle(tCCParticle* particle, const Vec2& newPosition)
{
    AX_UNUSED_PARAM(particle);
    AX_UNUSED_PARAM(newPosition);
    // should be overridden
}

void ParticleSystem::postStep()
{
    // should be overridden
}

// ParticleSystem - CCTexture protocol
void ParticleSystem::setTexture(Texture2D* var)
{
    if (m_pTexture != var)
    {
        AX_SAFE_RETAIN(var);
        AX_SAFE_RELEASE(m_pTexture);
        m_pTexture = var;
        updateBlendFunc();
    }
}

void ParticleSystem::updateBlendFunc()
{
    AXAssert(! m_pBatchNode, "Can't change blending functions when the particle is being batched");

    if(m_pTexture)
    {
        bool premultiplied = m_pTexture->hasPremultipliedAlpha();
        
        m_bOpacityModifyRGB = false;
        
        if( m_pTexture && ( m_tBlendFunc.src == AX_BLEND_SRC && m_tBlendFunc.dst == AX_BLEND_DST ) )
        {
            if( premultiplied )
            {
                m_bOpacityModifyRGB = true;
            }
            else
            {
                m_tBlendFunc.src = GL_SRC_ALPHA;
                m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
            }
        }
    }
}

Texture2D * ParticleSystem::getTexture()
{
    return m_pTexture;
}

// ParticleSystem - Additive Blending
void ParticleSystem::setBlendAdditive(bool additive)
{
    if( additive )
    {
        m_tBlendFunc.src = GL_SRC_ALPHA;
        m_tBlendFunc.dst = GL_ONE;
    }
    else
    {
        if( m_pTexture && ! m_pTexture->hasPremultipliedAlpha() )
        {
            m_tBlendFunc.src = GL_SRC_ALPHA;
            m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
        } 
        else 
        {
            m_tBlendFunc.src = AX_BLEND_SRC;
            m_tBlendFunc.dst = AX_BLEND_DST;
        }
    }
}

bool ParticleSystem::isBlendAdditive()
{
    return( m_tBlendFunc.src == GL_SRC_ALPHA && m_tBlendFunc.dst == GL_ONE);
}

// ParticleSystem - Properties of Gravity Mode 
void ParticleSystem::setTangentialAccel(float t)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.tangentialAccel = t;
}

float ParticleSystem::getTangentialAccel()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.tangentialAccel;
}

void ParticleSystem::setTangentialAccelVar(float t)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.tangentialAccelVar = t;
}

float ParticleSystem::getTangentialAccelVar()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.tangentialAccelVar;
}    

void ParticleSystem::setRadialAccel(float t)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.radialAccel = t;
}

float ParticleSystem::getRadialAccel()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.radialAccel;
}

void ParticleSystem::setRadialAccelVar(float t)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.radialAccelVar = t;
}

float ParticleSystem::getRadialAccelVar()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.radialAccelVar;
}

void ParticleSystem::setRotationIsDir(bool t)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.rotationIsDir = t;
}

bool ParticleSystem::getRotationIsDir()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.rotationIsDir;
}

void ParticleSystem::setGravity(const Vec2& g)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.gravity = g;
}

const Vec2& ParticleSystem::getGravity()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.gravity;
}

void ParticleSystem::setSpeed(float speed)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.speed = speed;
}

float ParticleSystem::getSpeed()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.speed;
}

void ParticleSystem::setSpeedVar(float speedVar)
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    modeA.speedVar = speedVar;
}

float ParticleSystem::getSpeedVar()
{
    AXAssert( m_nEmitterMode == kCCParticleModeGravity, "Particle Mode should be Gravity");
    return modeA.speedVar;
}

// ParticleSystem - Properties of Radius Mode
void ParticleSystem::setStartRadius(float startRadius)
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    modeB.startRadius = startRadius;
}

float ParticleSystem::getStartRadius()
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    return modeB.startRadius;
}

void ParticleSystem::setStartRadiusVar(float startRadiusVar)
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    modeB.startRadiusVar = startRadiusVar;
}

float ParticleSystem::getStartRadiusVar()
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    return modeB.startRadiusVar;
}

void ParticleSystem::setEndRadius(float endRadius)
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    modeB.endRadius = endRadius;
}

float ParticleSystem::getEndRadius()
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    return modeB.endRadius;
}

void ParticleSystem::setEndRadiusVar(float endRadiusVar)
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    modeB.endRadiusVar = endRadiusVar;
}

float ParticleSystem::getEndRadiusVar()
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    return modeB.endRadiusVar;
}

void ParticleSystem::setRotatePerSecond(float degrees)
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    modeB.rotatePerSecond = degrees;
}

float ParticleSystem::getRotatePerSecond()
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    return modeB.rotatePerSecond;
}

void ParticleSystem::setRotatePerSecondVar(float degrees)
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    modeB.rotatePerSecondVar = degrees;
}

float ParticleSystem::getRotatePerSecondVar()
{
    AXAssert( m_nEmitterMode == kCCParticleModeRadius, "Particle Mode should be Radius");
    return modeB.rotatePerSecondVar;
}

bool ParticleSystem::isActive()
{
    return m_bIsActive;
}

unsigned int ParticleSystem::getParticleCount()
{
    return m_uParticleCount;
}

float ParticleSystem::getDuration()
{
    return m_fDuration;
}

void ParticleSystem::setDuration(float var)
{
    m_fDuration = var;
}

const Vec2& ParticleSystem::getSourcePosition()
{
    return m_tSourcePosition;
}

void ParticleSystem::setSourcePosition(const Vec2& var)
{
    m_tSourcePosition = var;
}

const Vec2& ParticleSystem::getPosVar()
{
    return m_tPosVar;
}

void ParticleSystem::setPosVar(const Vec2& var)
{
    m_tPosVar = var;
}

float ParticleSystem::getLife()
{
    return m_fLife;
}

void ParticleSystem::setLife(float var)
{
    m_fLife = var;
}

float ParticleSystem::getLifeVar()
{
    return m_fLifeVar;
}

void ParticleSystem::setLifeVar(float var)
{
    m_fLifeVar = var;
}

float ParticleSystem::getAngle()
{
    return m_fAngle;
}

void ParticleSystem::setAngle(float var)
{
    m_fAngle = var;
}

float ParticleSystem::getAngleVar()
{
    return m_fAngleVar;
}

void ParticleSystem::setAngleVar(float var)
{
    m_fAngleVar = var;
}

float ParticleSystem::getStartSize()
{
    return m_fStartSize;
}

void ParticleSystem::setStartSize(float var)
{
    m_fStartSize = var;
}

float ParticleSystem::getStartSizeVar()
{
    return m_fStartSizeVar;
}

void ParticleSystem::setStartSizeVar(float var)
{
    m_fStartSizeVar = var;
}

float ParticleSystem::getEndSize()
{
    return m_fEndSize;
}

void ParticleSystem::setEndSize(float var)
{
    m_fEndSize = var;
}

float ParticleSystem::getEndSizeVar()
{
    return m_fEndSizeVar;
}

void ParticleSystem::setEndSizeVar(float var)
{
    m_fEndSizeVar = var;
}

const ccColor4F& ParticleSystem::getStartColor()
{
    return m_tStartColor;
}

void ParticleSystem::setStartColor(const ccColor4F& var)
{
    m_tStartColor = var;
}

const ccColor4F& ParticleSystem::getStartColorVar()
{
    return m_tStartColorVar;
}

void ParticleSystem::setStartColorVar(const ccColor4F& var)
{
    m_tStartColorVar = var;
}

const ccColor4F& ParticleSystem::getEndColor()
{
    return m_tEndColor;
}

void ParticleSystem::setEndColor(const ccColor4F& var)
{
    m_tEndColor = var;
}

const ccColor4F& ParticleSystem::getEndColorVar()
{
    return m_tEndColorVar;
}

void ParticleSystem::setEndColorVar(const ccColor4F& var)
{
    m_tEndColorVar = var;
}

float ParticleSystem::getStartSpin()
{
    return m_fStartSpin;
}

void ParticleSystem::setStartSpin(float var)
{
    m_fStartSpin = var;
}

float ParticleSystem::getStartSpinVar()
{
    return m_fStartSpinVar;
}

void ParticleSystem::setStartSpinVar(float var)
{
    m_fStartSpinVar = var;
}

float ParticleSystem::getEndSpin()
{
    return m_fEndSpin;
}

void ParticleSystem::setEndSpin(float var)
{
    m_fEndSpin = var;
}
float ParticleSystem::getEndSpinVar()
{
    return m_fEndSpinVar;
}

void ParticleSystem::setEndSpinVar(float var)
{
    m_fEndSpinVar = var;
}

float ParticleSystem::getEmissionRate()
{
    return m_fEmissionRate;
}

void ParticleSystem::setEmissionRate(float var)
{
    m_fEmissionRate = var;
}

unsigned int ParticleSystem::getTotalParticles()
{
    return m_uTotalParticles;
}

void ParticleSystem::setTotalParticles(unsigned int var)
{
    AXAssert( var <= m_uAllocatedParticles, "Particle: resizing particle array only supported for quads");
    m_uTotalParticles = var;
}

ccBlendFunc ParticleSystem::getBlendFunc()
{
    return m_tBlendFunc;
}

void ParticleSystem::setBlendFunc(ccBlendFunc blendFunc)
{
    if( m_tBlendFunc.src != blendFunc.src || m_tBlendFunc.dst != blendFunc.dst ) {
        m_tBlendFunc = blendFunc;
        this->updateBlendFunc();
    }
}

bool ParticleSystem::getOpacityModifyRGB()
{
    return m_bOpacityModifyRGB;
}

void ParticleSystem::setOpacityModifyRGB(bool bOpacityModifyRGB)
{
    m_bOpacityModifyRGB = bOpacityModifyRGB;
}

tCCPositionType ParticleSystem::getPositionType()
{
    return m_ePositionType;
}

void ParticleSystem::setPositionType(tCCPositionType var)
{
    m_ePositionType = var;
}

bool ParticleSystem::isAutoRemoveOnFinish()
{
    return m_bIsAutoRemoveOnFinish;
}

void ParticleSystem::setAutoRemoveOnFinish(bool var)
{
    m_bIsAutoRemoveOnFinish = var;
}

int ParticleSystem::getEmitterMode()
{
    return m_nEmitterMode;
}

void ParticleSystem::setEmitterMode(int var)
{
    m_nEmitterMode = var;
}


// ParticleSystem - methods for batchNode rendering

ParticleBatchNode* ParticleSystem::getBatchNode(void)
{
    return m_pBatchNode;
}

void ParticleSystem::setBatchNode(ParticleBatchNode* batchNode)
{
    if( m_pBatchNode != batchNode ) {

        m_pBatchNode = batchNode; // weak reference

        if( batchNode ) {
            //each particle needs a unique index
            for (unsigned int i = 0; i < m_uTotalParticles; i++)
            {
                m_pParticles[i].atlasIndex=i;
            }
        }
    }
}

//don't use a transform matrix, this is faster
void ParticleSystem::setScale(float s)
{
    m_bTransformSystemDirty = true;
    Node::setScale(s);
}

void ParticleSystem::setRotation(float newRotation)
{
    m_bTransformSystemDirty = true;
    Node::setRotation(newRotation);
}

void ParticleSystem::setScaleX(float newScaleX)
{
    m_bTransformSystemDirty = true;
    Node::setScaleX(newScaleX);
}

void ParticleSystem::setScaleY(float newScaleY)
{
    m_bTransformSystemDirty = true;
    Node::setScaleY(newScaleY);
}


NS_AX_END


/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2010      Ricardo Quesada

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

#include "Configuration.h"
#include "ccMacros.h"
#include "ccConfig.h"
#include <string.h>
#include "base/Dictionary.h"
#include "base/Integer.h"
#include "base/Bool.h"
#include "axolotl.h"
#include "platform/FileUtils.h"

using namespace std;

NS_AX_BEGIN

Configuration::Configuration(void)
	: m_nMaxTextureSize(0)
	, m_nMaxModelviewStackDepth(0)
	, m_bSupportsPVRTC(false)
	, m_bSupportsNPOT(false)
	, m_bSupportsBGRA8888(false)
	, m_bSupportsDiscardFramebuffer(false)
	, m_bSupportsShareableVAO(false)
	, m_nMaxSamplesAllowed(0)
	, m_nMaxTextureUnits(0)
	, m_pGlExtensions(NULL)
	, _valueDict(NULL)
{
}

Configuration::~Configuration(void)
{
	AX_SAFE_RELEASE(_valueDict);
}

static Configuration* _sharedConfiguration = nullptr;

Configuration* Configuration::sharedConfiguration()
{
	if (!_sharedConfiguration)
	{
		_sharedConfiguration = new Configuration();
		_sharedConfiguration->init();
	}
	return _sharedConfiguration;
}

void Configuration::purgeConfiguration()
{
	AX_SAFE_DELETE(_sharedConfiguration);
}

bool Configuration::init()
{
	_valueDict = Dictionary::create();
	AX_SAFE_RETAIN(_valueDict);

	_valueDict->setObject(String::create(axolotlVersion().toString()), "axolotl.version");

	_valueDict->setObject(Bool::create(AX_ENABLE_PROFILERS == 1), "axolotl.compiled_with_profiler");
	_valueDict->setObject(Bool::create(AX_ENABLE_GL_STATE_CACHE == 1), "axolotl.compiled_with_profiler");

	return true;
}

void Configuration::dumpInfo(void) const
{
	PrettyPrinter visitor(0);
	_valueDict->acceptVisitor(visitor);

	AXLOG("%s", visitor.getResult().c_str());

#if AX_ENABLE_PROFILERS
    AXLOGWARN("**** WARNING **** AX_ENABLE_PROFILERS is defined. Disable it when you finish profiling (from ccConfig.h)");
#endif

#if AX_ENABLE_GL_STATE_CACHE == 0
    AXLOGWARN("**** WARNING **** AX_ENABLE_GL_STATE_CACHE is disabled. To improve performance, enable it (from ccConfig.h)");
#endif

}

void Configuration::gatherGPUInfo()
{
	_valueDict->setObject( String::create( (const char*)glGetString(GL_VENDOR)), "gl.vendor");
	_valueDict->setObject( String::create( (const char*)glGetString(GL_RENDERER)), "gl.renderer");
	_valueDict->setObject( String::create( (const char*)glGetString(GL_VERSION)), "gl.version");

    m_pGlExtensions = (char *)glGetString(GL_EXTENSIONS);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_nMaxTextureSize);
	_valueDict->setObject( Integer::create((int)m_nMaxTextureSize), "gl.max_texture_size");

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_nMaxTextureUnits);
	_valueDict->setObject( Integer::create((int)m_nMaxTextureUnits), "gl.max_texture_units");

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    glGetIntegerv(GL_MAX_SAMPLES_APPLE, &m_nMaxSamplesAllowed);
	_valueDict->setObject( Integer::create((int)m_nMaxSamplesAllowed), "gl.max_samples_allowed");
#endif

    m_bSupportsPVRTC = checkForGLExtension("GL_IMG_texture_compression_pvrtc");
	_valueDict->setObject( Bool::create(m_bSupportsPVRTC), "gl.supports_PVRTC");

    m_bSupportsNPOT = true;
	_valueDict->setObject( Bool::create(m_bSupportsNPOT), "gl.supports_NPOT");
	
    m_bSupportsBGRA8888 = checkForGLExtension("GL_IMG_texture_format_BGRA888");
	_valueDict->setObject( Bool::create(m_bSupportsBGRA8888), "gl.supports_BGRA8888");

    m_bSupportsDiscardFramebuffer = checkForGLExtension("GL_EXT_discard_framebuffer");
	_valueDict->setObject( Bool::create(m_bSupportsDiscardFramebuffer), "gl.supports_discard_framebuffer");

    m_bSupportsShareableVAO = checkForGLExtension("vertex_array_object");
	_valueDict->setObject( Bool::create(m_bSupportsShareableVAO), "gl.supports_vertex_array_object");
    
    CHECK_GL_ERROR_DEBUG();
}

bool Configuration::checkForGLExtension(const string &searchName) const
{
    bool bRet = false;
    const char *kSearchName = searchName.c_str();
    
    if (m_pGlExtensions && 
        strstr(m_pGlExtensions, kSearchName))
    {
        bRet = true;
    }
    
    return bRet;
}

//
// getters for specific variables.
// Mantained for backward compatiblity reasons only.
//
int Configuration::getMaxTextureSize(void) const
{
	return m_nMaxTextureSize;
}

int Configuration::getMaxModelviewStackDepth(void) const
{
	return m_nMaxModelviewStackDepth;
}

int Configuration::getMaxTextureUnits(void) const
{
	return m_nMaxTextureUnits;
}

bool Configuration::supportsNPOT(void) const
{
	return m_bSupportsNPOT;
}

bool Configuration::supportsPVRTC(void) const
{
	return m_bSupportsPVRTC;
}

bool Configuration::supportsBGRA8888(void) const
{
	return m_bSupportsBGRA8888;
}

bool Configuration::supportsDiscardFramebuffer(void) const
{
	return m_bSupportsDiscardFramebuffer;
}

bool Configuration::supportsShareableVAO(void) const
{
	return m_bSupportsShareableVAO;
}

const std::string& Configuration::getString(const char* key, const std::string& defaultValue)
{
	Object* ret = _valueDict->objectForKey(key);
	if (ret)
	{
		if (String* str = dynamic_cast<String*>(ret))
			return str->getString();

		AXAssert(false, "Key found, but from different type");
	}

	return defaultValue;
}

//
// generic getters for properties
//
const char *Configuration::getCString( const char *key, const char *default_value ) const
{
	Object *ret = _valueDict->objectForKey(key);
	if( ret ) {
		if( String *str=dynamic_cast<String*>(ret) )
			return str->getCString();

		AXAssert(false, "Key found, but from different type");
	}

	// XXX: Should it throw an exception ?
	return default_value;
}

/** returns the value of a given key as a boolean */
bool Configuration::getBool( const char *key, bool default_value ) const
{
	Object *ret = _valueDict->objectForKey(key);
	if( ret ) {
		if( Bool *boolobj=dynamic_cast<Bool*>(ret) )
			return boolobj->getValue();
		if( String *strobj=dynamic_cast<String*>(ret) )
			return strobj->boolValue();
		AXAssert(false, "Key found, but from different type");
	}

	// XXX: Should it throw an exception ?
	return default_value;
}

/** returns the value of a given key as a double */
double Configuration::getNumber( const char *key, double default_value ) const
{
	Object *ret = _valueDict->objectForKey(key);
	if( ret ) {
		if( Double *obj=dynamic_cast<Double*>(ret) )
			return obj->getValue();

		if( Integer *obj=dynamic_cast<Integer*>(ret) )
			return obj->getValue();

		if( String *strobj=dynamic_cast<String*>(ret) )
			return strobj->doubleValue();

		AXAssert(false, "Key found, but from different type");
	}

	// XXX: Should it throw an exception ?
	return default_value;
}

Object * Configuration::getObject( const char *key ) const
{
	return _valueDict->objectForKey(key);
}

void Configuration::setObject( const char *key, Object *value )
{
	_valueDict->setObject(value, key);
}


//
// load file
//
void Configuration::loadConfigFile( const char *filename )
{
	Dictionary *dict = Dictionary::createWithContentsOfFile(filename);
	AXAssert(dict, "cannot create dictionary");

	// search for metadata
	bool metadata_ok = false;
	Object *metadata = dict->objectForKey("metadata");
	if( metadata && dynamic_cast<Dictionary*>(metadata) ) {
		Object *format_o = static_cast<Dictionary*>(metadata)->objectForKey("format");

		// XXX: cocos2d-x returns CCStrings when importing from .plist. This bug will be addressed in cocos2d-x v3.x
		if( format_o && dynamic_cast<String*>(format_o) ) {
			int format = static_cast<String*>(format_o)->intValue();

			// Support format: 1
			if( format == 1 ) {
				metadata_ok = true;
			}
		}
	}

	if( ! metadata_ok ) {
		AXLOG("Invalid config format for file: %s", filename);
		return;
	}

	Object *data = dict->objectForKey("data");
	if( !data || !dynamic_cast<Dictionary*>(data) ) {
		AXLOG("Expected 'data' dict, but not found. Config file: %s", filename);
		return;
	}

	// Add all keys in the existing dictionary
	Dictionary *data_dict = static_cast<Dictionary*>(data);
    DictElement* element;
    CCDICT_FOREACH(data_dict, element)
    {
		if( ! _valueDict->objectForKey( element->getStrKey() ) )
			_valueDict->setObject(element->getObject(), element->getStrKey() );
		else
			AXLOG("Key already present. Ignoring '%s'", element->getStrKey() );
    }
    
    Director::sharedDirector()->setDefaultValues();
}

NS_AX_END

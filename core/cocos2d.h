/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.
Copyright (c) Microsoft Open Technologies, Inc.

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

#ifndef __COCOS2D_H__
#define __COCOS2D_H__

// 0x00 HI ME LO
// 00   02 02 05
#define COCOS2D_VERSION 0x00020206

//
// all cocos2d include files
//
#include "ccConfig.h"

// actions
#include "actions/Action.h"
#include "actions/ActionInterval.h"
#include "actions/ActionCamera.h"
#include "actions/ActionManager.h"
#include "actions/ActionEase.h"
#include "actions/ActionPageTurn3D.h"
#include "actions/ActionGrid.h"
#include "actions/ActionProgressTimer.h"
#include "actions/ActionGrid3D.h"
#include "actions/ActionTiledGrid.h"
#include "actions/ActionInstant.h"
#include "actions/ActionTween.h"
#include "actions/ActionCatmullRom.h"

// base
#include "base/AutoreleasePool.h"
#include "base/Configuration.h"
#include "base/Director.h"
#include "base/Event.h"
#include "base/EventDispatcher.h"
#include "base/PoolManager.h"
#include "base/Scheduler.h"
#include "base/Touch.h"

// base_nodes
#include "base_nodes/Node.h"
#include "base_nodes/AtlasNode.h"

// cocoa
#include "cocoa/AffineTransform.h"
#include "cocoa/Dictionary.h"
#include "cocoa/Object.h"
#include "cocoa/Array.h"
#include "cocoa/Geometry.h"
#include "cocoa/Set.h"
#include "cocoa/Integer.h"
#include "cocoa/Float.h"
#include "cocoa/Double.h"
#include "cocoa/Bool.h"
#include "cocoa/String.h"
#include "cocoa/NS.h"
#include "cocoa/Number.h"
#include "cocoa/Zone.h"

// draw nodes
#include "draw_nodes/DrawingPrimitives.h"
#include "draw_nodes/DrawNode.h"

// effects
#include "effects/Grabber.h"
#include "effects/Grid.h"

// include
#include "EventType.h"
#include "Protocols.h"
#include "ccConfig.h"
#include "ccMacros.h"
#include "ccTypes.h"

// kazmath
#include "kazmath/include/kazmath/kazmath.h"
#include "kazmath/include/kazmath/GL/matrix.h"

// keypad_dispatcher
#include "keypad_dispatcher/KeypadDelegate.h"
#include "keypad_dispatcher/KeypadDispatcher.h"

// label_nodes
#include "label_nodes/LabelAtlas.h"
#include "label_nodes/LabelTTF.h"
#include "label_nodes/LabelBMFont.h"

// layers_scenes_transitions_nodes
#include "layers_scenes_transitions_nodes/Layer.h"
#include "layers_scenes_transitions_nodes/Scene.h"
#include "layers_scenes_transitions_nodes/Transition.h"
#include "layers_scenes_transitions_nodes/TransitionPageTurn.h"
#include "layers_scenes_transitions_nodes/TransitionProgress.h"

// menu_nodes
#include "menu_nodes/Menu.h"
#include "menu_nodes/MenuItem.h"

// misc_nodes
#include "misc_nodes/ClippingNode.h"
#include "misc_nodes/MotionStreak.h"
#include "misc_nodes/ProgressTimer.h"
#include "misc_nodes/RenderTexture.h"

// particle_nodes
#include "particle_nodes/ParticleBatchNode.h"
#include "particle_nodes/ParticleSystem.h"
#include "particle_nodes/ParticleExamples.h"
#include "particle_nodes/ParticleSystemQuad.h"

// platform
#include "platform/Device.h"
#include "platform/Common.h"
#include "platform/FileUtils.h"
#include "platform/Image.h"
#include "platform/SAXParser.h"
#include "platform/Thread.h"
#include "platform/platform.h"
#include "platform/PlatformConfig.h"
#include "platform/PlatformMacros.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    #include "platform/ios/Accelerometer.h"
    #include "platform/ios/Application.h"
    #include "platform/ios/EGLView.h"
    #include "platform/ios/GL.h"
    #include "platform/ios/StdC.h"
#endif // AX_TARGET_PLATFORM == AX_PLATFORM_IOS

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    #include "platform/android/Accelerometer.h"
    #include "platform/android/Application.h"
    #include "platform/android/EGLView.h"
    #include "platform/android/GL.h"
    #include "platform/android/StdC.h"
#endif // AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
	#include "platform/win32/Accelerometer.h"
	#include "platform/win32/Application.h"
	#include "platform/win32/EGLView.h"
	#include "platform/win32/GL.h"
	#include "platform/win32/StdC.h"
#endif // AX_TARGET_PLATFORM == AX_PLATFORM_WIN32

// script_support
#include "script_support/ScriptSupport.h"

// shaders
#include "shaders/GLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/ShaderCache.h"
#include "shaders/ccShaders.h"

// sprite_nodes
#include "sprite_nodes/Animation.h"
#include "sprite_nodes/AnimationCache.h"
#include "sprite_nodes/Sprite.h"
#include "sprite_nodes/SpriteBatchNode.h"
#include "sprite_nodes/SpriteFrame.h"
#include "sprite_nodes/SpriteFrameCache.h"

// support
#include "support/ccUTF8.h"
#include "support/NotificationCenter.h"
#include "support/PointExtension.h"
#include "support/Profiling.h"
#include "support/StringUtils.h"
#include "support/user_default/UserDefault.h"
#include "support/Vertex.h"
#include "support/tinyxml2/tinyxml2.h"

// text_input_node
#include "text_input_node/IMEDelegate.h"
#include "text_input_node/IMEDispatcher.h"
#include "text_input_node/TextFieldTTF.h"

// textures
#include "textures/Texture2D.h"
#include "textures/TextureAtlas.h"
#include "textures/TextureCache.h"
#include "textures/TexturePVR.h"

// tilemap_parallax_nodes
#include "tilemap_parallax_nodes/ParallaxNode.h"
#include "tilemap_parallax_nodes/TMXLayer.h"
#include "tilemap_parallax_nodes/TMXObjectGroup.h"
#include "tilemap_parallax_nodes/TMXTiledMap.h"
#include "tilemap_parallax_nodes/TMXXMLParser.h"
#include "tilemap_parallax_nodes/TileMapAtlas.h"

// root
#include "Camera.h"

// component
#include "support/component/Component.h"
#include "support/component/ComponentContainer.h"

NS_AX_BEGIN

AX_DLL const char* cocos2dVersion();

NS_AX_END

#endif // __COCOS2D_H__

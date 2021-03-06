/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _PHY3D_SHADERS_H_
#define _PHY3D_SHADERS_H_

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

namespace Shaders {
extern const GLchar* baseVertex;
extern const GLchar* squishVertex;
extern const GLchar* customColorFragment;
extern const GLchar* baseFragment;
}  // namespace Shaders

#endif
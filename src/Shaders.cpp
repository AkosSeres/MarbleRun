/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "Shaders.h"

// Given a projection and a model view matrix, does the projection on the
// incoming vertex
const GLchar* Shaders::baseVertex =
    "attribute vec4 vertexPos;"
    "uniform mat4 proj;"
    "uniform mat4 modelView;"
    "varying vec3 vPosition;"
    "void main() {"
    "   gl_Position = proj * modelView * vertexPos;"
    "   gl_Position.x = gl_Position.x * 2.0;"
    "   gl_Position.y = gl_Position.y * 2.0;"
    "   vPosition = gl_Position.xyz / gl_Position.w;"
    "}";

// Squished the picture as time moves forward
const GLchar* Shaders::squishVertex =
    "attribute vec4 pos;"
    "uniform float time;"
    "void main() {"
    "   gl_Position = vec4(pos.x*cos(time/1000.0), pos.y, pos.z, 1.0);"
    "}";

// Renders the pixel with a given color and does the clipping, because
// apparently OpenGL ES does not do it for me
const GLchar* Shaders::customColorFragment =
    "precision mediump float;"
    "varying vec3 vPosition;"
    "uniform vec4 color;"
    "void main() {"
    "   if (vPosition.z < 0.0) {"
    "       gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);"
    "   } else {"
    "       gl_FragColor = color;"
    "   }"
    "}";

// Renders the pixel in white color
const GLchar* Shaders::baseFragment =
    "precision mediump float;"
    "void main() {"
    "  gl_FragColor = vec4(1.0);"
    "}";

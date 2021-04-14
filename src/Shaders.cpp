#include "Shaders.h"

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

const GLchar* Shaders::squishVertex =
    "attribute vec4 pos;"
    "uniform float time;"
    "void main() {"
    "   gl_Position = vec4(pos.x*cos(time/1000.0), pos.y, pos.z, 1.0);"
    "}";

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

const GLchar* Shaders::baseFragment =
    "precision mediump float;"
    "void main() {"
    "  gl_FragColor = vec4(1.0);"
    "}";
#include "Shaders.h"

const GLchar* Shaders::squishVertex =
    "attribute vec4 pos;"
    "uniform float time;"
    "void main() {"
    "   gl_Position = vec4(pos.x*cos(time/1000.0), pos.y, pos.z, 1.0);"
    "}";

const GLchar* Shaders::baseFragment =
    "precision mediump float;"
    "void main() {"
    "  gl_FragColor = vec4 (vec4(1.0));"
    "}";
// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t COLOR_PIXEL_SHADER_OGL[] =
    "precision mediump float;\n"
    "varying lowp vec4 ex_Color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = ex_Color;\n"
    "}";
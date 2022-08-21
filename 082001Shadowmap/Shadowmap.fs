#version 330 core

void main()
{             
    //注释与否都行，因为这个绑定的framebuffer含有depthattach，所以无论如何都会绘制出深度缓存
    // gl_FragDepth = gl_FragCoord.z;
}
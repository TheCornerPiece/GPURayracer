#ifndef FBO_H
#define FBO_H

#include <GLFW/glfw3.h>
#include <GL/gl.h>


int loadFBO(int w, int h){
    GLuint fboId;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLuint texId;
    glGenTextures(1, &texId);

    glBindTexture(GL_TEXTURE_2D, texId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                 w, h, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                            GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D,
                            texId,
                            0);

    GLuint depthId;
    glGenRenderbuffers(1, &depthId);

    glBindRenderbuffer(GL_RENDERBUFFER, depthId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER,
                                depthId);

    // glCheckFramebufferStatus(fboId)

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fboId;
}

#endif
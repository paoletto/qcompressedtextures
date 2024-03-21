#include "mainwindow.h"

#include <QImage>
#include <QFile>
#include <QIODevice>
#include <QOpenGLPixelTransferOptions>

namespace  {
QOpenGLVertexArrayObject datalessVao;
static constexpr char vertexShader[] = R"(
#version 450 core
const int indices[6] = {0,1,3,1,2,3};
const vec4 vertices[4] = {
     vec4(-1,-1,0,1)
    ,vec4(1,-1,0,1)
    ,vec4(1,1,0,1)
    ,vec4(-1,1,0,1)
};
smooth out vec2 texCoord;
void main()
{
    gl_Position = vertices[indices[gl_VertexID]];
    texCoord = (gl_Position.xy + vec2(1,1)) * 0.5;
}
)";
static constexpr char fragmentShaderTextured[] = R"(
#version 450 core
uniform highp vec4 color;
uniform sampler2D raster;
smooth in vec2 texCoord;
out vec4 fragColor;
void main()
{
    fragColor = texture(raster, texCoord);
}
)";
}

OpenGLWindow::~OpenGLWindow()
{
}

OpenGLWindow::OpenGLWindow()
    : QOpenGLWindow(QOpenGLWindow::NoPartialUpdate)
{
    setGeometry(300, 300, 500, 500);
    m_image.load(":/mtest.png");
    m_image = m_image.mirrored(false, true);
    QFile f(":/mtest4x4.astc");
    f.open(QIODevice::ReadOnly);
    m_astc = f.readAll();
    f.close();
}

void OpenGLWindow::paintGL()
{

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    if (!m_shader) {
        m_shader.reset(new QOpenGLShaderProgram);
        m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                          QByteArray(vertexShader));
        m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                          QByteArray(fragmentShaderTextured));
        m_shader->link();
        m_shader->setObjectName("shader");

        m_texWhite.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        m_texWhite->setMaximumAnisotropy(16);
        m_texWhite->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,
                                     QOpenGLTexture::Linear);
        m_texWhite->setWrapMode(QOpenGLTexture::ClampToEdge);
        QImage white(2,2,QImage::Format_RGB888);
        white.setPixelColor(0,0, QColorConstants::Red);
        white.setPixelColor(0,1, QColorConstants::Red);
        white.setPixelColor(1,0, QColorConstants::Red);
        white.setPixelColor(1,1, QColorConstants::Red);
        m_texWhite->setData(white);

        m_tex.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        m_tex->setMaximumAnisotropy(16);
        m_tex->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,
                                     QOpenGLTexture::Linear);
        m_tex->setWrapMode(QOpenGLTexture::ClampToEdge);
        m_tex->setData(m_image);


        m_texASTC.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        m_texASTC->setMaximumAnisotropy(16);
        auto error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        m_texASTC->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,
                                     QOpenGLTexture::Linear);
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        m_texASTC->setFormat(QOpenGLTexture::RGBA_ASTC_4x4);
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        m_texASTC->setAutoMipMapGenerationEnabled(false);
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        m_texASTC->setSize(m_image.width(), m_image.height());
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        m_texASTC->allocateStorage();
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(1);
        m_texASTC->setCompressedData(m_astc.size(), m_astc.constData(), &uploadOptions);
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;

        f->glGenTextures(1, &m_nativeTexASTC);
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
        f->glBindTexture(GL_TEXTURE_2D, m_nativeTexASTC);
        f->glCompressedTexImage2D(GL_TEXTURE_2D,
                                    0,
                                    GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
                                    m_image.width(),
                                    m_image.height(),
                                    0,
                                    m_astc.size(),
                                    (const GLvoid*) m_astc.constData());
        error = f->glGetError(); if (error != GL_NO_ERROR) qDebug() << "GLERROR("<<__LINE__<<"): " << error;
    }
    m_shader->bind();
    f->glEnable(GL_TEXTURE_2D);

    QOpenGLTexture *t = m_tex.get();
    t = m_texASTC.get();

    t->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(&datalessVao);
    const int numVertices = 6;
    f->glDrawArrays(GL_TRIANGLES, 0, numVertices);
    m_shader->release();

    t->release();
}

void OpenGLWindow::resizeGL(int, int)
{
}

void OpenGLWindow::keyPressEvent(QKeyEvent *)
{
}

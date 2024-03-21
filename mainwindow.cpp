#include "mainwindow.h"

#include <QImage>
#include <QFile>
#include <QIODevice>
#include <QOpenGLPixelTransferOptions>
#include <private/qtexturefilereader_p.h>
#include <private/qtexturefiledata_p.h>
#include <private/qtexturefilehandler_p.h>

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
//    QFile f(":/mtest4x4.astc");
    QFile f(":/Earth-Night4x4.astc"); // from https://github.com/ARM-software/opengl-es-sdk-for-android/tree/master/samples/advanced_samples/AstcTextures/assets
    bool res = f.open(QIODevice::ReadOnly);
    if (!res) {
        qWarning()<<"Failed opening " <<f.fileName();
    }
    QTextureFileReader fr(&f);
    if (!fr.canRead())
        qWarning()<<"TFR cannot read texture!";

    m_astc = fr.read();

//    m_astc = f.readAll();
    f.close();
}

namespace {
void printGLError(QOpenGLFunctions *f, int line) {
    GLenum err;
    while((err = f->glGetError()) != GL_NO_ERROR)
    {
      // Process/log the error.
        qDebug() << "GLERROR("<<line<<"): " << err;
    }
}
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


        QSize astcSize = m_image.size();
        astcSize = QSize(1024,512);
        m_texASTC.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
        m_texASTC->setMaximumAnisotropy(16);
        printGLError(f, __LINE__);
        m_texASTC->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,
                                     QOpenGLTexture::Linear);
        printGLError(f, __LINE__);
        m_texASTC->setFormat(QOpenGLTexture::TextureFormat(m_astc.glInternalFormat()));
//        m_texASTC->setFormat(QOpenGLTexture::RGBA_ASTC_6x5);
        printGLError(f, __LINE__);
        m_texASTC->setAutoMipMapGenerationEnabled(false);
        printGLError(f, __LINE__);
        m_texASTC->setSize(m_astc.size().width(), m_astc.size().height());
        printGLError(f, __LINE__);
        m_texASTC->allocateStorage();
        printGLError(f, __LINE__);
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(1);
        m_texASTC->setCompressedData(m_astc.dataLength(), m_astc.data().constData(), &uploadOptions);
        printGLError(f, __LINE__);

        f->glGenTextures(1, &m_nativeTexASTC);
        printGLError(f, __LINE__);
        f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        f->glBindTexture(GL_TEXTURE_2D, m_nativeTexASTC);
        f->glCompressedTexImage2D(GL_TEXTURE_2D,
                                    0,
                                    m_astc.glFormat(),
                                    astcSize.width(),
                                    astcSize.height(),
                                    0,
                                    m_astc.dataLength(),
                                    (const GLvoid*) m_astc.data().constData());
        printGLError(f, __LINE__);
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

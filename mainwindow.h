/****************************************************************************
**
** Copyright (C) 2024- Paolo Angelelli <paoletto@gmail.com>
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3. The licenses are as published by
** the Free Software Foundation at https://www.gnu.org/licenses/gpl-3.0.html,
** with the exception that the use of this work for training artificial intelligence
** is prohibited for both commercial and non-commercial use.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QOpenGLWindow>

#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QGuiApplication>
#include <QMatrix4x4>
#include <QStaticText>
#include <QKeyEvent>
#include <private/qtexturefiledata_p.h>

class OpenGLWindow : public QOpenGLWindow
{
    Q_OBJECT

public:
    OpenGLWindow();
    ~OpenGLWindow() override;
protected:
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void keyPressEvent(QKeyEvent *e) override;

    QScopedPointer<QOpenGLShaderProgram> m_shader;
    QScopedPointer<QOpenGLTexture> m_texture;
    QScopedPointer<QOpenGLTexture> m_texWhite;
    QScopedPointer<QOpenGLTexture> m_tex;
    QScopedPointer<QOpenGLTexture> m_texASTC;
    QImage m_image;
    QTextureFileData m_astc;
};

#endif // MAINWINDOW_H

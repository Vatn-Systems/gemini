#include "compasswidget.h"
#include "gemininetwork.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QOpenGLContext>
#include <QtGui/QWheelEvent>
#include <QtGui/QOpenGLFramebufferObject>

#ifdef USE_GLES
#include <GLES3/gl3ext.h>
#else
#include <GL/gl.h>
#endif

#include <math.h>


CompassWidget::CompassWidget(QWidget *parent)
:   QOpenGLWidget(parent)
  ,  m_glfLogger(false)
  ,  m_zoom(1.0f)
  ,  m_enableWheelZoom(true)
,   m_cleanupRender( false )
{

}

CompassWidget::~CompassWidget()
{
    cleanup();
}

void CompassWidget::onCompassData(GLF::CompassDataRecord *cdr)
{
    m_glfLogger = true;

    unsigned short deviceID = 1;        //To change this for a correct device id
    if (deviceID != 0)
    {
        if (m_renderers.find(deviceID) == m_renderers.end())
        {
            // create the renderer...
            m_renderers[deviceID] = new CompassRenderer();
            if (context())
            {
                m_renderers[deviceID]->initialiseGL(*context());
                m_renderers[deviceID]->setEnabled(true);
            }
        }
        m_renderers[deviceID]->setCompassRec(*cdr);
        update();   // redraw request
    }

}

#ifdef _ECD_LOGGER_
void CompassWidget::onEcdCompassData(GLF::CompassDataRecord *cdr)
{


}
#endif

void CompassWidget::initializeGL()
{
    // print the opengl version information
    QString info = ((context()->isOpenGLES()) ? "OpenGL ES " : "OpenGL ");
    info += QString::fromUtf8(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    info += " " ;
    info += QString::fromUtf8(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    info += " GLSL:" ;
    info += QString::fromUtf8(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    info += " Format: " + QString("%1").arg(context()->format().version().first);
    info += "." ;
    info += QString("%1").arg(context()->format().version().second);
    qDebug() << info;

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &CompassWidget::cleanup);
}

void CompassWidget::paintGL()
{

    LockRenderer();
    if( m_cleanupRender )
    {
        cleanup();
        m_cleanupRender = false;
    }
    UnlockRenderer();

    QOpenGLContext& gl = *context();

    glEnable(GL_SCISSOR_TEST);

    // work out the number of rows/columns we need to put each renderer in its own viewport...
    double sonarCount = static_cast<double>(m_renderers.size());
    int columns = static_cast<int>(ceil(sqrt(sonarCount)));
    int rows = static_cast<int>(ceil(sonarCount / columns));
    if (height() > width())
    {
        int temp = rows;
        rows = columns;
        columns = temp;
    }

    // float view[4];
    // glGetFloatv(GL_VIEWPORT, view);

    auto it = m_renderers.begin();
    for (int y = 0; y < rows; y++)
    {
        // In OpenGL (0,0) is bottom left
        int ystart = height() - (((y + 1) * height()) / rows);
        int yend = height() - ((y * height()) / rows);
        for (int x = 0; x < columns; x++, it++)
        {
            if (it == m_renderers.end())
                break;

            int xstart = (x * width()) / columns;
            int xend = ((x + 1) * width()) / columns;
            glScissor(xstart, ystart, xend - xstart, yend - ystart);
            glViewport(xstart, ystart, xend - xstart, yend - ystart);

            // depending on the count we want to set a viewport and clip region for this sonar
            it->second->paintGL(gl);
        }
        if (it == m_renderers.end())
            break;
    }

}

void CompassWidget::resizeGL(int w, int h)
{

    static int height = 0;
    if (height != h)
    {
        emit(sizeChanged(w, h));
        height = h;
    }

}

void CompassWidget::onSetZoom(float zoom)
{
    // clamp zoom values
    if (zoom < 0.5)
    {
        zoom = 0.5;
    }
    else if (3.0 < zoom)
    {
        zoom = 3.0;
    }

    m_zoom = zoom;

    update();
}

void CompassWidget::wheelEvent(QWheelEvent* event)
{

    if (m_enableWheelZoom)
    {
        onSetZoom(m_zoom + (event->delta() / 120.0f) / 15.0f);
    }

}


void CompassWidget::clearImage()
{
    LockRenderer();
    m_cleanupRender = true;
    update();   // redraw request
    UnlockRenderer();
}


void CompassWidget::cleanup()
{
    makeCurrent();
    if (context() && (m_renderers.size() > 0))
    {
        QOpenGLContext& gl = *context();
        for (auto it = m_renderers.begin(); it != m_renderers.end(); it++)
        {
            it->second->cleanup(gl);
            delete it->second;
        }
        m_renderers.clear();
    }
    doneCurrent();
}

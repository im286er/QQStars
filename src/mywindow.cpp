#include "mywindow.h"
#include "utility.h"
#include <QDebug>
#include <QQmlComponent>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#ifdef Q_OS_WIN
#include <winuser.h>
#endif
#include <QWidget>

bool test(QObject *, Qt::ShortcutContext)
{
    return true;
}

MyWindow::MyWindow(QQuickWindow *parent) :
    QQuickWindow(parent)
{
    connect (this, &QQuickWindow::widthChanged, this, &MyWindow::actualWidthChanged);
    connect (this, &QQuickWindow::heightChanged, this, &MyWindow::actualHeightChanged);
    connect (this, &QQuickWindow::xChanged, this, &MyWindow::actualXChanged);
    connect (this, &QQuickWindow::yChanged, this, &MyWindow::actualYChanged);
    connect (this, &QQuickWindow::xChanged, this, &MyWindow::onActualXChanged);
    connect (this, &QQuickWindow::yChanged, this, &MyWindow::onActualYChanged);
    connect (contentItem (), &QQuickItem::xChanged, this, &MyWindow::xChanged);
    connect (contentItem (), &QQuickItem::yChanged, this, &MyWindow::yChanged);
    
    setObjectName ("MyWindow");
    setActualWidth (QQuickWindow::width ());
    setActualHeight (QQuickWindow::height ());
    
    m_noBorder = false;
    m_windowStatus = StopCenter;
    m_topHint = false;
    old_topHint=false;
    m_noNotifyIcon = false;
    m_windowActive = false;
}

bool MyWindow::noNotifyIcon() const
{
    return m_noNotifyIcon;
}

int MyWindow::width() const
{
    return m_width;
}

int MyWindow::height() const
{
    return m_height;
}

int MyWindow::actualWidth() const
{
    return QQuickWindow::width ();
}

int MyWindow::actualHeight() const
{
    return QQuickWindow::height ();
}

bool MyWindow::windowActive() const
{
    return m_windowActive;
}

int MyWindow::x() const
{
    return QQuickWindow::x ()+contentItem ()->x ();
}

int MyWindow::y() const
{
    return QQuickWindow::y ()+contentItem ()->y ();
}

int MyWindow::actualX() const
{
    return QQuickWindow::x ();
}

int MyWindow::actualY() const
{
    return QQuickWindow::y ();
}

bool MyWindow::topHint() const
{
    return m_topHint;
}

QUrl MyWindow::windowIcon()
{
    return m_windowIcon;
}

void MyWindow::setWindowIcon(QUrl icon)
{
    if( icon!=m_windowIcon ){
        QString str = icon.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        setIcon (QIcon(str));
        m_windowIcon = icon;
        emit windowIconChanged ();
    }
}

void MyWindow::setWindowActive(bool arg)
{
    if(arg!=m_windowActive){
        m_windowActive = arg;
        emit windowActiveChanged (arg);
    }
}

void MyWindow::focusInEvent(QFocusEvent *ev)
{
    QQuickWindow::focusInEvent(ev);
    setWindowActive (true);
}

void MyWindow::focusOutEvent(QFocusEvent *ev)
{
    QQuickWindow::focusOutEvent (ev);
    setWindowActive (false);
}

void MyWindow::onActualXChanged()
{
    emit xChanged();
}

void MyWindow::onActualYChanged()
{
    emit yChanged();
}

bool MyWindow::noBorder()
{
    return m_noBorder;
}

void MyWindow::setNoBorder(bool isNoBroder)
{
    if( isNoBroder!=m_noBorder ) {
        m_noBorder = isNoBroder;
        if(!isVisible ()) {
            if( isNoBroder )
                setFlags (flags ()|Qt::FramelessWindowHint);
            else
                setFlags (flags ()&~Qt::FramelessWindowHint);
        }else{
            if( isNoBroder )
                setFlags (flags ()|Qt::FramelessWindowHint);
            else
                setFlags (flags ()&~Qt::FramelessWindowHint);
        }
        
        emit noBorderIconChanged();
    }
}

MyWindow::Status MyWindow::windowStatus()
{
    return m_windowStatus;
}

void MyWindow::setWindowStatus(MyWindow::Status new_status)
{
    if( new_status!=m_windowStatus ) {
        if( new_status == BerthPrepare&&m_windowStatus!=StopCenter ) {
            setTopHint (old_topHint);
        }else if(new_status!=StopCenter&&new_status!=BerthPrepare){
            old_topHint = topHint ();
            setTopHint (true);//设置窗口为最前端
        }
        m_windowStatus = new_status;
        emit windowStatusChanged ();
    }
}

int MyWindow::borderLeft()
{
#ifdef Q_OS_WIN
    return 0;
#elif defined(Q_OS_MACX)
    return 0;
#elif defined(Q_OS_LINUX)
    return 65;
#else
    return 0;
#endif
}

int MyWindow::borderRight()
{
#ifdef Q_OS_WIN
    return QApplication::screens ()[0]->size ().width ();
#elif defined(Q_OS_MACX)
    return QApplication::screens ()[0]->size ().width ();
#elif defined(Q_OS_LINUX)
    return QApplication::screens ()[0]->size ().width ();
#else
    return QApplication::screens ()[0]->size ().width ();
#endif
}

int MyWindow::borderTop()
{
#ifdef Q_OS_WIN
    return 0;
#elif defined(Q_OS_MACX)
    return 0;
#elif defined(Q_OS_LINUX)
    return 25;
#else
    return 0;
#endif
}


void MyWindow::setTopHint(bool arg)
{
    if (m_topHint != arg) {
        m_topHint = arg;
        if( arg ){
            setFlags (flags ()|Qt::WindowStaysOnTopHint);
#ifdef Q_OS_LINUX
            if(isVisible()){
                hide();
                show();
            }
#endif
        }else{
            setFlags (flags ()&~Qt::WindowStaysOnTopHint);
#ifdef Q_OS_LINUX
            if(isVisible()){
                hide();
                show();
            }
#endif
#ifdef Q_OS_WIN
            SetWindowPos ((HWND)this->winId (),HWND_NOTOPMOST,0,0,width(),height(),SWP_NOSIZE|SWP_NOMOVE);
#endif
        }
        emit topHintChanged(arg);
    }
}

void MyWindow::setNoNotifyIcon(bool arg)
{
    if ( m_noNotifyIcon != arg ) {
        m_noNotifyIcon = arg;
        if( arg )
            setFlags (flags ()&~Qt::Tool);
        else{
            setFlags (flags ()|Qt::Tool);
        }
        emit noNotifyIconChanged(arg);
    }
}

void MyWindow::setWidth(int arg)
{
    if (m_width != arg) {
        m_width = arg;
        contentItem ()->setWidth (arg);
        emit widthChanged(arg);
    }
}

void MyWindow::setHeight(int arg)
{
    if (m_height != arg) {
        m_height = arg;
        contentItem ()->setHeight (arg);
        emit heightChanged(arg);
    }
}

void MyWindow::setActualWidth(int arg)
{
    QQuickWindow::setWidth (arg);
}

void MyWindow::setActualHeight(int arg)
{
    QQuickWindow::setHeight (arg);
}

void MyWindow::setX(int arg)
{
    QQuickWindow::setX (arg-contentItem ()->x ());
}

void MyWindow::setY(int arg)
{
    QQuickWindow::setY (arg-contentItem ()->y ());
}

void MyWindow::setActualX(int arg)
{
    QQuickWindow::setX (arg);
}

void MyWindow::setActualY(int arg)
{
    QQuickWindow::setY (arg);
}


#ifndef qtcurvesizegrip_h
#define qtcurvesizegrip_h

//////////////////////////////////////////////////////////////////////////////
// qtcurvesizegrip.h
// -------------------
//
// Taken from Oxygen kwin decoration
// ------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include <QtGui/QWidget>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>

namespace KWinQtCurve
{

  class QtCurveClient;

  //! implements size grip for all widgets
  class QtCurveSizeGrip: public QWidget
  {

    public:

    //! constructor
    QtCurveSizeGrip( QtCurveClient* );

    //! constructor
    virtual ~QtCurveSizeGrip( void );

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    public slots:

    //! update background color
    void activeChange( void );

    protected slots:

    //! embed into parent widget
    void embed( void );

    protected:

    //!@name event handlers
    //@{

    //! paint
    virtual void paintEvent( QPaintEvent* );

    //! mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //@}

    //! client
    QtCurveClient& client( void ) const
    { return *client_; }

    //! update position
    void updatePosition( void );

    private:

    //! grip size
    enum {
      OFFSET = 0,
      GRIP_SIZE = 12
    };

    // qtcurve client
    QtCurveClient* client_;

  };


}

#endif

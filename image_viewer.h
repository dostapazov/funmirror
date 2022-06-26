#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H

#include <QtWidgets>



class wimage_viewer : public QWidget
{
  Q_OBJECT
protected:
  double     zoom_factor;
  bool       is_flip;
  QImage     image;
  QScrollArea * scroll_area;
  QPixmap    pixmap;
  virtual void paintEvent(QPaintEvent * par) override;
          void update_pixmap ();
          void set_zoom_factor (double factor);
public:
    wimage_viewer(QScrollArea * sa);
    void set_image  (const QString &image_path);
    void set_image  (const QImage & new_image);
    void set_pixmap (const QPixmap& new_pixmap);
    void make_pixmap();
    virtual QSize sizeHint() const override;
    const QPixmap & get_pixmap(){return pixmap;}

public  Q_SLOTS :
    void zoom_in    ();
    void zoom_out   ();
    void actual_size();
    void flip       ();

Q_SIGNALS:
  void  on_change();

};

class tmirror_circle
{
  QPoint center;
  int    radius;
  double angle;
  QPoint spot,spot_1;
  int    spot_radius;
  void   update_spots  ();
public:
      tmirror_circle():radius(0),angle(0),spot_radius(6) {}
const QPoint get_centr   (){return center;}
      int    get_radius  (){return radius;}
      void   set_center  (const QPoint pt);
      void   move_centr  (int x,int y    );
      void   set_radius  (const int rad  );
      void   paint       (QPainter & p, QPixmap &arrow_pm);
      bool   is_in_circle(const QPoint & p);
      bool   is_in_spot  (const QPoint & p);
      void   set_angle   (double _angle);
      void   set_angle   (const QPoint p);
      double get_angle   (){return angle;}
      void   change_angle(double d_angle){set_angle(angle+d_angle);}


const QRect  rect        (){int w = radius<<1     ; return  QRect(center.x()-radius,center.y()-radius,w,w);}
const QRect  spot_rect   (){int w = spot_radius<<1; return  QRect(spot.x()-spot_radius,spot.y()-spot_radius,w,w);}

};


class wimage_mirror : public wimage_viewer
{
 Q_OBJECT
 QPoint  dpos;
 tmirror_circle mrc;
 QPixmap arrows[2];
 bool    in_circle;
 bool    in_spot;
 bool    right_mirror;
 virtual void paintEvent(QPaintEvent * pv) override;
 virtual void mousePressEvent  (QMouseEvent * mv) override;
 virtual void mouseMoveEvent   (QMouseEvent * mv) override;
 virtual void mouseReleaseEvent(QMouseEvent * mv) override;
 public:
              wimage_mirror(QScrollArea *sa);
      QPixmap get_mirrored_pixmap();
      double  get_angle (){return qRadiansToDegrees(mrc.get_angle());}
      QPoint  get_centr (){return mrc.get_centr ();}
      int     get_radius(){return mrc.get_radius();}


public  Q_SLOTS :
      void    toggle_mirror(){right_mirror = !right_mirror; update(); emit on_change();}

};

#endif // IMAGE_VIEWER_H

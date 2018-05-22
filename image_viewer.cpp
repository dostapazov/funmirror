#include "image_viewer.h"


wimage_viewer::wimage_viewer(QScrollArea *sa)
            :QWidget     (nullptr)
            ,zoom_factor (1.0)
            ,is_flip     (false)
            ,scroll_area (sa)

{
 setBackgroundRole(QPalette::Background);
 setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
}

void wimage_viewer::set_image(const QString image_path)
{
    QImageReader reader(image_path);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    set_image(newImage);
}


QSize wimage_viewer::sizeHint() const
{
    if(pixmap.isNull())
      return size();
    return pixmap.size();
}

void wimage_viewer::set_image(const QImage & new_image)
{
  image = new_image;
  make_pixmap();
}

void wimage_viewer::make_pixmap()
{
  QSizeF sz = image.size();
  sz*=zoom_factor;
  pixmap = QPixmap (QSize(qRound(sz.width()),qRound(sz.height())));
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
  if(this->is_flip)
  painter.drawImage(pixmap.rect(),image.mirrored(true,false),image.rect());
  else
  painter.drawImage(pixmap.rect(),image,image.rect());
  update_pixmap();
}

void wimage_viewer::update_pixmap ()
{
    adjustSize();
    if(scroll_area)
       scroll_area->ensureWidgetVisible(this,pixmap.width()/2,pixmap.height()/2);
    update();
    emit on_change();
}

void wimage_viewer::set_pixmap (const QPixmap& new_pixmap)
{
  pixmap = new_pixmap;
  update_pixmap();
}

void wimage_viewer::set_zoom_factor (double factor)
{
    zoom_factor = factor;
    make_pixmap();
}

void wimage_viewer::zoom_in    ()
{
  set_zoom_factor(zoom_factor+0.25);
}

void wimage_viewer::zoom_out   ()
{
 set_zoom_factor(zoom_factor-0.25);
}

void wimage_viewer::actual_size()
{
 set_zoom_factor(1.0);
}

void wimage_viewer::flip()
{
 is_flip = !is_flip;
 make_pixmap();
}



void wimage_viewer::paintEvent(QPaintEvent * par)
{
  QPainter painter(this);
  if(!pixmap.isNull())
  {
      painter.drawPixmap(this->rect(),pixmap,pixmap.rect());
  }
  else
    QWidget::paintEvent(par);
}

wimage_mirror::wimage_mirror (QScrollArea * sa)
             : wimage_viewer (sa)
             , in_circle (false)
             , in_spot   (false)
             , right_mirror    (false)


{
  arrows[0] = QPixmap (tr(":/res/images/left-arrow.png" ));
  arrows[1] = QPixmap (tr(":/res/images/right-arrow.png"));
}

void wimage_mirror::paintEvent(QPaintEvent * pv)
{
    wimage_viewer::paintEvent(pv);
    if(mrc.get_radius())
    {
       QPainter painter(this);
       mrc.paint(painter, arrows[right_mirror]);
    }
}

bool tmirror_circle::is_in_circle(const QPoint & pos)
{
   int dx = (pos.x() -  this->center.x());
   int dy = (pos.y() -  this->center.y());
   return int(sqrt(double(dx*dx + dy*dy))) < radius ? true : false;
}

bool tmirror_circle::is_in_spot(const QPoint & pos)
{
   double dx1 = (pos.x() -  this->spot.x());
   double dy1 = (pos.y() -  this->spot.y());
   dx1 = qSqrt(dx1*dx1+dy1*dy1);
   return (qRound(dx1) < this->spot_radius ) ? true : false;
}

void tmirror_circle::set_angle   (double _angle)
{
    angle = _angle;
    if(qAbs(angle) > M_PI_2)
        angle = angle < 0 ? -M_PI_2 : M_PI_2;
    update_spots();
}

void tmirror_circle::set_angle   (const QPoint p)
{
    double dx = p.x() - center.x();
    double dy = p.y() - center.y();
    double _angle = qAtan2(dx,dy);
    set_angle(_angle);
}

void tmirror_circle::set_center  (const QPoint pt)
{
    center = pt;
    update_spots();
}

void tmirror_circle::set_radius  (const int rad  )
{
    radius = rad;
    update_spots();
}

void tmirror_circle::move_centr  (int x,int y    )
{
  Q_UNUSED(x); Q_UNUSED(y);
    center.rx()-=x;
    center.ry()-=y;
    update_spots();
}

void tmirror_circle::paint       (QPainter & p,QPixmap & pm)
{

  QPainter::CompositionMode cm = p.compositionMode();
  p.setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);

  QPen pen =  p.pen();
  pen.setStyle(Qt::PenStyle::SolidLine);
  pen.setWidth(2);
  p.setPen(pen);

  p.drawEllipse(rect());

  pen.setWidth(1);
  p.setPen(pen);

  pen.setStyle(Qt::PenStyle::DotLine);
  pen.setWidth(1);
  p.setPen(pen);
  p.drawLine(QLine(spot,spot_1));

//  p.setBrush(Qt::white);
//  p.drawEllipse(spot_rect());
  p.setCompositionMode(QPainter::CompositionMode_SourceOver);
  p.drawPixmap(spot.x()-pm.width()/2,spot.y()-pm.height()/2,pm);

  p.setCompositionMode(cm);
}

void tmirror_circle::update_spots  ()
{
  if(radius)
  {

    int dx = qRound(double(radius) * sin(angle));
    int dy = qRound(double(radius) * cos(angle));
    spot.setX(this->center.x()+dx);
    spot.setY(this->center.y()+dy);
    spot_1.setX(this->center.x()-dx);
    spot_1.setY(this->center.y()-dy);
  }
}

//bool wimage_mirror::eventFilter(QObject *obj, QEvent *ev)
//{
// bool ret = QWidget::eventFilter(obj,ev);
// qDebug(" %s  %d  ret %s"
//         , obj->objectName().toStdString().c_str()
//         , ev->type()
//         , ret ? "true" : "false"
//         );
// return ret;
//}

void wimage_mirror::mousePressEvent  (QMouseEvent * mv)
{
   in_circle = mrc.is_in_circle(mv->pos());
   in_spot   = mrc.is_in_spot(mv->pos());
    dpos = mv->pos();
    if(!in_spot)
    {
     if(!in_circle)
        mrc.set_center(dpos);
    }
    setMouseTracking(true);
}

void wimage_mirror::mouseMoveEvent   (QMouseEvent * mv)
{
  if(hasMouseTracking())
 {
    if(in_spot)
    {
       mrc.set_angle(mv->pos());
    }
    else
    {
     Qt::KeyboardModifiers kbm = mv->modifiers();
     if(!in_circle || kbm.testFlag(Qt::KeyboardModifier::ShiftModifier))
     {
         // Growing the circle
           dpos    = mrc.get_centr() - mv->pos();
           int dradius =  qRound(sqrt(double(dpos.x()*dpos.x() + dpos.y()*dpos.y()))) - mrc.get_radius();
           if(dradius)
           {
             mrc.set_radius( mrc.get_radius() +  dradius );
           }
     }
     else
     {
         // Moving the circle
         dpos   -= mv->pos();
         mrc.move_centr(dpos.x(),dpos.y());

     }
    }
    dpos = mv->pos();
    repaint();
    if(mrc.get_radius()) emit on_change();
 }

}

void wimage_mirror::mouseReleaseEvent(QMouseEvent * mv)
{
  Q_UNUSED(mv);
  setMouseTracking(false);
  repaint();
  if(mrc.get_radius()) emit on_change  ();
  in_circle = in_spot = false;
}

QPixmap create_mask_pixmap (int width,double rad_angle, bool right)
{
   QPixmap  pixmap (width+1,width);
   QPainter painter(&pixmap);
   Qt::GlobalColor color = Qt::black ;
   pixmap.fill     ( Qt::white );
   painter.setPen  ( color );
   painter.setBrush( color );
   int angle = qRound(16.0*qRadiansToDegrees(rad_angle));
   const int a090 = 90 <<4;
   const int a180 = 180<<4;
   painter.drawChord(0,0,width,width,angle+ (right ? -a090 : a090),a180);
   return pixmap;
}

void    get_pixmap_from(QPixmap & dst,QPixmap & src,QRect & src_rect)
{

  QPainter painter(&dst);
  painter.setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
  painter.setCompositionMode(QPainter::CompositionMode::RasterOp_SourceOrDestination);
  //painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceIn);
  painter.drawPixmap(dst.rect(),src,src_rect);
}

QPixmap  mirror_pixmap(QPixmap &  src)
{
  QImage img = src.toImage();
  QPixmap px = QPixmap::fromImage(img.mirrored(true,false));
  return px;
}

QPixmap  rotate_pixmap(QPixmap &  src,double angle)
{
 QTransform trans;
 return src.transformed(trans.rotateRadians(-angle));
}

QPixmap adjust_pixmap(QPixmap & src,int width)
{   
    QRect src_rect((src.width()-width)/2,(src.height()-width)/2,width,width);
//    return src.copy(src_rect);

    QPixmap pm(width,width);
    pm.fill(Qt::white);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
    painter.drawPixmap(pm.rect(),src,src_rect);
    return  pm;
}

void join_pixmaps2(QPixmap & p1,QPixmap & p2)
{
  QPainter painter(&p1);
  painter.setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
  QPainter::CompositionMode cm = QPainter::RasterOp_SourceAndDestination;
  painter.setCompositionMode(cm);
  painter.drawPixmap(0,0,p2);
}



QPixmap wimage_mirror::get_mirrored_pixmap()
{
  if(!pixmap.isNull() && mrc.get_radius())
  {
   QRect   srect = mrc.rect();
   double  angle = mrc.get_angle();
   QPixmap mp1   = create_mask_pixmap(srect.width(),angle,right_mirror);
   get_pixmap_from(mp1,pixmap,srect);

   mp1 = rotate_pixmap  (mp1,2.0 * M_PI - angle);
   mp1 = adjust_pixmap  (mp1,srect.width());

   QPixmap mp2   = mirror_pixmap(mp1);
   join_pixmaps2(mp1,mp2);
   return mp1;
  }
  return  QPixmap();
}


#include "mainwnd.h"
#include <QSplitter>
#include <QLayout>
#include <QtWidgets>
#include <QFileDialog>

MainWnd::MainWnd(QWidget *parent)
    : QMainWindow(parent)
    , img_mirror    (nullptr)
    , img_viewer (nullptr)
{
    create_controls();
    create_actions ();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    this->setWindowTitle(tr("Веселое зеркало"));
}

MainWnd::~MainWnd()
{
}

void MainWnd::create_controls()
{
 splitter = new QSplitter(Qt::Vertical,this);
 QScrollArea * sa = new QScrollArea;
 sa->setAlignment(Qt::AlignHCenter|Qt::AlignHCenter);

 img_mirror  = new wimage_mirror(sa);
 sa->setWidget(img_mirror);
 splitter->addWidget(sa);

 sa = new QScrollArea;
 sa->setAlignment(Qt::AlignHCenter|Qt::AlignHCenter);
 img_viewer  = new wimage_viewer(sa);

 sa->setWidget(img_viewer);
 splitter->addWidget(sa);

 splitter->setStretchFactor(0,3);
 splitter->setStretchFactor(1,2);
 setCentralWidget(splitter);
 connect(img_mirror,&wimage_mirror::on_change,this,&MainWnd::on_image_changed);
 QStatusBar * sb = new QStatusBar;
 sb->setSizeGripEnabled(true);
 setStatusBar(sb);
 statusBar()->showMessage(tr("Мышью выделить участок.Курсор на стрелке - вращение заркала. Внутри участка - перемещение.+Shift - размер"),0);
}

void MainWnd::create_actions ()
{
 QToolBar * tb = new QToolBar;
 tb->addAction(QIcon(tr(":/res/images/open_image32.png")),tr("Открыть изображение"),this,&MainWnd::open_image);

 tb->addSeparator();
 tb->addAction(QIcon(tr(":/res/images/zoom-in.png" )),tr("Увеличить 25%") ,img_mirror,&wimage_mirror::zoom_in);
 tb->addAction(QIcon(tr(":/res/images/zoom-out.png")),tr("Уменьшить 25%"),img_mirror,&wimage_mirror::zoom_out);
 tb->addAction(QIcon(tr(":/res/images/Zoom-Original.png")),tr("Реальный размер"),img_mirror,&wimage_mirror::actual_size);
 tb->addAction(QIcon(tr(":/res/images/flip-vertical.png")),tr("Сменить сторону зеркала"),img_mirror,&wimage_mirror::toggle_mirror);
 tb->addSeparator();
 tb->addAction(QIcon(tr(":/res/images/exit.png")),tr("Завершить"),this,&MainWnd::close);
 addToolBar(Qt::ToolBarArea::LeftToolBarArea,tb);
 tb = new QToolBar;
 tb->addAction(QIcon(tr(":/res/images/save_image.png")),tr("Сохранить изображение"),this,&MainWnd::save_mirrored_image);
 addToolBar(Qt::ToolBarArea::RightToolBarArea,tb);

}

void MainWnd::resizeEvent(QResizeEvent * evt)
{
   QMainWindow::resizeEvent(evt);
   Qt::Orientation new_orient = width()> height() ?  Qt::Horizontal : Qt::Vertical;
   if(new_orient != splitter->orientation())
      splitter->setOrientation(new_orient);
}

void MainWnd::open_image()
{
   QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
   if(!fileName.isEmpty())
    img_mirror->set_image(fileName);
}

void MainWnd::on_image_changed()
{
  QPixmap pm = img_mirror->get_mirrored_pixmap();
  int angle = img_mirror->get_angle();

  if(qAbs(angle)>90)
  {
    QTransform trm = QTransform().rotate(180);
    pm = pm.transformed(trm);
  }
  img_viewer->set_pixmap(pm);
}

void MainWnd::save_mirrored_image()
{
 const QPixmap & pxm = img_viewer->get_pixmap();
 if(!pxm.isNull())
  {
   QString fileName = QFileDialog::getSaveFileName(this,
                      tr("Save Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
  if(!fileName.isEmpty())
  {
    pxm.save(fileName);
  }
 }
}

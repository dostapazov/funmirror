#ifndef MAINWND_H
#define MAINWND_H

#include <QMainWindow>
#include <QAction>
#include "image_viewer.h"

class MainWnd : public QMainWindow
{
    Q_OBJECT
    wimage_mirror  * img_mirror;
    wimage_viewer  * img_viewer;
    QSplitter      * splitter;
    void create_controls();
    void create_actions ();
    virtual void resizeEvent(QResizeEvent * evt) override final;
private Q_SLOTS:
    void open_image ();
    void on_image_changed();
    void save_mirrored_image();
public:
    MainWnd(QWidget *parent = nullptr);
    ~MainWnd() override final;
};

#endif // MAINWND_H

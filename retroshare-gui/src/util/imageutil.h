#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <QTextCursor>
#include <QWidget>
#include <QImage>


class ImageUtil
{
public:
	ImageUtil();

	static void extractImage(QWidget *window, QTextCursor cursor);
	static bool checkImage(QTextCursor cursor);

	static void cropBorders(QImage& img);
};

#endif // IMAGEUTIL_H

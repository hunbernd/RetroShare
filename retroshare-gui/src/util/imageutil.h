#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <QTextCursor>
#include <QWidget>
#include <QImage>
#include <QString>


class ImageUtil
{
public:
	ImageUtil();

	static void extractImage(QWidget *window, QTextCursor cursor);
	static bool checkImage(QTextCursor cursor);

	static void cropBorders(QImage& img);
	static void optimizeSize(QString &html, const QImage& original, QImage &optimized, int maxPixels = -1, int maxBytes = -1);

private:
	static bool checkSize(QString& embeddedImage, const QImage& img, int maxBytes = -1, int quality = -1);
};

#endif // IMAGEUTIL_H

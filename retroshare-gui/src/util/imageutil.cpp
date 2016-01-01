#include "imageutil.h"
#include "util/misc.h"

#include <QMessageBox>
#include <QTextCursor>
#include <QByteArray>
#include <QString>
#include <QImage>
#include <QTextDocumentFragment>
#include <QApplication>

ImageUtil::ImageUtil() {}

void ImageUtil::extractImage(QWidget *window, QTextCursor cursor)
{
	cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
	QString imagestr = cursor.selection().toHtml();
	bool success = false;
	int start = imagestr.indexOf("base64,") + 7;
	int stop = imagestr.indexOf("\"", start);
	int length = stop - start;
	if((start >= 0) && (length > 0))
	{
		QByteArray ba = QByteArray::fromBase64(imagestr.mid(start, length).toLatin1());
		QImage image = QImage::fromData(ba);
		if(!image.isNull())
		{
			QString file;
			success = true;
			if(misc::getSaveFileName(window, RshareSettings::LASTDIR_IMAGES, "Save Picture File", "Pictures (*.png *.xpm *.jpg)", file))
			{
				if(!image.save(file, 0, 100))
					if(!image.save(file + ".png", 0, 100))
						QMessageBox::warning(window, QApplication::translate("ImageUtil", "Save image"), QApplication::translate("ImageUtil", "Cannot save the image, invalid filename"));
			}
		}
	}
	if(!success)
	{
		QMessageBox::warning(window, QApplication::translate("ImageUtil", "Save image"), QApplication::translate("ImageUtil", "Not an image"));
	}
}

bool ImageUtil::checkImage(QTextCursor cursor)
{
	cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
	QString imagestr = cursor.selection().toHtml();
	return imagestr.indexOf("base64,") != -1;
}

void ImageUtil::cropBorders(QImage &img)
{
	//Cropping algoritmh from here: http://stackoverflow.com/questions/10678015/how-to-auto-crop-an-image-white-border-in-java
	QRgb bc = img.pixel(0,0); //The top left pixel is the border color
	int l=img.width(), r=-1, t=img.height(), b=-1;
	for(int y=0; y<img.height(); ++y) {
		for(int x=0; x<img.width(); ++x) {
			if(bc != img.pixel(x, y)) {
				if(x<l) l=x;
				if(x>r) r=x;
				if(y<t) t=y;
				if(y>b) b=y;
			}
		}
	}
	if(r == -1)
		img = QImage();
	else
		img = img.copy(l, t, r-l+1, b-t+1);
}

#include "imageutil.h"
#include "util/misc.h"

#include <QMessageBox>
#include <QTextCursor>
#include <QByteArray>
#include <QString>
#include <QImage>
#include <QTextDocumentFragment>
#include <QApplication>
#include <QBuffer>
#include <QtGlobal>
#include <QtMath>
#include <iostream>

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

void ImageUtil::optimizeSize(QString &html, const QImage& original, QImage &optimized, int maxPixels, int maxBytes)
{
	optimized = original;
	if ((maxPixels <= 0) || (optimized.width()*optimized.height() <= maxPixels)) {
		if(checkSize(html, optimized, maxBytes)) {
			return;
		}
	}

	cropBorders(optimized);
	if (optimized.width()*optimized.height() <= maxPixels) {
		if(checkSize(html, optimized, maxBytes)) {
			return;
		}
	}

	QImage cropped = optimized;

	//Downscale the image to fit into maxPixels
	qreal scale = qSqrt((qreal)(maxPixels) / cropped.width() / cropped.height());
	if(scale > 1.0) scale = 1.0;

	//Check with max quality
	optimized = cropped.scaledToWidth((int)(cropped.width() * scale), Qt::SmoothTransformation);
	if(checkSize(html, optimized, maxBytes)) {
		return;
	}

	//Set quality to 0 and half the resolution until image fits into maxBytes, or width becomes 0
	do {
		optimized = cropped.scaledToWidth((int)(cropped.width() * scale), Qt::SmoothTransformation);
		scale = scale / 2.0;
	} while((!optimized.isNull()) && (!checkSize(html, optimized, maxBytes, 0)));
}

bool ImageUtil::checkSize(QString &embeddedImage, const QImage &img, int maxBytes, int quality)
{
	QByteArray bytearray;
	QBuffer buffer(&bytearray);

	std::cout << QString("Trying image: format PNG, size %1x%2, quality %3\n").arg(img.width()).arg(img.height()).arg(quality).toStdString();
	if (buffer.open(QIODevice::WriteOnly)) {
		if (img.save(&buffer, "PNG", quality)) {
			QByteArray encodedByteArray = bytearray.toBase64();
			embeddedImage = "<img src=\"data:image/png;base64,";
			embeddedImage.append(encodedByteArray);
			embeddedImage.append("\">");
			if((maxBytes > 0) && (embeddedImage.size() > maxBytes))
			{
				std::cout << QString("\tToo large, size: %1, limit: %2 bytes\n").arg(embeddedImage.size()).arg(maxBytes).toStdString();
			}else{
				std::cout << QString("\tOK, size: %1, limit: %2 bytes\n").arg(embeddedImage.size()).arg(maxBytes).toStdString();
				return true;
			}
		} else {
			std::cerr << "ImageUtil: image can't be saved to buffer" << std::endl;
		}
		buffer.close();
		bytearray.clear();
	} else {
		std::cerr << "ImageUtil: buffer can't be opened" << std::endl;
	}

	std::cout << QString("Trying image: format JPEG, size %1x%2, quality %3\n").arg(img.width()).arg(img.height()).arg(quality).toStdString();
	if (buffer.open(QIODevice::WriteOnly)) {
		if (img.save(&buffer, "JPEG", quality)) {
			QByteArray encodedByteArray = bytearray.toBase64();
			embeddedImage = "<img src=\"data:image/jpeg;base64,";
			embeddedImage.append(encodedByteArray);
			embeddedImage.append("\">");
			if((maxBytes > 0) && (embeddedImage.size() > maxBytes))
			{
				std::cout << QString("\tToo large, size: %1, limit: %2 bytes\n").arg(embeddedImage.size()).arg(maxBytes).toStdString();
			}else{
				std::cout << QString("\tOK, size: %1, limit: %2 bytes\n").arg(embeddedImage.size()).arg(maxBytes).toStdString();
				return true;
			}
		} else {
			std::cerr << "ImageUtil: image can't be saved to buffer" << std::endl;
		}
		buffer.close();
		bytearray.clear();
	} else {
		std::cerr << "ImageUtil: buffer can't be opened" << std::endl;
	}

	embeddedImage.clear();
	return false;
}

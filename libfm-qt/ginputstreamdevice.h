/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef FM_GINPUTSTREAMDEVICE_H
#define FM_GINPUTSTREAMDEVICE_H

#include "libfmqtglobals.h"
#include <QIODevice>
#include <gio/gio.h>

namespace Fm {

class LIBFM_QT_API GInputStreamDevice : public QIODevice {
public:
  GInputStreamDevice(GInputStream* stream, guint64 size, GCancellable* cancellable);
  ~GInputStreamDevice();

  virtual bool isSequential();
  virtual void close();
  virtual qint64 size();
  virtual bool seek(qint64 pos);
  virtual bool atEnd();
  virtual bool reset();
  virtual qint64 bytesAvailable();

protected:
  virtual qint64 writeData(const char* data, qint64 len);
  virtual qint64 readData(char* data, qint64 maxlen);
  
private:
  GInputStream* stream_;
  GCancellable* cancellable_;
  qint64 seekStartPos_;
  qint64 pos_;
  qint64 size_;
};

}

#endif // FM_GINPUTSTREAMDEVICE_H

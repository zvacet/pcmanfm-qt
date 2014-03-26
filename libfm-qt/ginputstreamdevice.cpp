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

#include "ginputstreamdevice.h"

using namespace Fm;

GInputStreamDevice::GInputStreamDevice(GInputStream* stream, guint64 size, GCancellable* cancellable):
  QIODevice(),
  pos_(0),
  seekStartPos_(G_IS_SEEKABLE(stream) ? g_seekable_tell(G_SEEKABLE(stream)) :  -1),
  size_(size),
  cancellable_(G_CANCELLABLE(g_object_ref(cancellable))),
  stream_(G_INPUT_STREAM(g_object_ref(stream))) {
   
  // open(QIODevice::ReadOnly|QIODevice::Unbuffered);
}

GInputStreamDevice::~GInputStreamDevice() {
  if(stream_)
    close();
  if(cancellable_)
    g_object_unref(cancellable_);
}

qint64 GInputStreamDevice::writeData(const char* data, qint64 len) {
  // Not supported
  return 0;
}

qint64 GInputStreamDevice::readData(char* data, qint64 maxlen) {
  qDebug("call readData: %lld, pos: %lld, size: %lld", maxlen, pos_, size_);
  if(pos_ + maxlen > size_) {
    maxlen = size_ - pos_;
  }
  if(maxlen > 0) {
    qint64 ret = g_input_stream_read(stream_, data, maxlen, cancellable_, NULL);
    if(ret > 0) {
      pos_ += ret;
      qDebug("readData: %lld, pos: %lld", ret, pos_);
      return ret;
    }
  }
  qDebug("readData failed");
  return 0;
}

bool GInputStreamDevice::isSequential() {
  return (seekStartPos_ != -1);
}

void GInputStreamDevice::close() {
  QIODevice::close();
  qDebug("close");
  if(stream_) {
    g_object_unref(stream_);
    stream_ = NULL;
  }
}

qint64 GInputStreamDevice::size() {
  qDebug("size: %lld", size_);
  return size_;
}

bool GInputStreamDevice::seek(qint64 pos) {
  qDebug("seek: %lld, startPos: %lld, stream: %p, pos: %lld", pos, seekStartPos_, stream_, pos_);
  if(seekStartPos_ != -1) { // the stream is seekable
    GError* err = NULL;
    // if(g_seekable_seek(G_SEEKABLE(stream_), pos - pos_, G_SEEK_CUR, cancellable_, &err)) {
    if(g_seekable_seek(G_SEEKABLE(stream_), seekStartPos_ + pos, G_SEEK_SET, cancellable_, &err)) {
      pos_ = pos;
      QIODevice::seek(pos);
      qDebug("seek to %lld, real pos: %lld", g_seekable_tell(G_SEEKABLE(stream_)));
      return true;
    }
    if(err) {
      qDebug("seek error: %s", err->message);
      g_error_free(err);
    }
  }
  return false;
}

bool GInputStreamDevice::atEnd() {
  qDebug("atEnd");
  return (pos_ >= size_);
}

bool GInputStreamDevice::reset() {
  qDebug("reset");
  return false;
}

qint64 GInputStreamDevice::bytesAvailable() {
  qDebug("bytesAvailable: %lld, %lld", QIODevice::bytesAvailable(), size_ - pos_);
  return QIODevice::bytesAvailable() + (size_ - pos_);
}

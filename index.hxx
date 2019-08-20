#ifndef PUSH_STREAM_H
#define PUSH_STREAM_H

#include <string>
#include <iostream>
#include <vector>
#include <any>

struct Stream {
  bool ended = false;
  bool aborted = false;
  bool paused = false;
  bool hasSink = false;

  Stream* source;
  Stream* sink;

  virtual int write (const std::any& data) {
    return 0;
  };

  virtual bool resume () {
    return true;
  };

  virtual void pipe () {};

  virtual bool end () {
    return true;
  };
};

Stream& operator | (Stream& source, Stream& sink) {
  sink.source = dynamic_cast<Stream*>(&source);
  source.sink = dynamic_cast<Stream*>(&sink);

  sink.source->hasSink = true;
  sink.source->pipe();
  return sink;
}

#endif

#include <vector>
#include <string>
#include <iostream>

#include "../deps/heapwolf/cxx-tap/index.hxx"
#include "../index.hxx"

using String = std::string;
using Any = std::any;
using Buffer = std::vector<String>;
using Callback = std::function<String(String)>;

struct Source : Stream {
  Buffer buf { "paper", "clips", "for", "sale" };
	size_t i = 0;

  bool resume () override {
    if(!this->hasSink || this->ended) {
      return false;
    }

    while (!this->sink->paused && i < buf.size()) {
      auto s = buf[i++];
      this->sink->write(s);
    }

    if (this->i == this->buf.size()) {
      this->sink->end();
    }

    return true;
  }

  void pipe () override {
    this->resume();
  }
};

struct Sink : Stream {
  Buffer buf;

  bool end () override {
    this->ended = true;
    return true;
  }

  int write (const Any& data) override {
    auto str = std::any_cast<String>(data);
    buf.push_back(str);
    return str.size();
  }
};

struct Through : Stream {
  Callback callback;

  Through(Callback cb) : callback(cb) {
    this->paused = true;
  };

  int write (const Any& data) override {
    auto str = std::any_cast<String>(data);
    auto s = callback(str);

    if (!this->hasSink) {
      return 0;
    }

    this->sink->write(s);
    return str.size();
  }

  bool end () override {
    this->sink->end();
    return true;
  }

  bool resume () override {
    this->source->resume();
    return true;
  }

  void pipe () override {
    this->paused = this->sink->paused;

    if (!this->paused) {
      this->resume();
    }
  }
};

int main () {
  TAP::Test t;

  t.test("pipe", [&](auto t) {
    Source source;
    Sink sink;

    t->equal(sink.ended, false, "sink has not yet ended");

    source | sink;

    t->equal(sink.buf.size(), 4, "sink buffer equal in size");
    t->equal(sink.ended, true, "sink has ended");
    t->end();
  });

  t.test("pipe through", [&](auto t) {
    Source source;
    Sink sink;

    Through through([](String str) -> String {
      for (size_t i = 0; i < str.size(); i++) {
        str[i] = toupper(str[i]);
      }

      return str;
    });

    t->equal(sink.ended, false, "sink has not yet ended");

    source | through | sink;

    t->equal(sink.buf.size(), 4, "sink buffer equal in size");
    t->equal(sink.ended, true, "sink has ended");

    Buffer buf { "PAPER", "CLIPS", "FOR", "SALE" };

    for (size_t i = 0; i < sink.buf.size(); i++) {
      t->equal(sink.buf[i], buf[i], "buffers match");
    }

    t->end();
  });
}

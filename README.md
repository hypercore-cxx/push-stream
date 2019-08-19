# SYNOPSIS

`push-stream` is a streams implementation based on this streams [spec][1]. There
is a minimal base-class that implements pipe and adds placeholders for typical
properties and methods that will be used in implementing the usual `Source`,
`Sync`, `Through` and `Duplex` streams.

The base class overloads the `|` operator to improve readability, ie you can do 
this...

```c++
int main () {
  source | through | sink;
  return 0;
}
```


# USAGE
This module is designed to work with the [`datcxx`][0] build tool. To add this
module to your project us the following command...

```bash
build add datcxx/push-stream
```


# TEST

```bash
build test
```

# EXAMPLES

### SOURCE EXAMPLE

```c++
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

    if (i == buf.size()) {
      this->sink->end();
    }

    return true;
  }

  void pipe () override {
    this->resume();
  }
};
```

### SINK EXAMPLE

```c++
struct Sink : Stream {
  bool end () override {
    this->ended = true;
    return true;
  }

  int write (const std::any& data) override {
    cout << std::any_cast<String>(data);
    return str.size();
  }
};
```

[0]:https://github.com/datcxx/build
[1]:https://github.com/push-stream/push-stream

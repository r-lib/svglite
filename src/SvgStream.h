#ifndef __SVG_STREAM__
#define __SVG_STREAM__

#include <fstream>
#include <sstream>
#include <Rcpp.h>

class SvgStream {
  public:

  virtual ~SvgStream() {};

  virtual void write(int data) = 0;
  virtual void write(double data) = 0;
  virtual void write(const char* data) = 0;
  virtual void write(const std::string& data) = 0;
  virtual void write(char data) = 0;

  void put(char data) {
    write(data);
  }

  virtual void flush() = 0;
  virtual void finish() = 0;
};

template <typename T>
SvgStream& operator<<(SvgStream& object, const T& data) {
  object.write(data);
  return object;
}

class SvgStreamFile : public SvgStream {
  std::ofstream stream_;

public:
  SvgStreamFile(const std::string& path) {
    stream_.open(R_ExpandFileName(path.c_str()));

    if (stream_.fail())
      Rcpp::stop("cannot open stream " + path);

    stream_ << std::fixed << std::setprecision(2);
  }

  void write(int data)            { stream_ << data; }
  void write(double data)         { stream_ << data; }
  void write(const char* data)    { stream_ << data; }
  void write(char data)           { stream_ << data; }
  void write(const std::string& data) { stream_ << data; }

  void flush() {
    stream_ << "</svg>";
    stream_.seekp(-6, std::ios_base::cur);
    stream_.flush();
  }

  void finish() {
  }

  ~SvgStreamFile() {
    stream_.close();
  }
};


class SvgStreamString : public SvgStream {
  std::stringstream stream_;
  Rcpp::Environment env_;

public:
  SvgStreamString(Rcpp::Environment env): env_(env) {
    stream_ << std::fixed << std::setprecision(2);
  }

  void write(int data)                { stream_ << data; }
  void write(double data)             { stream_ << data; }
  void write(const char* data)        { stream_ << data; }
  void write(char data)               { stream_ << data; }
  void write(const std::string& data) { stream_ << data; }

  void seek(int n) {
    stream_.seekg(n, std::ios_base::cur);
  }

  void flush() {
    stream_.flush();
    env_["svg_string"] = stream_.str() + "</svg>";
  }

  void finish() {
    flush();
  }


};


#endif

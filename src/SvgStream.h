#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cpp11/protect.hpp>
#include <cpp11/environment.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/r_string.hpp>
#include <cpp11/as.hpp>
#include <cpp11/function.hpp>
#include <unordered_set>
#include "utils.h"

namespace svglite { namespace internal {

template <typename T>
void write_double(T& stream, double data) {
  std::streamsize prec = stream.precision();
  uint8_t newprec = std::fabs(data) >= 1 || data == 0. ? prec : std::ceil(-std::log10(std::fabs(data))) + 1;
  stream << std::setprecision(newprec) << data << std::setprecision(prec);
}

}} // namespace svglite::internal


class SvgStream {
  std::unordered_set<std::string> clip_ids;
  bool clipping = false;

  public:

  bool has_clip_id(std::string id) {
    return clip_ids.find(id) != clip_ids.end();
  }
  void set_clipping(bool clip) {
    clipping = clip;
  }
  void add_clip_id(std::string id) {
    clip_ids.insert(id);
  }
  void clear_clip_ids() {
    clip_ids.clear();
  }
  bool is_clipping() {return clipping;}

  virtual ~SvgStream() {};

  virtual void write(int data) = 0;
  virtual void write(double data) = 0;
  virtual void write(const char* data) = 0;
  virtual void write(const std::string& data) = 0;
  virtual void write(char data) = 0;
  virtual bool is_file_stream() = 0;

  void put(char data) {
    write(data);
  }

  virtual void flush() = 0;
  virtual void finish(bool close) = 0;
};

template <typename T>
SvgStream& operator<<(SvgStream& object, const T& data) {
  object.write(data);
  return object;
}

class SvgStreamFile : public SvgStream {
  std::ofstream stream_;
  bool compress = false;
  std::string file = "";
  bool always_valid = false;

public:
  SvgStreamFile(const std::string& path, bool _always_valid = false) : always_valid(_always_valid) {
    std::string svgz_ext = path.size() > 5 ? path.substr(path.size() - 5) : "";
    std::string gz_ext = path.size() > 3 ? path.substr(path.size() - 3) : "";
    compress = iequals(svgz_ext, ".svgz") || iequals(gz_ext, ".gz");
    file = R_ExpandFileName(path.c_str());

    stream_.open(file.c_str());

    if (stream_.fail())
      cpp11::stop("cannot open stream %s", path.c_str());

    stream_ << std::fixed << std::setprecision(2);
  }

  SvgStreamFile(const std::string& path, int pageno, bool _always_valid = false) : always_valid(_always_valid) {
    std::string svgz_ext = path.size() > 5 ? path.substr(path.size() - 5) : "";
    std::string gz_ext = path.size() > 3 ? path.substr(path.size() - 3) : "";
    compress = iequals(svgz_ext, ".svgz") || iequals(gz_ext, ".gz");

    char buf[PATH_MAX+1];
    snprintf(buf, PATH_MAX, path.c_str(), pageno);
    buf[PATH_MAX] = '\0';
    file = R_ExpandFileName(buf);

    stream_.open(file.c_str());
    if (stream_.fail())
      cpp11::stop("cannot open stream %s", buf);

    stream_ << std::fixed << std::setprecision(2);
  }

  void write(int data)            { stream_ << data; }
  void write(double data)         { svglite::internal::write_double(stream_, data); }
  void write(const char* data)    { stream_ << data; }
  void write(char data)           { stream_ << data; }
  void write(const std::string& data) { stream_ << data; }
  bool is_file_stream() {return true; }

  // Adding a final newline here creates problems on Windows when
  // seeking back to original position. So we only write the newline
  // in finish()
  void flush() {
    if (!always_valid) {
      return;
    }
#ifdef _WIN32
    int offset = -12;
#else
    int offset = -11;
#endif

    if (is_clipping()) {
      // We don't do newline here just to avoid having to deal with windows
      stream_ << "</g>";
      offset -= 4;
    }
    stream_ << "</g>\n</svg>";
    stream_.seekp(offset, std::ios_base::cur);
  }

  void finish(bool close) {
    const auto compressor = cpp11::package("svglite")["create_svgz"];

    if (is_clipping()) {
      stream_ << "</g>\n";
    }
    stream_ << "</g>\n</svg>\n";
    stream_.flush();
    clear_clip_ids();

    if (compress) {
      compressor(cpp11::r_string(file));
    }
  }

  ~SvgStreamFile() {
    stream_.close();
  }
};


class SvgStreamString : public SvgStream {
  std::stringstream stream_;
  cpp11::environment env_;

public:
  SvgStreamString(cpp11::environment env): env_(env) {
    stream_ << std::fixed << std::setprecision(2);
    env_["is_closed"] = false;
  }

  void write(int data)                { stream_ << data; }
  void write(double data)             { svglite::internal::write_double(stream_, data); }
  void write(const char* data)        { stream_ << data; }
  void write(char data)               { stream_ << data; }
  void write(const std::string& data) { stream_ << data; }
  bool is_file_stream() {return false; }

  void flush() {
  }

  void finish(bool close) {
    // When device is closed, stream_ will be destroyed, so we can no longer
    // get the svg string from stream_. In this case, we save the final string
    // to the environment env, so that R can read from env$svg_string even
    // after device is closed.
    env_["is_closed"] = close;

    stream_.flush();
    std::string svgstr = stream_.str();
    // If the current svg is empty, we also make the string empty
    // Otherwise append "</svg>" to make it a valid SVG
    if(!svgstr.empty()) {
      if (is_clipping()) {
        svgstr.append("</g>\n");
      }
      svgstr.append("</g>\n</svg>");
    }
    if (env_.exists("svg_string")) {
      cpp11::writable::strings str(env_["svg_string"]);
      str.push_back(svgstr.c_str());
      env_["svg_string"] = str;
    } else {
      env_["svg_string"] = svgstr;
    }

    // clear the stream
    stream_.str(std::string());
    stream_.clear();
    clear_clip_ids();
  }

  std::stringstream* string_src() {
    return &stream_;
  }
};

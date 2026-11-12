#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cpp11/protect.hpp>
#include <cpp11/environment.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/r_string.hpp>
#include <cpp11/as.hpp>
#include <cpp11/function.hpp>
#include <unordered_set>
#include "utils.h"
#include <cstdint>

namespace svglite { namespace internal {

// Mirrors the old iostream-based formatting: std::fixed with precision 2 for
// values >= 1 (and 0), and just enough decimals to keep one significant
// digit for values < 1
inline void write_double(std::string& buf, double data) {
  int prec = 2;
  double abs_data = std::fabs(data);
  if (abs_data < 1 && data != 0.) {
    prec = (int)std::ceil(-std::log10(abs_data)) + 1;
    prec = prec > UINT8_MAX ? UINT8_MAX : prec;
  }
  char tmp[512];
  int len = std::snprintf(tmp, sizeof(tmp), "%.*f", prec, data);
  buf.append(tmp, len);
}

inline void write_int(std::string& buf, int data) {
  char tmp[12];
  int len = std::snprintf(tmp, sizeof(tmp), "%d", data);
  buf.append(tmp, len);
}

}} // namespace svglite::internal


class SvgStream {
  std::unordered_set<std::string> clip_ids;
  bool clipping = false;

  protected:
  // All element output is accumulated here; subclasses flush it to their
  // backing store (file, R string) as needed
  std::string buffer_;

  public:

  bool has_clip_id(const std::string& id) {
    return clip_ids.find(id) != clip_ids.end();
  }
  void set_clipping(bool clip) {
    clipping = clip;
  }
  void add_clip_id(const std::string& id) {
    clip_ids.insert(id);
  }
  void clear_clip_ids() {
    clip_ids.clear();
  }
  void merge_clip_ids_from(SvgStream& other) {
    clip_ids.insert(other.clip_ids.begin(), other.clip_ids.end());
  }
  bool is_clipping() {return clipping;}

  virtual ~SvgStream() {};

  void write(int data)                { svglite::internal::write_int(buffer_, data); }
  void write(double data)             { svglite::internal::write_double(buffer_, data); }
  void write(const char* data)        { buffer_.append(data); }
  void write(const std::string& data) { buffer_.append(data); }
  void write(char data)               { buffer_.push_back(data); }
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
  }

  bool is_file_stream() {return true; }

  void flush_buffer() {
    if (!buffer_.empty()) {
      stream_ << buffer_;
      buffer_.clear();
    }
  }

  // Adding a final newline here creates problems on Windows when
  // seeking back to original position. So we only write the newline
  // in finish()
  void flush() {
    flush_buffer();
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

    flush_buffer();
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


// A plain in-memory stream used to buffer output while control is handed
// back to R (e.g. when recording patterns, masks, clip paths, and groups).
// The buffered output is only folded into the real stream once the R
// callback has completed without error.
class SvgStreamBuffer : public SvgStream {

public:
  SvgStreamBuffer() {
  }

  bool is_file_stream() {return false; }

  void flush() {
  }

  void finish(bool close) {
  }

  const std::string& str() {
    return buffer_;
  }
};


class SvgStreamString : public SvgStream {
  cpp11::environment env_;

public:
  SvgStreamString(cpp11::environment env): env_(env) {
    env_["is_closed"] = false;
  }

  bool is_file_stream() {return false; }

  void flush() {
  }

  void finish(bool close) {
    // When device is closed, buffer_ will be destroyed, so we can no longer
    // get the svg string from buffer_. In this case, we save the final string
    // to the environment env, so that R can read from env$svg_string even
    // after device is closed.
    env_["is_closed"] = close;

    std::string svgstr = buffer_;
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

    // clear the buffer
    buffer_.clear();
    clear_clip_ids();
  }

  // Current SVG content, closed off so that it is valid XML
  std::string get_content() {
    std::string svgstr = buffer_;
    // If the current svg is empty, we also make the string empty
    // Otherwise append "</svg>" to make it a valid SVG
    if (!svgstr.empty()) {
      if (is_clipping()) {
        svgstr.append("</g>\n");
      }
      svgstr.append("</g>\n</svg>");
    }
    return svgstr;
  }
};
